
#ifndef __PlayerDefs__
#define __PlayerDefs__

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvMatrix44.h>

//-------------------------------------------------------------------------------
//-- Classes --------------------------------------------------------------------
//-------------------------------------------------------------------------------

class IvShaderProgram;
class IvUniform;
class ModelFromObj;

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
	IvShaderProgram* mShader;

	IvUniform* mLightPositionUniform;
	IvUniform* mLightRadianceUniform;
	IvUniform* mViewPositionUniform;

	IvVector3 mLightPosition;		// position of point light source
	IvVector3 mLightRadiance;		// color of the light source
	IvVector3 mViewPosition;		// camera position in world coordinates

	// This will hold a model that we load from an .obj file
	ModelFromObj* mModel;
};

#endif
