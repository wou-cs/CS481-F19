//===============================================================================
// @ Player.cpp
// ------------------------------------------------------------------------------
// I, K - translate in x
// J, L - translate in y
// U, O - rotate around z axis
// P, : - uniformly scale
// T, G - rotate around y axis
// D, E - rotate around x axis
// F    - toggle solid fill / wireframe
//
//===============================================================================

//-------------------------------------------------------------------------------
//-- Dependencies ---------------------------------------------------------------
//-------------------------------------------------------------------------------

#include <IvRenderer.h>
#include <IvEventHandler.h>
#include <IvMath.h>
#include <IvVector3.h>
#include <IvVector4.h>
#include <IvRendererHelp.h>
#include <IvShaderProgram.h>
#include <IvUniform.h>
#include <IvResourceManager.h>
#include <iostream>

#include "Player.h"
#include "Game.h"
#include "ModelFromObj.h"

//-------------------------------------------------------------------------------
//-- Static Members -------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
//-- Methods --------------------------------------------------------------------
//-------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// @ Player::Player()
//-------------------------------------------------------------------------------
// Constructor
//-------------------------------------------------------------------------------
Player::Player()
	: mFill(true)
{
	mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
		IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile("PointPhong"),
		IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile("PointPhong"));

	if (mShader == NULL)
	{
		// shader failed to compile
		std::cout << "Shader failed to compile" << std::endl;
		return;
	}

	IvRenderer::mRenderer->SetShaderProgram(mShader);

	// Set up point light source
	mLightPosition.Set(2.0f, 2.0f, 10.0f);	// back up and to the right of the camera
	mLightRadiance.Set(1.0f, 1.0f, 1.0f);		// White light

	mLightPositionUniform = mShader->GetUniform("lightPosition");
	mLightRadianceUniform = mShader->GetUniform("lightRadiance");

	mLightPositionUniform->SetValue(mLightPosition,0);
	mLightRadianceUniform->SetValue(mLightRadiance, 0);

	// send texture
	IvUniform* texUniform = mShader->GetUniform("textureLU");
	if (texUniform != NULL)
	{
		//texUniform->SetValue(msurf.GetTexture());
	}

	mViewPosition.Set(-10.f, 2.0f, 10.0f);
	// sets standard lookat(0,0,0) from this position
	IvSetDefaultViewer(mViewPosition.x,mViewPosition.y,mViewPosition.z);

	mViewPositionUniform = mShader->GetUniform("viewPosition");
	mViewPositionUniform->SetValue(mViewPosition, 0);

	// Load a model from an .obj file.  Not all obj's will work.  If it crashes, try another one!
	mModel = new ModelFromObj("butterfly.obj");

}   // End of Player::Player()


//-------------------------------------------------------------------------------
// @ Player::~Player()
//-------------------------------------------------------------------------------
// Destructor
//-------------------------------------------------------------------------------
Player::~Player()
{
	IvRenderer::mRenderer->GetResourceManager()->Destroy(mShader);
}   // End of Player::~Player()


//-------------------------------------------------------------------------------
// @ Player::Update()
//-------------------------------------------------------------------------------
// Main update loop
//-------------------------------------------------------------------------------
void
Player::Update( float dt )
{
    // get change in transform for this frame
    IvMatrix44 scale, rotate, xlate, unxform, rexform, rotateX, rotateY;
    scale.Identity();
    rotate.Identity();
    float s = 1.0f;
    float r = 0.0f;
	float rx = 0.0f;
	float ry = 0.0f;
    float x = 0.0f, y = 0.0f, z = 0.0f; 

    // set up scaling
    if (IvGame::mGame->mEventHandler->IsKeyDown(';'))
    {
        s -= 0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('p'))
    {
        s += 0.25f*dt;
    }
    scale.Scaling( IvVector3(s, s, s) );
    
    // set up rotate
    if (IvGame::mGame->mEventHandler->IsKeyDown('o'))
    {
        r -= kPI*0.25f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('u'))
    {
        r += kPI*0.25f*dt;
    }
    rotate.RotationZ( r );

	// set up rotate about X
	if (IvGame::mGame->mEventHandler->IsKeyDown('d'))
	{
		rx -= kPI * 0.25f * dt;
	}
	if (IvGame::mGame->mEventHandler->IsKeyDown('e'))
	{
		rx += kPI * 0.25f * dt;
	}
	rotateX.RotationX(rx);

	// set up rotate about Y
	if (IvGame::mGame->mEventHandler->IsKeyDown('t'))
	{
		ry -= kPI * 0.25f * dt;
	}
	if (IvGame::mGame->mEventHandler->IsKeyDown('g'))
	{
		ry += kPI * 0.25f * dt;
	}
	rotateY.RotationY(ry);
    
    // set up translation
    if (IvGame::mGame->mEventHandler->IsKeyDown('k'))
    {
        x -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('i'))
    {
        x += 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('l'))
    {
        y -= 3.0f*dt;
    }
    if (IvGame::mGame->mEventHandler->IsKeyDown('j'))
    {
        y += 3.0f*dt;
    }
    IvVector3 xlatevector(x,y,z);
    xlate.Translation( xlatevector );
    
    // clear transform
    if (IvGame::mGame->mEventHandler->IsKeyPressed(' '))
    {
        mTransform.Identity();
    }

	IvVector3 originalXlate(mTransform(0, 3), mTransform(1, 3), mTransform(2, 3));
	unxform.Translation(-originalXlate);
	rexform.Translation(originalXlate);

	mTransform = xlate * rexform * rotateX * rotateY * rotate * unxform * mTransform * scale;
    
	// toggle the fill mode
	if (IvGame::mGame->mEventHandler->IsKeyPressed('f'))
	{
		mFill = !mFill;
	}

}   // End of Player::Update()


//-------------------------------------------------------------------------------
// @ Player::Render()
//-------------------------------------------------------------------------------
// Render stuff
//-------------------------------------------------------------------------------
void 
Player::Render()                                    
{   
    // store current matrix
    IvRenderer::mRenderer->SetWorldMatrix( mTransform );
	mShader->GetUniform("modelMatrix")->SetValue(mTransform, 0);

	if(mFill)
		IvRenderer::mRenderer->SetFillMode(IvFillMode::kSolidFill);
	else
		IvRenderer::mRenderer->SetFillMode(IvFillMode::kWireframeFill);
	
	//IvDrawTeapot();
	mModel->Draw();
}
