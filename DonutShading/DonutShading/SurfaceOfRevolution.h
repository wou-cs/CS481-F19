#pragma once

#include <IvVertexFormats.h>
#include <string>

// Forward declarations
class IvVertexBuffer;
class IvIndexBuffer;

class SurfaceOfRevolution
{
public:
	SurfaceOfRevolution(const char* filename, unsigned int nPoints);
	~SurfaceOfRevolution();

	void Draw();

private:

	// geometry
	IvCNPVertex*		mDataPtr;		// Points and colors
	UInt32*				mIndexPtr;		// Indices
	IvVertexBuffer*		mVertices;
	IvIndexBuffer*		mIndices;
	size_t				mNumVertices;
	size_t				mNumTriangles;
};

