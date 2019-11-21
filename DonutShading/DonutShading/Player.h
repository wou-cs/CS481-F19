
#ifndef __PlayerDefs__
#define __PlayerDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvMatrix44.h>
#include "SurfaceOfRevolution.h"

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvShaderProgram;
class IvUniform;

class Player
{
public:
	Player();
    ~Player();

    void Update( float dt );
    void Render();

private:
    IvMatrix44  mTransform;
	bool mFill;
	SurfaceOfRevolution msurf;
	IvShaderProgram* mShader;

	// Constant model color
	IvVector4 mModelColor;
	IvUniform* mColor;
	IvUniform* mLightDirUniform;
	IvUniform* mLightRadianceUniform;

	// Directional light source
	IvVector3 mLightDir;
	IvVector3 mLightRadiance;		// color of the light source
};

#endif
