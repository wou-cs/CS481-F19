#include "SurfaceOfRevolution.h"
#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>
#include <IvResourceManager.h>
#include <IvRenderer.h>
#include <IvVector3.h>
#include <IvColor.h>
#include <IvTexture.h>
#include <IvImage.h>
#include <IvStackAllocator.h>
#include <IvFileReader.h>
#include <IvMatrix33.h>
#include <IvMath.h>
#include <iostream>

void printProfile(IvVector3* arr, size_t n)
{
	for (size_t i = 0; i < n; ++i)
	{
		std::cout << arr[i].x << " " << arr[i].y << " " << arr[i].z << std::endl;
	}
}

SurfaceOfRevolution::SurfaceOfRevolution(const char* filename, unsigned int nPoints, const char* textureFilename)
	: mDataPtr(0),mIndexPtr(0),mVertices(0),mIndices(0),mNumVertices(0),mNumTriangles(0)
{
	//std::cout << "Surface of revolution" << std::endl;
	// Load texture image
	IvImage* image = IvImage::CreateFromFile(textureFilename);
	if (image)
	{
		mTexture = IvRenderer::mRenderer->GetResourceManager()->CreateTexture(
			(image->GetBytesPerPixel() == 4) ? kRGBA32TexFmt : kRGB24TexFmt,
			image->GetWidth(), image->GetHeight(), image->GetPixels(), kImmutableUsage);

		delete image;
		image = 0;
	}
	else
	{
		std::cout << "Failed to load texture image" << std::endl;
	}

	BuildSurface(filename, nPoints);
	
}

SurfaceOfRevolution::~SurfaceOfRevolution()
{
	// clean up
	delete[] mDataPtr;
	delete[] mIndexPtr;
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mVertices);
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mIndices);
}

void SurfaceOfRevolution::BuildSurface(const char* filename, unsigned int nPoints)
{
	// Assumes mTexture has been loaded

	IvFileReader in(filename);
	if (!in)
	{
		return;
	}
	// Get number of points
	size_t n;
	in >> n;
	if (!in.good() && n > 1)
	{
		return;
	}

	// Read in the profile (x,z) of the surface to create
	// If the first point is 0,0 then it uses only triangles to create the first "row"
	// and the surface is continuous across the origin (i.e. a cup), otherwise it uses
	// a triangulated quad for all rows and the surface is a revolution about the origin
	// but doesn't contain the origin, i.e. a torus
	IvVector3* profile = new IvVector3[n];
	for (size_t i = 0; i < n; ++i)
	{
		float x, z;
		in >> x >> z;
		profile[i].Set(x, 0.0f, z);
	}

	bool firstPointIsOrigin = false;
	if (profile[0].IsZero())
	{
		firstPointIsOrigin = true;
	}

	// Set up rotation matrix, to rotate profile for each angle
	IvMatrix33 rotate;
	size_t ncircumference = (size_t)nPoints;
	float angle = kTwoPI / ncircumference;
	rotate.RotationZ(angle);

	//printProfile(profile, n);

	// Create geometry
	mNumVertices = ncircumference * n;
	if (firstPointIsOrigin)
	{
		mNumTriangles = ncircumference * (n - 1) * 2 - ncircumference;	// for first row a fan
	}
	else
	{
		mNumTriangles = ncircumference * (n - 1) * 2;		// first row is a pair of triangles like the rest
	}

	mDataPtr = new IvTNPVertex[mNumVertices];
	mIndexPtr = new UInt32[mNumTriangles * 3];

	//std::cout << mNumVertices << " vertices, " << mNumTriangles << " triangles" << std::endl;

	// Build vertices, texture u,v and triangle indices.  Really need to draw a picture of the 
	// vertices and indices to understand this
	size_t idx = 0;
	float texU = 0.0f;
	float texV = 0.0f;
	for (size_t i = 0; i < ncircumference; ++i)
	{
		texU = i / (float)(ncircumference - 1);
		for (size_t j = 0; j < n; ++j)
		{
			size_t index = i * n + j;
			texV = j / (float)(n - 1);		// assumes points in the profile are evenly spaced
			// Vertices
			mDataPtr[index].position = profile[j];
			mDataPtr[index].texturecoord.Set(texU, texV);
			mDataPtr[index].normal.Set(0.0f, 0.0f, 0.0f);	// normals are calculated below
			// Indices
			if (j < n - 1)
			{
				if (j == 0 && firstPointIsOrigin)
				{
					mIndexPtr[idx++] = 0;
					mIndexPtr[idx++] = (UInt32)((index + (n + 1)) % mNumVertices);
					mIndexPtr[idx++] = (UInt32)(index + 1);
				}
				else
				{
					mIndexPtr[idx++] = (UInt32)(index);
					mIndexPtr[idx++] = (UInt32)((index + (n + 1)) % mNumVertices);
					mIndexPtr[idx++] = (UInt32)(index + 1);
					mIndexPtr[idx++] = (UInt32)(index);
					mIndexPtr[idx++] = (UInt32)((index + n) % mNumVertices);
					mIndexPtr[idx++] = (UInt32)((index + n + 1) % mNumVertices);
				}
				assert(idx - 1 < mNumTriangles * 3);
			}
		}
		// rotate profile to be ready for next iteration
		for (size_t j = 0; j < n; ++j)
		{
			profile[j] = rotate * profile[j];
		}
	}

	// Assign normals based on average of normals of all triangles using that vertex,
	// just add them up and then normalize
	for (size_t i = 0; i < mNumTriangles * 3; i += 3)
	{
		IvVector3 P = mDataPtr[mIndexPtr[i]].position;
		IvVector3 Q = mDataPtr[mIndexPtr[i + 1]].position;
		IvVector3 R = mDataPtr[mIndexPtr[i + 2]].position;
		IvVector3 vNorm = (Q - P).Cross(R - P);
		vNorm.Normalize();
		mDataPtr[mIndexPtr[i]].normal += vNorm;
		mDataPtr[mIndexPtr[i + 1]].normal += vNorm;
		mDataPtr[mIndexPtr[i + 2]].normal += vNorm;
	}
	// and then we need to normalize all of them
	for (size_t i = 0; i < mNumVertices; ++i)
	{
		mDataPtr[i].normal.Normalize();
	}

	// Transfer data to the OpenGL server (i.e. graphics card)
	mVertices = IvRenderer::mRenderer->GetResourceManager()
		->CreateVertexBuffer(kTNPFormat, (unsigned int)mNumVertices, mDataPtr, kDefaultUsage);
	mIndices = IvRenderer::mRenderer->GetResourceManager()
		->CreateIndexBuffer((unsigned int)(mNumTriangles * 3), mIndexPtr, kDefaultUsage);

	delete[] profile;
}

IvTexture* SurfaceOfRevolution::GetTexture()
{
	return mTexture;
}

void SurfaceOfRevolution::Draw()
{
	IvRenderer::mRenderer->Draw(kTriangleListPrim, mVertices, mIndices);
	//IvRenderer::mRenderer->Draw(kPointListPrim, mVertices, (unsigned int)mNumVertices);
}

