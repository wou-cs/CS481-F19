#include "ModelFromObj.h"
#include <iostream>
#include <algorithm>
#include <IvAssert.h>
#include <vector>
#include <IvRenderer.h>
#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>
#include <IvResourceManager.h>


// TODO: Refactor main model loading code into a separate method
// TODO: Load a texture file and see if uv's are correct

ModelFromObj::ModelFromObj(const std::string & filename)
{
	mObj = new tinyobj::ObjReader();
	struct tinyobj::ObjReaderConfig objrc;
	objrc.vertex_color = false;
	objrc.triangulate = false;
	if (!mObj->ParseFromFile(filename, objrc))
	{
		std::cout << "Obj file could not be parsed" << std::endl;
		std::cout << mObj->Error() << std::endl;
		return;
	}
	// print out warnings, which may be present even if file was loaded
	std::cout << mObj->Warning() << std::endl;
	mNumVertices	= mObj->GetAttrib().vertices.size();
	mNumNormals		= mObj->GetAttrib().normals.size();
	mNumTexcoords	= mObj->GetAttrib().texcoords.size();
	size_t maxNumAttributes = std::max(std::max(mNumVertices, mNumNormals), mNumTexcoords);

	std::cout << "Number of vertices : " << mNumVertices << std::endl;
	std::cout << "Number of normals  : " << mNumNormals << std::endl;
	std::cout << "Number of texcoords: " << mNumTexcoords << std::endl;

	// Not reading materials yet even if they're present, .mtl files

	// Shapes (obj can define multiple objects in one file)
	std::cout << "Number of shapes   : " << mObj->GetShapes().size() << std::endl;
	std::cout << "  For shape #1     : \n";
	const tinyobj::shape_t & shape0 = mObj->GetShapes()[0];
	std::cout << "     Name       : " << shape0.name << std::endl;
	std::cout << "     Num indices: " << shape0.mesh.indices.size() << std::endl;
	std::cout << "     Num numface: " << shape0.mesh.num_face_vertices.size() << std::endl;

	//mDataPtr = new IvTNPVertex[maxNumAttributes];

	const tinyobj::mesh_t& mesh = shape0.mesh;
	const tinyobj::attrib_t& attrib = mObj->GetAttrib();
	//mNumTriangles = mesh.num_face_vertices.size();
	//mIndexPtr = new UInt32[mNumTriangles * 3];

	// Check for smoothing group and compute smoothing normals
	std::map<int, vec3> smoothVertexNormals;
	if (hasSmoothingGroup(shape0) > 0) 
	{
		std::cout << "Compute smoothingNormal for shape" << std::endl;
		computeSmoothingNormals(attrib, shape0, smoothVertexNormals);
	}

	for (size_t f = 0; f < mesh.indices.size() / 3; f++) 
	{
		tinyobj::index_t idx0 = mesh.indices[3 * f + 0];
		tinyobj::index_t idx1 = mesh.indices[3 * f + 1];
		tinyobj::index_t idx2 = mesh.indices[3 * f + 2];

		// texture coordinates
		float tc[3][2];
		if (attrib.texcoords.size() > 0)
		{
			if ((idx0.texcoord_index < 0) || (idx1.texcoord_index < 0) ||
				(idx2.texcoord_index < 0)) 
			{
				// face does not contain valid uv index.
				tc[0][0] = 0.0f;
				tc[0][1] = 0.0f;
				tc[1][0] = 0.0f;
				tc[1][1] = 0.0f;
				tc[2][0] = 0.0f;
				tc[2][1] = 0.0f;
			}
			else 
			{
				assert(attrib.texcoords.size() > size_t(2 * idx0.texcoord_index + 1));
				assert(attrib.texcoords.size() > size_t(2 * idx1.texcoord_index + 1));
				assert(attrib.texcoords.size() > size_t(2 * idx2.texcoord_index + 1));

				// Flip Y coord.
				tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
				tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
				tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
				tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
				tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
				tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
			}
		}
		else 
		{
			tc[0][0] = 0.0f;
			tc[0][1] = 0.0f;
			tc[1][0] = 0.0f;
			tc[1][1] = 0.0f;
			tc[2][0] = 0.0f;
			tc[2][1] = 0.0f;
		}

		// Vertices
		float v[3][3];
		for (int k = 0; k < 3; k++) 
		{
			int f0 = idx0.vertex_index;
			int f1 = idx1.vertex_index;
			int f2 = idx2.vertex_index;
			assert(f0 >= 0);
			assert(f1 >= 0);
			assert(f2 >= 0);

			v[0][k] = attrib.vertices[3 * f0 + k];
			v[1][k] = attrib.vertices[3 * f1 + k];
			v[2][k] = attrib.vertices[3 * f2 + k];
		}

		// Normals
		float n[3][3];
		{
			bool invalid_normal_index = false;
			if (attrib.normals.size() > 0) 
			{
				int nf0 = idx0.normal_index;
				int nf1 = idx1.normal_index;
				int nf2 = idx2.normal_index;

				if ((nf0 < 0) || (nf1 < 0) || (nf2 < 0)) 
				{
					// normal index is missing from this face.
					invalid_normal_index = true;
				}
				else 
				{
					for (int k = 0; k < 3; k++) 
					{
						assert(size_t(3 * nf0 + k) < attrib.normals.size());
						assert(size_t(3 * nf1 + k) < attrib.normals.size());
						assert(size_t(3 * nf2 + k) < attrib.normals.size());
						n[0][k] = attrib.normals[3 * nf0 + k];
						n[1][k] = attrib.normals[3 * nf1 + k];
						n[2][k] = attrib.normals[3 * nf2 + k];
					}
				}
			}
			else 
			{
				invalid_normal_index = true;
			}

			if (invalid_normal_index && !smoothVertexNormals.empty()) {
				// Use smoothing normals
				int f0 = idx0.vertex_index;
				int f1 = idx1.vertex_index;
				int f2 = idx2.vertex_index;

				if (f0 >= 0 && f1 >= 0 && f2 >= 0) {
					n[0][0] = smoothVertexNormals[f0].v[0];
					n[0][1] = smoothVertexNormals[f0].v[1];
					n[0][2] = smoothVertexNormals[f0].v[2];

					n[1][0] = smoothVertexNormals[f1].v[0];
					n[1][1] = smoothVertexNormals[f1].v[1];
					n[1][2] = smoothVertexNormals[f1].v[2];

					n[2][0] = smoothVertexNormals[f2].v[0];
					n[2][1] = smoothVertexNormals[f2].v[1];
					n[2][2] = smoothVertexNormals[f2].v[2];

					invalid_normal_index = false;
				}
			}

			if (invalid_normal_index) {
				// compute geometric normal
				CalcNormal(n[0], v[0], v[1], v[2]);
				n[1][0] = n[0][0];
				n[1][1] = n[0][1];
				n[1][2] = n[0][2];
				n[2][0] = n[0][0];
				n[2][1] = n[0][1];
				n[2][2] = n[0][2];
			}
		}

		for (int k = 0; k < 3; k++) 
		{
			IvTNPVertex vTNP;
			vTNP.position.Set(v[k][0],v[k][1],v[k][2]);
			vTNP.normal.Set(n[k][0],n[k][1],n[k][2]);
			vTNP.texturecoord.Set(tc[k][0],tc[k][1]);
			mDataVec.push_back(vTNP);
		}
	}

	// Transfer data to the OpenGL server (i.e. graphics card)
	mVertices = IvRenderer::mRenderer->GetResourceManager()
		->CreateVertexBuffer(kTNPFormat, mDataVec.size(), &mDataVec[0], kDefaultUsage);
	// Can't use indices on these since the vertex, normal and tex coordinates indices
	// are all typically different.  We would need 3 index arrays.
	//mIndices = IvRenderer::mRenderer->GetResourceManager()
	//	->CreateIndexBuffer((unsigned int)(mNumTriangles * 3), mIndexPtr, kDefaultUsage);
}

ModelFromObj::~ModelFromObj()
{
	delete mObj;
	delete[] mDataPtr;
	delete[] mIndexPtr;
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mVertices);
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mIndices);
}

void ModelFromObj::Draw()
{
	IvRenderer::mRenderer->Draw(kTriangleListPrim, mVertices);
}

bool ModelFromObj::hasSmoothingGroup(const tinyobj::shape_t& shape)
{
	for (size_t i = 0; i < shape.mesh.smoothing_group_ids.size(); i++) 
	{
		if (shape.mesh.smoothing_group_ids[i] > 0) 
		{
			return true;
		}
	}
	return false;
}

void ModelFromObj::computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape, std::map<int, vec3>& smoothVertexNormals)
{
	smoothVertexNormals.clear();
	std::map<int, vec3>::iterator iter;

	for (size_t f = 0; f < shape.mesh.indices.size() / 3; f++) {
		// Get the three indexes of the face (all faces are triangular)
		tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
		tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
		tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

		// Get the three vertex indexes and coordinates
		int vi[3];      // indexes
		float v[3][3];  // coordinates

		for (int k = 0; k < 3; k++) {
			vi[0] = idx0.vertex_index;
			vi[1] = idx1.vertex_index;
			vi[2] = idx2.vertex_index;
			assert(vi[0] >= 0);
			assert(vi[1] >= 0);
			assert(vi[2] >= 0);

			v[0][k] = attrib.vertices[3 * vi[0] + k];
			v[1][k] = attrib.vertices[3 * vi[1] + k];
			v[2][k] = attrib.vertices[3 * vi[2] + k];
		}

		// Compute the normal of the face
		float normal[3];
		CalcNormal(normal, v[0], v[1], v[2]);

		// Add the normal to the three vertexes
		for (size_t i = 0; i < 3; ++i) {
			iter = smoothVertexNormals.find(vi[i]);
			if (iter != smoothVertexNormals.end()) {
				// add
				iter->second.v[0] += normal[0];
				iter->second.v[1] += normal[1];
				iter->second.v[2] += normal[2];
			}
			else {
				smoothVertexNormals[vi[i]].v[0] = normal[0];
				smoothVertexNormals[vi[i]].v[1] = normal[1];
				smoothVertexNormals[vi[i]].v[2] = normal[2];
			}
		}

	}  // f

	// Normalize the normals, that is, make them unit vectors
	for (iter = smoothVertexNormals.begin(); iter != smoothVertexNormals.end();
		iter++) {
		normalizeVector(iter->second);
	}
}

void ModelFromObj::CalcNormal(float N[3], float v0[3], float v1[3], float v2[3])
{
	float v10[3];
	v10[0] = v1[0] - v0[0];
	v10[1] = v1[1] - v0[1];
	v10[2] = v1[2] - v0[2];

	float v20[3];
	v20[0] = v2[0] - v0[0];
	v20[1] = v2[1] - v0[1];
	v20[2] = v2[2] - v0[2];

	N[0] = v20[1] * v10[2] - v20[2] * v10[1];
	N[1] = v20[2] * v10[0] - v20[0] * v10[2];
	N[2] = v20[0] * v10[1] - v20[1] * v10[0];

	float len2 = N[0] * N[0] + N[1] * N[1] + N[2] * N[2];
	if (len2 > 0.0f) {
		float len = sqrtf(len2);

		N[0] /= len;
		N[1] /= len;
		N[2] /= len;
	}
}

void ModelFromObj::normalizeVector(vec3& v)
{
	float len2 = v.v[0] * v.v[0] + v.v[1] * v.v[1] + v.v[2] * v.v[2];
	if (len2 > 0.0f) {
		float len = sqrtf(len2);

		v.v[0] /= len;
		v.v[1] /= len;
		v.v[2] /= len;
	}
}
