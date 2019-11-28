#pragma once

#include <IvVertexFormats.h>
#include <string>
#include "tiny_obj_loader.h"

// Forward declarations
class IvVertexBuffer;
class IvIndexBuffer;
//namespace tinyobj { class ObjReader; }


class ModelFromObj
{
	struct vec3
	{
		float v[3];
		vec3()
		{
			v[0] = 0.0f;
			v[1] = 0.0f;
			v[2] = 0.0f;
		}
	};

public:

	ModelFromObj(const std::string & filename);
	~ModelFromObj();

	void Draw();

private:

	static bool hasSmoothingGroup(const tinyobj::shape_t& shape);
	static void computeSmoothingNormals(const tinyobj::attrib_t& attrib, const tinyobj::shape_t& shape,std::map<int, vec3>& smoothVertexNormals);
	static void CalcNormal(float N[3], float v0[3], float v1[3], float v2[3]);
	static void normalizeVector(vec3& v);

	tinyobj::ObjReader*	mObj;

	// geometry
	IvTNPVertex*		mDataPtr;
	std::vector<IvTNPVertex> mDataVec;
	UInt32*				mIndexPtr;
	IvVertexBuffer*		mVertices;
	IvIndexBuffer*		mIndices;
	size_t				mNumVertices;
	size_t				mNumNormals;
	size_t				mNumTexcoords;
	size_t				mNumTriangles;
};

