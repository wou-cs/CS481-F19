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
	: mFill(true), msurf("donut.txt",36), mModelColor(1.0,0.0,0.0,0.0)
{
	mShader = IvRenderer::mRenderer->GetResourceManager()->CreateShaderProgram(
		IvRenderer::mRenderer->GetResourceManager()->CreateVertexShaderFromFile("DirDiffuse"),
		IvRenderer::mRenderer->GetResourceManager()->CreateFragmentShaderFromFile("DirDiffuse"));

	if (mShader == NULL)
	{
		// shader failed to compile
		std::cout << "Shader failed to compile" << std::endl;
		return;
	}

	IvRenderer::mRenderer->SetShaderProgram(mShader);

	//mColor = mShader->GetUniform("constColor");
	//mColor->SetValue(mModelColor,0);

	// Set up light source
	mLightDir.Set(0.5f, 0.75f, -1.0f);			// moved light source to point more down y so we can see more of the dark side
	mLightRadiance.Set(0.6f, 0.8f, 1.0f);		// Slightly green-blue

	mLightDirUniform = mShader->GetUniform("lightDir");
	mLightRadianceUniform = mShader->GetUniform("lightRadiance");

	mLightDirUniform->SetValue(mLightDir,0);
	mLightRadianceUniform->SetValue(mLightRadiance, 0);

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

	// Change constant uniform color via keys
	if (IvGame::mGame->mEventHandler->IsKeyDown(','))
	{
		mModelColor.x -= 0.25f * dt;
	}
	if (IvGame::mGame->mEventHandler->IsKeyDown('.'))
	{
		mModelColor.x += 0.25f * dt;
	}
	//mColor->SetValue(mModelColor, 0);

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

	if(mFill)
		IvRenderer::mRenderer->SetFillMode(IvFillMode::kSolidFill);
	else
		IvRenderer::mRenderer->SetFillMode(IvFillMode::kWireframeFill);
	
	msurf.Draw();
}
