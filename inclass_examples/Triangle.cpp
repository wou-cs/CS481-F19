#include "Triangle.h"

#include <IvVertexBuffer.h>
#include <IvIndexBuffer.h>
#include <IvResourceManager.h>
#include <IvRenderer.h>
#include <IvVector3.h>
#include <IvColor.h>

#define NUM_VERTS 3
#define NUM_INDICES (1*3)

Triangle::Triangle()
	: mDataPtr(0), mIndexPtr(0),mVertices(0),mIndices(0)
{
	mDataPtr = new IvCPVertex[NUM_VERTS];
	mDataPtr[0].position.Set(0.0f,  1.0f, 0.0f);
	mDataPtr[1].position.Set(0.0f, -1.0f, 0.0f);
	mDataPtr[2].position.Set(0.0f,  0.0f, 2.0f);
	mDataPtr[0].color.Set(0, 0, 255, 255);
	mDataPtr[1].color.Set(180, 200, 20, 255);
	mDataPtr[2].color.Set(50, 200, 50, 255);

	mIndexPtr = new UInt32[NUM_INDICES]{
		0,1,2
	};

	mVertices = IvRenderer::mRenderer->GetResourceManager()->CreateVertexBuffer(kCPFormat, NUM_VERTS, mDataPtr, kDefaultUsage);
	mIndices = IvRenderer::mRenderer->GetResourceManager()->CreateIndexBuffer(NUM_INDICES, mIndexPtr, kDefaultUsage);
}

Triangle::~Triangle()
{
	delete[] mDataPtr;
	delete[] mIndexPtr;
}

void Triangle::Draw()
{
	IvRenderer::mRenderer->Draw(kTriangleListPrim, mVertices, mIndices);
}
