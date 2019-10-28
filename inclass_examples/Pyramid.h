#pragma once
#include <IvVertexFormats.h>

// Forward declarations
class IvVertexBuffer;
class IvIndexBuffer;

class Pyramid
{
public:
	Pyramid();
	~Pyramid();

	void Draw();

private:
	void CalculateNormals();

	// geometry
	IvCPVertex * mDataPtr;		// Points and colors
	UInt32 * mIndexPtr;			// Indices
	IvVertexBuffer * mVertices;
	IvIndexBuffer * mIndices;
	IvVertexBuffer* mVerticesNormals;
};

