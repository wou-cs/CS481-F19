
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

	IvUniform* mLightPositionUniform;
	IvUniform* mLightRadianceUniform;
	IvUniform* mViewPositionUniform;

	IvVector3 mLightPosition;		// position of point light source
	IvVector3 mLightRadiance;		// color of the light source
	IvVector3 mViewPosition;		// camera position in world coordinates
};

#endif
