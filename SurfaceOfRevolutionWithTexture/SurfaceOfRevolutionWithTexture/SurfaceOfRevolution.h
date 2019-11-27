#pragma once

#include <IvVertexFormats.h>
#include <string>

// Forward declarations
class IvVertexBuffer;
class IvIndexBuffer;
class IvTexture;

class SurfaceOfRevolution
{
public:
	SurfaceOfRevolution(const char* filename, unsigned int nPoints, const char* textureFilename);
	~SurfaceOfRevolution();

	void BuildSurface(const char* filename, unsigned int nPoints);

	IvTexture* GetTexture();

	void Draw();

private:

	// geometry
	IvTNPVertex*		mDataPtr;		// Points, normals and texture coordinates
	UInt32*				mIndexPtr;		// Indices
	IvVertexBuffer*		mVertices;
	IvIndexBuffer*		mIndices;
	size_t				mNumVertices;
	size_t				mNumTriangles;

	IvTexture*			mTexture;		// This version uses a texture and no per-vertex colors
	// This choice to embed the texture in the object isn't the best choice if the texture is to be re-used
	// Would be better to have a single class/object to manage all textures used, load them and make them
	// available to set in a uniform.  That way we can share textures across images and not duplicate allocated
	// resources.
	// We'll embed it here for convenience in this example
};

