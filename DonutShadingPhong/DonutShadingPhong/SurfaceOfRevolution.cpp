#include "SurfaceOfRevolution.h"
#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>
#include <IvResourceManager.h>
#include <IvRenderer.h>
#include <IvVector3.h>
#include <IvColor.h>
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

SurfaceOfRevolution::SurfaceOfRevolution(const char* filename, unsigned int nPoints)
	: mDataPtr(0),mIndexPtr(0),mVertices(0),mIndices(0),mNumVertices(0),mNumTriangles(0)
{
	//std::cout << "Surface of revolution" << std::endl;
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
	
	mDataPtr = new IvCNPVertex[mNumVertices];
	mIndexPtr = new UInt32[mNumTriangles * 3];

	//std::cout << mNumVertices << " vertices, " << mNumTriangles << " triangles" << std::endl;

	// Build vertices, colors and triangle indices.  Really need to draw a picture of the 
	// vertices and indices to understand this
	size_t idx = 0;
	for (size_t i = 0; i < ncircumference; ++i)
	{
		for (size_t j = 0; j < n; ++j)
		{
			size_t index = i * n + j;
			// Vertices
			mDataPtr[index].position = profile[j];
			mDataPtr[index].color.Set(80, 220, 50, 0);		// change hard coded color here
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
				assert(idx-1 < mNumTriangles * 3);
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
		IvVector3 Q = mDataPtr[mIndexPtr[i+1]].position;
		IvVector3 R = mDataPtr[mIndexPtr[i+2]].position;
		IvVector3 vNorm = (Q - P).Cross(R - P);
		vNorm.Normalize();
		mDataPtr[mIndexPtr[i]].normal += vNorm;
		mDataPtr[mIndexPtr[i+1]].normal += vNorm;
		mDataPtr[mIndexPtr[i+2]].normal += vNorm;
	}
	// and then we need to normalize all of them
	for (size_t i = 0; i < mNumVertices; ++i)
	{
		mDataPtr[i].normal.Normalize();
	}

	// Transfer data to the OpenGL server (i.e. graphics card)
	mVertices = IvRenderer::mRenderer->GetResourceManager()
		->CreateVertexBuffer(kCNPFormat, (unsigned int)mNumVertices, mDataPtr, kDefaultUsage);
	mIndices = IvRenderer::mRenderer->GetResourceManager()
		->CreateIndexBuffer((unsigned int)(mNumTriangles * 3), mIndexPtr, kDefaultUsage);

	delete[] profile;
}



SurfaceOfRevolution::~SurfaceOfRevolution()
{
	// clean up
	delete[] mDataPtr;
	delete[] mIndexPtr;
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mVertices);
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mIndices);
}

void SurfaceOfRevolution::Draw()
{
	IvRenderer::mRenderer->Draw(kTriangleListPrim, mVertices, mIndices);
	//IvRenderer::mRenderer->Draw(kPointListPrim, mVertices, (unsigned int)mNumVertices);
}

/*
// Create geometry
	mNumVertices = ncircumference * (n - 1) + 1;
	mNumTriangles = ncircumference * (2 * n - 3);
	mDataPtr = new IvCNPVertex[mNumVertices];
	mIndexPtr = new UInt32[mNumTriangles * 3];

	size_t index = 0;
	// origin first, and separately, since it is fixed (everything
	// rotates around it)
	mDataPtr[index].position = profile[0];
	mDataPtr[index].color.Set(200, 50, 50, 255);
	mDataPtr[index++].normal.Set(0.0f, 0.0f, -1.0f);
	// and for all the other vertices
	for (size_t i = 0; i < ncircumference; ++i)
	{
		for (size_t j = 1; j < n; ++j)
		{
			assert(index < mNumVertices);
			mDataPtr[index].position = profile[j];
			mDataPtr[index].color.Set(200, 50, 50, 255);
			mDataPtr[index++].normal.Set(0.5f, 0.5f, 0.0f);
		}
		// rotate profile
		for (size_t j = 0; j < n; ++j)
		{
			profile[j] = rotate * profile[j];
		}
	}
*/