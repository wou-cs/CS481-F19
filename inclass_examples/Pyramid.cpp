#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>
#include <IvResourceManager.h>
#include <IvRenderer.h>
#include <IvVector3.h>
#include <IvColor.h>
#include <IvStackAllocator.h>

#include "Pyramid.h"

#define NUM_INDICES (6*3)


Pyramid::Pyramid()
	: mDataPtr(0),mIndexPtr(0),mVertices(0),mIndices(0),mVerticesNormals(0)
{
	// Pyramid building code removed


	CalculateNormals();
}

Pyramid::~Pyramid()
{
	// other cleanup code removed

	IvRenderer::mRenderer->GetResourceManager()->Destroy(mVerticesNormals);
}

void Pyramid::Draw()
{
	// Other drawing code removed

	
	// Draw normal vector "hair" as lines
	IvRenderer::mRenderer->Draw(kLineListPrim, mVerticesNormals);
}

void Pyramid::CalculateNormals()
{
	// Use preallocated memory from this "stack"
	size_t currentOffset = IvStackAllocator::mScratchAllocator->GetCurrentOffset();
	IvCPVertex* dataPtr = (IvCPVertex*)IvStackAllocator::mScratchAllocator->Allocate(kIvVFSize[kCPFormat] * 2 * NUM_INDICES / 3);

	if (nullptr == dataPtr)
	{
		return;
	}

	// Iterate over each triangle
	for (int i = 0; i < NUM_INDICES / 3; ++i)
	{
		// Grab the 3 points of this triangle
		IvVector3 p0 = mDataPtr[mIndexPtr[3 * i + 0]].position;
		IvVector3 p1 = mDataPtr[mIndexPtr[3 * i + 1]].position;
		IvVector3 p2 = mDataPtr[mIndexPtr[3 * i + 2]].position;
		// Calculate starting point of the line, at the centroid
		IvVector3 pc = (p0 + p1 + p2) / 3.0f;
		// Need the normal vector to calculate the end-point
		IvVector3 vn = (p1 - p0).Cross(p2 - p0);
		vn.Normalize();
		IvVector3 pend = pc + 1 * vn;

		// Put this line segment into our data array
		dataPtr[2 * i + 0].position = pc;
		dataPtr[2 * i + 0].color.Set(0, 0, 0, 255);

		dataPtr[2 * i + 1].position = pend;
		dataPtr[2 * i + 1].color.Set(0, 0, 0, 255);
	}

	// Send it to the GPU in a Vertex Buffer Object
	mVerticesNormals = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, 2 * NUM_INDICES / 3, dataPtr, kImmutableUsage);

	// pop allocation off the stack
	IvStackAllocator::mScratchAllocator->Reset(currentOffset);
}
