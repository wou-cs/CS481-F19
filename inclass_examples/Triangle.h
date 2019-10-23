#pragma once

#include <IvVertexFormats.h>

// Forward Declaration
class IvVertexBuffer;
class IvIndexBuffer;

class Triangle
{
public:
	Triangle();
	~Triangle();

	void Draw();

private:
	IvCPVertex* mDataPtr;
	UInt32* mIndexPtr;
	IvVertexBuffer* mVertices;
	IvIndexBuffer* mIndices;
};

