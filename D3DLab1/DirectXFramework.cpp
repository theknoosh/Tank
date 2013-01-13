//////////////////////////////////////////////////////////////////////////
// Name:	DirectXFramework.cpp
//////////////////////////////////////////////////////////////////////////
#define DIMOUSE_LEFTBUTTON 0
#define DIMOUSE_RIGHTBUTTON 1

#include "DirectXFramework.h"
#include <iostream>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <DxErr.h>

CDirectXFramework::CDirectXFramework(void)
{
	// Init or NULL objects before use to avoid any undefined behavior
	m_bVsync		= false;
	m_pD3DObject	= 0;
	m_pD3DDevice	= 0;
	
}

CDirectXFramework::~CDirectXFramework(void)
{
	// If Shutdown is not explicitly called correctly, call it when 
	// this class is destroyed or falls out of scope as an error check.
	Shutdown();
}

void CDirectXFramework::Init(HWND& hWnd, HINSTANCE& hInst, bool bWindowed)
{
	m_hWnd = hWnd;
	HRESULT hr;
	TCHAR *ErrorMessage;


	//////////////////////////////////////////////////////////////////////////
	// Direct3D Foundations - D3D Object, Present Parameters, and D3D Device
	//////////////////////////////////////////////////////////////////////////

	// Create the D3D Object
	m_pD3DObject = Direct3DCreate9(D3D_SDK_VERSION);

	// Find the width and height of window using hWnd and GetWindowRect()
	RECT rect;
	GetWindowRect(hWnd, &rect);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	// Set D3D Device presentation parameters before creating the device
	D3DPRESENT_PARAMETERS D3Dpp;
	ZeroMemory(&D3Dpp, sizeof(D3Dpp));  // NULL the structure's memory

	D3Dpp.hDeviceWindow					= hWnd;										// Handle to the focus window
	D3Dpp.Windowed						= bWindowed;								// Windowed or Full-screen boolean
	D3Dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;								// Format of depth/stencil buffer, 24 bit depth, 8 bit stencil
	D3Dpp.EnableAutoDepthStencil		= TRUE;										// Enables Z-Buffer (Depth Buffer)
	D3Dpp.BackBufferCount				= 1;										// Change if need of > 1 is required at a later date
	D3Dpp.BackBufferFormat				= D3DFMT_X8R8G8B8;							// Back-buffer format, 8 bits for each pixel
	D3Dpp.BackBufferHeight				= height;									// Make sure resolution is supported, use adapter modes
	D3Dpp.BackBufferWidth				= width;									// (Same as above)
	D3Dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;					// Discard back-buffer, must stay discard to support multi-sample
	D3Dpp.PresentationInterval			= m_bVsync ? D3DPRESENT_INTERVAL_DEFAULT : D3DPRESENT_INTERVAL_IMMEDIATE; // Present back-buffer immediately, unless V-Sync is on								
	D3Dpp.Flags							= D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;		// This flag should improve performance, if not set to NULL.
	D3Dpp.FullScreen_RefreshRateInHz	= bWindowed ? 0 : D3DPRESENT_RATE_DEFAULT;	// Full-screen refresh rate, use adapter modes or default
	D3Dpp.MultiSampleQuality			= 0;										// MSAA currently off, check documentation for support.
	D3Dpp.MultiSampleType				= D3DMULTISAMPLE_NONE;						// MSAA currently off, check documentation for support.

	// Check device capabilities
	DWORD deviceBehaviorFlags = 0;
	m_pD3DObject->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &m_D3DCaps);

	// Determine vertex processing mode
	if(m_D3DCaps.DevCaps & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		// Hardware vertex processing supported? (Video Card)
		deviceBehaviorFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	}
	else
	{
		// If not, use software (CPU)
		deviceBehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING; 
	}
	
	// If hardware vertex processing is on, check pure device support
	if(m_D3DCaps.DevCaps & D3DDEVCAPS_PUREDEVICE && deviceBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING)
	{
		deviceBehaviorFlags |= D3DCREATE_PUREDEVICE;	
	}
	
	// Create the D3D Device with the present parameters and device flags above
	m_pD3DObject->CreateDevice(
		D3DADAPTER_DEFAULT,		// which adapter to use, set to primary
		D3DDEVTYPE_HAL,			// device type to use, set to hardware rasterization
		hWnd,					// handle to the focus window
		deviceBehaviorFlags,	// behavior flags
		&D3Dpp,					// presentation parameters
		&m_pD3DDevice);			// returned device pointer

	//*************************************************************************

	//////////////////////////////////////////////////////////////////////////
	// Create a Font Object
	//////////////////////////////////////////////////////////////////////////
	
	// Load a font for private use for this process
	// Load external font
	AddFontResourceEx(L"Delicious-Roman.otf",FR_PRIVATE,0);

	// Load D3DXFont, each font style you want to support will need an ID3DXFont
	D3DXFONT_DESC fontFace;
	fontFace.Height			= 24;
	fontFace.Width			= 0;
	fontFace.Weight			= 0;
	fontFace.MipLevels		= 1;
	fontFace.Italic			= false;
	fontFace.CharSet		= DEFAULT_CHARSET;
	fontFace.OutputPrecision = OUT_DEFAULT_PRECIS;
	fontFace.Quality		= DEFAULT_QUALITY;
	fontFace.PitchAndFamily	= DEFAULT_PITCH | FF_DONTCARE;
	_tcscpy(fontFace.FaceName, _T("Delicious-Roman"));

	D3DXCreateFontIndirect(m_pD3DDevice, &fontFace, &m_pD3DFont);


	//////////////////////////////////////////////////////////////////////////
	// Create Sprite Object and Textures
	//////////////////////////////////////////////////////////////////////////

	// Create a sprite object, note you will only need one for all 2D sprites
	D3DXCreateSprite(m_pD3DDevice, &m_pD3DSprite);

	// Load texture for sprite
	D3DXCreateTextureFromFile(m_pD3DDevice, L"BattleTankAnimation.png", &m_pTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"BattleTankAnimationIdle.png",&m_pTankIdleTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"BackgroundLevel1.png", &m_pBackgroundTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"MainMenu.png", &m_pMainMenuTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"Animated-Explosion.png", &m_pExplosionTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"Bomb.png", &m_pBombTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"smokeAnimation01.png",&m_pSmokeTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"GameOver.png",&m_pGameOverTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"HUD.png",&m_pHUDTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"GermanNest.png", &m_pMachineGunNestTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"HealthCrate.png", &m_pHealthTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"Instructions.png", &m_pInstructionsTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"GermanNest.png", &m_pMachineGunTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"GermanNestDestroyed.png", &m_pMachineGunNestDestroyedTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"machineGunFire.png",&m_pMachineGunFlashTexture);
	hr = D3DXCreateTextureFromFile(m_pD3DDevice, L"tankBlastAnim.png",&m_pTankBlastTexture);
	D3DXCreateTextureFromFile(m_pD3DDevice, L"AmmoCrate.png",&m_pAmmoTexture);

	if(hr != D3D_OK) {
		ErrorMessage = (TCHAR *)DXGetErrorString(hr);
	}

	// Graphics for Main Menu
	mMenuTexture[0] = L"MenuItem01.png";
	mMenuTexture[1] = L"MenuItem02.png";
	mMenuTexture[2] = L"MenuItem03.png";
	mMenuTexture[3] = L"MenuItem04.png";

	mMenuTextureSel[0] = L"MenuItemSel01.png";
	mMenuTextureSel[1] = L"MenuItemSel02.png";
	mMenuTextureSel[2] = L"MenuItemSel03.png";
	mMenuTextureSel[3] = L"MenuItemSel04.png";

	// Textures for menu item
	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTexture[0], &m_pMenuTexture[0]);
	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTexture[1], &m_pMenuTexture[1]);
	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTexture[2], &m_pMenuTexture[2]);
	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTexture[3], &m_pMenuTexture[3]);

	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTextureSel[0], &m_pMenuTextureSel[0]);
	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTextureSel[1], &m_pMenuTextureSel[1]);
	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTextureSel[2], &m_pMenuTextureSel[2]);
	D3DXCreateTextureFromFile(m_pD3DDevice, mMenuTextureSel[3], &m_pMenuTextureSel[3]);

	/******************************FMOD Sound System Initialization
	Create a System object and initialize.
	*/

	result = FMOD::System_Create(&system);
	ERRCHECK(result);

	result = system->getVersion(&version);
	ERRCHECK(result);

	if (version < FMOD_VERSION)
	{
		printf("Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", version, FMOD_VERSION);
		//return 0;
	}

	result = system->getNumDrivers(&numdrivers);
	ERRCHECK(result);

	if (numdrivers == 0)
	{
		result = system->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
		ERRCHECK(result);
	}
	else
	{

		result = system->getDriverCaps(0, &caps, 0, &speakermode);
		ERRCHECK(result);

		result = system->setSpeakerMode(speakermode);       /* Set the user selected speaker mode. */
		ERRCHECK(result);

		if (caps & FMOD_CAPS_HARDWARE_EMULATED)             /* The user has the 'Acceleration' slider set to off!  This is really bad for latency!. */
		{                                                   /* You might want to warn the user about this. */
			result = system->setDSPBufferSize(1024, 10);
			ERRCHECK(result);
		}

		result = system->getDriverInfo(0, name, 256, 0);
		ERRCHECK(result);

		if (strstr(name, "SigmaTel"))   /* Sigmatel sound devices crackle for some reason if the format is PCM 16bit.  PCM floating point output seems to solve it. */
		{
			result = system->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0, FMOD_DSP_RESAMPLER_LINEAR);
			ERRCHECK(result);
		}
	}

	// This initialization uses 100 'virtual voices'
	result = system->init(100, FMOD_INIT_NORMAL, 0);
	if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)         /* Ok, the speaker mode selected isn't supported by this soundcard.  Switch it back to stereo... */
	{
		result = system->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
		ERRCHECK(result);

		result = system->init(100, FMOD_INIT_NORMAL, 0);/* ... and re-init. */
		ERRCHECK(result);
	}

	// Load sound files
	//Loops
	result = system->createSound("Military.mp3", FMOD_LOOP_NORMAL, 0, &soundLoop); // Sound file to play
	ERRCHECK(result);
	result = system->createSound("WarAmbience.wav",FMOD_LOOP_NORMAL, 0, &warLoop);
	ERRCHECK(result);
	result = system->createSound("MenuOpeningMusic.mp3",FMOD_LOOP_NORMAL, 0, &menuOpen);
	ERRCHECK(result);
	result = system->createSound("Tank_idleLoop_med.wav",FMOD_LOOP_NORMAL, 0, &tankIdle);
	ERRCHECK(result);
	result = system->createSound("TankMovingLoop.mp3",FMOD_LOOP_NORMAL, 0, &tankMoving);
	ERRCHECK(result);
	result = system->createSound("drumLoop.mp3",FMOD_LOOP_NORMAL, 0, &drumLoop);
	ERRCHECK(result);

	// Non Loops
	result = system->createSound("MilitaryGameOverClip.mp3", FMOD_DEFAULT, 0, &gameOver); // gameOver Music
	ERRCHECK(result);
	result = system->createSound("BombDrop.mp3", FMOD_DEFAULT, 0, &bombDrop); // gameOver Music
	ERRCHECK(result);
	result = system->createSound("SmallExplosion.mp3", FMOD_DEFAULT, 0, &smallExplosion); // gameOver Music
	ERRCHECK(result);
	result = system->createSound("machineGunFire.mp3",FMOD_DEFAULT, 0, &machineGunFire);
	ERRCHECK(result);
	result = system->createSound("tankFire.mp3",FMOD_DEFAULT, 0, &tankBlast);
	ERRCHECK(result);
	result = system->createSound("Woop Woop.mp3",FMOD_DEFAULT, 0, &battleAlarm);
	ERRCHECK(result);
	result = system->createSound("can_crush.wav",FMOD_DEFAULT, 0, &healthConnect);
	ERRCHECK(result);
	result = system->createSound("bull_1.mp3",FMOD_DEFAULT, 0, &planeBullet);
	ERRCHECK(result);
	result = system->createSound("ricochet_metal_03.wav",FMOD_DEFAULT,0, &ricochet);
	ERRCHECK(result);



	scoreTable = new ScoreBoard();
	EnemyPlane = new BiPlane();
	EnemyPlane->Init(m_pD3DDevice,m_pD3DSprite);
	// EnemyPlane->Spawn(); // Plane invisible at start
	timeToNextPlane = rand() % 30; // initial time to next plane


	// Initialize random functions
	srand (time(NULL));

	// Settings for the Tank
	tankInfo.pos.x = 20;
	tankInfo.pos.y = 450;
	tankInfo.rotation =0;
	tankInfo.scale.x = 1;
	tankInfo.scale.y = 1;
	tankInfo.scale.z = 0;
	tankInfo.width = 186;
	tankInfo.height = 64;
	tankInfo.curframe = 0;
	tankInfo.lastframe = 7;
	tankInfo.animdelay = 2;
	tankInfo.animcount = 0;
	tankInfo.movex = 6;
	tankInfo.movey = 0;
	tankInfo.center.x = 93;
	tankInfo.center.y = 32;

	tankIdleInfo.pos.x = 20;
	tankIdleInfo.pos.y = 450;
	tankIdleInfo.rotation = 0;
	tankIdleInfo.scale.x = 1;
	tankIdleInfo.scale.y = 1;
	tankIdleInfo.width = 186;
	tankIdleInfo.height = 64;
	tankIdleInfo.curframe = 0;
	tankIdleInfo.lastframe = 7;
	tankIdleInfo.animdelay = 2;
	tankIdleInfo.movex = 0;
	tankIdleInfo.movey = 0;
	tankIdleInfo.center.x = 93;
	tankIdleInfo.center.y = 32;

	machineGunFlashInfo.pos.x = 655;
	machineGunFlashInfo.pos.y = 445;
	machineGunFlashInfo.width = 27;
	machineGunFlashInfo.height = 22;
	machineGunFlashInfo.curframe = 0;
	machineGunFlashInfo.lastframe = 1;
	machineGunFlashInfo.animdelay = 1;
	flashDelay = 0.1f;

	tankBlastInfo.width = 60;
	tankBlastInfo.height = 41;
	tankBlastInfo.curframe = 0;
	tankBlastInfo.lastframe = 2;
	tankBlastInfo.animdelay = 5;
	fireDelay = 20;


	// Machine Gun Nest
	machineGunInfo.pos.x = 650;
	machineGunInfo.pos.y = 450;
	machineGunInfo.rotation = 0;
	machineGunInfo.scale.x = 1;
	machineGunInfo.scale.y = 1;
	machineGunInfo.scale.z = 1;
	machineGunInfo.width = 128;
	machineGunInfo.height = 64;
	machineGunInfo.center.x = 64;
	machineGunInfo.center.y = 32;

	machineGunDestroyedInfo.pos.x = 650;
	machineGunDestroyedInfo.pos.y = 450;
	machineGunDestroyedInfo.rotation = 0;
	machineGunDestroyedInfo.scale.x = 1;
	machineGunDestroyedInfo.scale.y = 1;
	machineGunDestroyedInfo.scale.z = 1;
	machineGunDestroyedInfo.width = 128;
	machineGunDestroyedInfo.height = 64;
	machineGunDestroyedInfo.center.x = 64;
	machineGunDestroyedInfo.center.y = 32;

	// Health Info
	healthInfo.width = 32;
	healthInfo.height = 32;
	healthInfo.pos.y = 475;
	healthInfo.pos.x = 750;

	ammoInfo.width = 32;
	ammoInfo.height = 32;
	ammoInfo.pos.x = 750;
	ammoInfo.pos.y = 475;

	tankAmmo = 10; // Amount of rounds in tank

	// Settings for explosion
	explosionInfo.pos.x = SCREEN_WIDTH/2;
	explosionInfo.pos.y = tankInfo.pos.y;
	explosionInfo.pos.z = 0;
	explosionInfo.width = 46;
	explosionInfo.height = 64;
	explosionInfo.curframe = 0;
	explosionInfo.lastframe = 8;
	explosionInfo.animdelay = 2;
	explosionInfo.animcount = 0;
	explosionInfo.center.x = 23;
	explosionInfo.center.y = 32;

	// Settings for smoke
	smokeInfo.pos.x = 0;
	smokeInfo.pos.y = 0;
	smokeInfo.pos.z = 0;
	smokeInfo.width = 128;
	smokeInfo.height = 128;
	smokeInfo.curframe = 0;
	smokeInfo.lastframe = 7;
	smokeInfo.animdelay = 2;
	smokeInfo.animcount = 0;
	smokeInfo.center.x = 64;
	smokeInfo.center.y = 128;

	//Settings for game over banner
	gameOverInfo.center.x = 256;
	gameOverInfo.center.y = 64;
	gameOverInfo.pos.x = (SCREEN_WIDTH/2) - gameOverInfo.center.x;
	gameOverInfo.pos.y = (SCREEN_HEIGHT/2) - gameOverInfo.center.y;
	gameOverInfo.pos.z = 0;
	gameOverInfo.width = 512;
	gameOverInfo.height = 128;
	

	// Settings for bomb
	bombInfo.pos.x = SCREEN_WIDTH/2;
	bombInfo.pos.y = -20;
	bombInfo.pos.z = 0;
	bombInfo.width = 64;
	bombInfo.height = 64;
	bombInfo.movex = 0;
	bombInfo.movey = 16;

	// Settings for the background
	backgroundInfo.pos.x = 0;
	backgroundInfo.pos.y = 0;
	backgroundInfo.pos.z = 0;
	backgroundInfo.scale.x = 1;
	backgroundInfo.scale.y = 1;
	backgroundInfo.scale.z = 1;
	backgroundInfo.width = 2940;
	backgroundInfo.height = 1024;
	backgroundInfo.movex = 6;
	backgroundInfo.movey = 0;

	// Settings for the Main Menu
	mainMenuInfo.pos.x = 0;
	mainMenuInfo.pos.y = 0;
	mainMenuInfo.pos.z = 0;
	mainMenuInfo.scale.x = 1;
	mainMenuInfo.scale.y = 1;
	mainMenuInfo.scale.z = 1;
	mainMenuInfo.width = 800;
	mainMenuInfo.height = 600;

	// Settings for Instructions
	instructionsInfo.pos.x = 0;
	instructionsInfo.pos.y = 0;
	instructionsInfo.pos.z = 0;

	// Default startup values **********************************************

	// Settings for HUD
	HUDInfo.pos.x = 0;
	HUDInfo.pos.y = 530;
	HUDInfo.pos.z = 0;

	// Default game mode
	gameMode = 'm';

	// Start sound at main menu
	menuPlayOnce = true;
	playTankMovingOnce = true;

	// Tank initially idling
	tankIsIdling = true;

	// initial bomb x position
	bombXPosition = rand() % 800;
	// initial time to next bomb
	timeToNextBomb = .01f * (rand() % 100 + 1); //time between .01 and 1 second

	timeToNextHealth = rand()% 30; // time between 2 seconds to 30 seconds
	timeToNextAmmo = rand() & 30; 

	// Initial time to next ammo
	timeToNextAmmo = rand() % 30; 

	float maxPos = (backgroundInfo.width - (2*SCREEN_WIDTH)-tankInfo.width);

	// Initial trigger location for machine gun nest
	machineGunTriggerPos = rand() % (backgroundInfo.width - (2*SCREEN_WIDTH)-tankInfo.width);
	machineGunTriggerPos += SCREEN_WIDTH;
	machineGunTriggerPos = -machineGunTriggerPos;

	// Test max position
	// machineGunTriggerPos = -(maxPos + SCREEN_WIDTH);

	// Bomb and tank defaults
	hasNotExploded = true;
	dropBomb = false;
	tankDamage = 0;

	// Background loop count
	loopCount = 1;
	// Which loop triggers machine Gun Nest
	loopTrigger = 1;

	healthExists = false;
	ammoExists = false;


	// End Default startup values *************************************************

	mSpriteCenter = D3DXVECTOR3(48.0f, 25.0f, 0);

	// Setup for keyboard and mouse input
	// Clear memory for the following two variables
	ZeroMemory(mKeyboardState, sizeof(mKeyboardState));
	ZeroMemory(&mMouseState, sizeof(mMouseState));

	DirectInput8Create(hInst , DIRECTINPUT_VERSION,
		IID_IDirectInput8, (void**)&mDInput, 0);
	
	// Set up the keyboard
	mDInput->CreateDevice(GUID_SysKeyboard, &mKeyboard, 0);
	mKeyboard->SetDataFormat(&c_dfDIKeyboard);
	// mKeyboard->SetCooperativeLevel(g_hInstance->getMainWnd , keyboardCoopFlags);

	// Set up the mouse
	mDInput->CreateDevice(GUID_SysMouse, &mMouse, 0);
	mMouse->SetDataFormat(&c_dfDIMouse2);
	// mMouse->SetCooperativeLevel(g_hInstance->getMainWnd, mouseCoopFlags);

}

void CDirectXFramework::Update(float dt)
{
	
	// ************* Display frame count on screen ********************
	// Make static so their values persist across function calls
	static float numFrames = 0.0f;
	static float timeElapsed = 0.0f;
	static float moveTimeElapsed = 0.0f;
	static float bombTimeElapsed = 0.0f;
	static float healthTimeElapsed = 0.0f;
	static float ammoTimeElapsed = 0.0f;
	static float planeTimeElapsed = 0.0f;

	static float saveBackgroundPos;
	static float saveTankPos;
	static float fireTimeElapsed = 0;
	static int   tankFireDelayCount = 0;
	static bool	 tankBlastForward;
	static int	 machineGunNestDelay;
	static int	 offset;

	float		triggerRange;
	float		distanceToTank;

	// Increment the frame count
	numFrames += 1.0f;

	// Accumlate time passed
	timeElapsed += dt;
	moveTimeElapsed += dt;
	bombTimeElapsed += dt;
	if (!healthExists)healthTimeElapsed += dt; // count down to next health only if not exists already
	if (!ammoExists)ammoTimeElapsed += dt; // count down to next ammo only if not exists already

	gameTime += dt;

	if(!EnemyPlane->IsVisible())planeTimeElapsed += dt; // count down to next plane only when not visible

	//Compute frame stats once per second
	if(timeElapsed >= 1.0f)
	{
		mFPS = numFrames;

		timeElapsed = 0.0f;
		numFrames = 0.0f;
	}
	// ************* End of frame count on screen *********************

	// Check for keyboard activity
	mKeyboard->Acquire();

	HRESULT hr = mKeyboard->GetDeviceState(sizeof(mKeyboardState),
		(void**)&mKeyboardState);
	if( FAILED(hr))
	{
		// keyboard lost, zero out structure
		ZeroMemory(mKeyboardState, sizeof(mKeyboardState));

		// Try to reaquire for next time
		hr = mKeyboard->Acquire();
	}

	if ((mKeyboardState[DIK_M] & 0x80)!=0) gameMode = 'm'; // MainMenu mode
	if ((mKeyboardState[DIK_Q] & 0x80)!=0) gameMode = 'q'; // Quit game


	// Check for mouse activity
	mMouse->Acquire();

	hr = mMouse->GetDeviceState(sizeof(DIMOUSESTATE2), (void**)&mMouseState);
	if( FAILED(hr))
	{
		// Mouse lost, zero out structure
		ZeroMemory(&mMouseState, sizeof(mMouseState));

		// Try to reaquire for next time
		hr = mMouse->Acquire();
	}
if(moveTimeElapsed > 0.04f) // Set game to run 30 frames per second
{
	moveTimeElapsed = 0.0f;
	switch (gameMode)
	{
	case 'p': // game play mode

		newScore = true;
				
		if(healthTimeElapsed > timeToNextHealth && !healthExists) // calculate next health if not exists
		{
			healthTimeElapsed = 0.0f; // reset counter
			healthExists = true; // enable health icon
			healthXPos = 800; // position health icon off screen to right
		}

		if(ammoTimeElapsed > timeToNextAmmo && !ammoExists)
		{
			ammoTimeElapsed = 0.0f;
			ammoExists = true;
			ammoXPos = 825;
		}
		
		if ((mKeyboardState[DIK_RIGHT] & 0x80)!=0)
			{
				if(tankInfo.pos.x < SCREEN_WIDTH - tankInfo.width - 100)
				{
					tankInfo.pos.x += tankInfo.movex;

				}else
				{
					backgroundInfo.pos.x -= backgroundInfo.movex;
					if(healthExists)healthXPos -= backgroundInfo.movex;
					if(ammoExists)ammoXPos -= backgroundInfo.movex;

					if (backgroundInfo.pos.x < -(backgroundInfo.width) + SCREEN_WIDTH)
					{
						float testNumber = -(backgroundInfo.width) + SCREEN_WIDTH;
						backgroundInfo.pos.x = 0;
						++loopCount;
					}
						
						

				}
				//Animate wheels
				if(++tankInfo.animcount > tankInfo.animdelay)
				{
					//reset counter
					tankInfo.animcount = 0;
					//Animate the sprite
					if(++tankInfo.curframe > tankInfo.lastframe)
						tankInfo.curframe = 0;
				}
			}

			if ((mKeyboardState[DIK_LEFT] & 0x80) != 0)
			{
				if(tankInfo.pos.x > 0)
				{
					tankInfo.pos.x -= tankInfo.movex;				
				}else
				{
					backgroundInfo.pos.x += backgroundInfo.movex;
					if(healthExists)healthXPos += backgroundInfo.movex;
					if(ammoExists)ammoXPos += backgroundInfo.movex;

					if (backgroundInfo.pos.x > 0 && loopCount > 1)
					{
						backgroundInfo.pos.x = -(backgroundInfo.width) + SCREEN_WIDTH;
						--loopCount;
						if(loopCount < 1) loopCount = 1;
					}else if(backgroundInfo.pos.x > 0)backgroundInfo.pos.x = 0;
				}
				// Animate wheels (reverse)
				if(++tankInfo.animcount > tankInfo.animdelay)
				{
					// reset counter
					tankInfo.animcount = 0;
					//Animate the sprite
					if(--tankInfo.curframe < 0)
						tankInfo.curframe = tankInfo.lastframe;
				}
			}
			if(((mKeyboardState[DIK_RIGHT] & 0x80)!=0) || ((mKeyboardState[DIK_LEFT] & 0x80) != 0))
			{
				tankIsIdling = false;
			}else
			{
				tankIsIdling = true;
			}

			if(tankIsIdling)
			{
				//Animate wheels
				if(++tankIdleInfo.animcount > tankIdleInfo.animdelay)
				{
					//reset counter
					tankIdleInfo.animcount = 0;
					//Animate the sprite
					if(++tankIdleInfo.curframe > tankIdleInfo.lastframe)
						tankIdleInfo.curframe = 0;
				}
				tankIdleInfo.pos = tankInfo.pos;
			}

			// Should we drop a bomb?
			if(bombTimeElapsed > timeToNextBomb) dropBomb = true;
			if(dropBomb)
			{
				
					if(bombInfo.pos.y < tankInfo.pos.y)
					{
						playExplodeOnce = true;
						bombInfo.pos.y += bombInfo.movey;

						if(bombDropOffset < backgroundInfo.pos.x) // background moving to the right (number getting smaller)
						{
							bombInfo.pos.x = bombXPosition + abs(bombDropOffset - backgroundInfo.pos.x);
						} else // background moving to the left (number getting larger)
						{
							bombInfo.pos.x = bombXPosition - abs(bombDropOffset - backgroundInfo.pos.x);
						}
						
					} else
					{
						playDropOnce = true;
						explosionInfo.pos.x = bombInfo.pos.x;
						if(++explosionInfo.curframe > explosionInfo.lastframe)
						{
							explosionInfo.curframe = 0;
							bombInfo.pos.y = -20;
							// hasNotExploded = false;
							bombTimeElapsed = 0.0f;
							timeToNextBomb = 0.01f * (rand() % 100 + 1);
							dropBomb = false;
							bombXPosition = rand() % 800;
							bombDropOffset = backgroundInfo.pos.x;

							// Check for collision
							int collisionRange = abs(((int)explosionInfo.pos.x + (int)explosionInfo.center.x)-
								((int)tankInfo.pos.x + (int)tankInfo.center.x));
							if(collisionRange <= tankInfo.width/2)
							{
								
								// Stop damage for testing here
								tankDamage += 1;
								tankInfo.movex -= 1;
								backgroundInfo.movex -= 1;

								if (tankDamage > 3)
								{
									gameMode = 'o';
									tankDamage = 3;
								}
								
							}

						}
					}
			}
			

		// Health Icon
		if(healthExists)healthInfo.pos.x = healthXPos;
		if(ammoExists)ammoInfo.pos.x = ammoXPos;

		
		//Has tank reached health?
		float contact;
		if(healthExists)
		{
			contact	= abs(healthInfo.pos.x - tankInfo.pos.x);
			if (contact < tankInfo.width/2)
			{
				tankDamage -= 1;
				if (tankDamage<0){
					tankDamage = 0;
					machineGunDamage = 0;
				}
				healthExists = false;

				// Play sound on contact with health
				result = system->playSound(FMOD_CHANNEL_FREE,healthConnect,0,&healthConnectChannel);
				ERRCHECK(result);
				healthConnectChannel->setLoopCount(0);

				timeToNextHealth = rand() % 30;

			}
		}

		if(ammoExists)
		{
			contact	= abs(ammoInfo.pos.x - tankInfo.pos.x);
			if (contact < tankInfo.width/2)
			{
				tankAmmo += 2;
				ammoExists = false;

				// Play sound on contact with health
				result = system->playSound(FMOD_CHANNEL_FREE,healthConnect,0,&healthConnectChannel);
				ERRCHECK(result);
				healthConnectChannel->setLoopCount(0);

				timeToNextAmmo = rand() % 30; 

			}
		}

		// Calculate Distance
		distance = tankInfo.pos.x - (backgroundInfo.pos.x) + ((backgroundInfo.width - SCREEN_WIDTH) * (loopCount - 1));
		score = distance - gameTime;

		// Should machine gun nest be triggered?
		triggerRange = abs(backgroundInfo.pos.x - machineGunTriggerPos);
		if(triggerRange < 10.0f && loopTrigger == loopCount){
			gameMode = 'b'; // set game mode to battle
			saveBackgroundPos = backgroundInfo.pos.x; // Save background position to restore later
			saveTankPos = tankInfo.pos.x; // Save tank position to restore later
			scrollComplete = false; // Screen should be scrolled
			flashActive = true; //do muzzle flash
			shootOnce = false;
			tankBlastForward = true;
			shellFired = false;
			okToFire = true;
			machineGunNestDelay = 0;
			result = system->playSound(FMOD_CHANNEL_FREE,battleAlarm,0,&battleAlarmChannel);
			ERRCHECK(result);
			battleAlarmChannel->setLoopCount(0);
		}

		if(planeTimeElapsed > timeToNextPlane)
		{
			EnemyPlane->Spawn();
			planeTimeElapsed = 0.0f;
		}

		EnemyPlane->Update(tankInfo.pos,dt);
		
		// Updating bullets
		if(EnemyPlane->IsVisible())
		{
			if(EnemyPlane->bulletsInFlight < EnemyPlane->bulletsInBay) {
				for(int j=0;j < EnemyPlane->bulletsInBay;++j) {
					if(EnemyPlane->TheBay[j]->m_state == InBay) {
						--EnemyPlane->bulletDelay;
						if( EnemyPlane->bulletDelay <= 0) {
							EnemyPlane->TheBay[j]->Fire(EnemyPlane->pos,tankInfo.pos);
							EnemyPlane->bulletDelay = 5;
							result = system->playSound(FMOD_CHANNEL_FREE,planeBullet,0,&PlaneFireChannel);
						}
					} else {
						if(EnemyPlane->TheBay[j]->myLoc == tankInfo.pos){
							++tankDamage;
						}
					}
					if(EnemyPlane->TheBay[j]->m_state == Used) {
						EnemyPlane->TheBay[j]->m_state = InBay;
						--EnemyPlane->bulletsInFlight;/*
						EnemyPlane->bulletDelay = 10;*/
					}
				}
			}
			for(int j=0;j < EnemyPlane->bulletsInBay;++j) {
				// TODO: Check for bullet collision to tank here\

				bool contact = false;
				contact = EnemyPlane->TheBay[j]->Update(tankInfo.pos.x, tankInfo.pos.y);
					if(contact){
					result = system->playSound(FMOD_CHANNEL_FREE,ricochet,0,&ricochetChannel);
					++machineGunDamage;
					if(machineGunDamage > 100){
						machineGunDamage -= 100;
						++tankDamage;
					}
				}
			}
			//EnemyPlane->m_bullet->Update();
		}
		break;

	case 'b':
		// Scroll tank and background back to left side of screen if necessary
		if(!scrollComplete){
			backgroundInfo.pos.x -= backgroundInfo.movex * 2;
			tankInfo.pos.x -= backgroundInfo.movex * 2;
			machineGunInfo.pos.x = tankInfo.pos.x + 650;
			if(tankInfo.pos.x <= 0)scrollComplete = true; // Scrolling is finished, proceed with battle
		}

		if(!flashActive && scrollComplete)
		{
			fireTimeElapsed += dt;
			if (fireTimeElapsed > flashDelay)
			{
				flashActive = true;
				fireTimeElapsed = 0;
				shootOnce = false;
			}
		}
		if(scrollComplete) // proceed with battle only after scrolling has completed
		{
			// Generate muzzle flash
				// animate the sprite
				if(++machineGunFlashInfo.curframe > machineGunFlashInfo.lastframe)
				{
					machineGunFlashInfo.curframe = 0;
					if(flashActive)
					{
						distanceToTank = abs(machineGunInfo.pos.x - tankInfo.pos.x);
						int damageLevel;
						damageLevel = (int)(distanceToTank/130);
						switch (damageLevel)
						{
						case 5:
							machineGunDamage += 1;
							break;
						case 4:
							machineGunDamage += 2;
							break;
						case 3:
							machineGunDamage += 3;
							break;
						case 2:
							machineGunDamage += 4;
						case 1:
							machineGunDamage += 5;
							break;
						default:
							machineGunDamage += 1;
						}
						if(machineGunDamage > 100){
							machineGunDamage = machineGunDamage - 100;
							++tankDamage;
						}
						if (tankDamage > 3)
						{
							gameMode = 'o';
							tankDamage = 3;
						}
					}
				}

			if ((mKeyboardState[DIK_RIGHT] & 0x80)!=0)
			{
				if(tankInfo.pos.x < SCREEN_WIDTH - tankInfo.width - 100)
				{
					tankInfo.pos.x += tankInfo.movex;

				}
				//Animate wheels
				if(++tankInfo.animcount > tankInfo.animdelay)
				{
					//reset counter
					tankInfo.animcount = 0;
					//Animate the sprite
					if(++tankInfo.curframe > tankInfo.lastframe)
						tankInfo.curframe = 0;
				}
			}

			if ((mKeyboardState[DIK_LEFT] & 0x80) != 0)
			{
				if(tankInfo.pos.x > 0)
				{
					tankInfo.pos.x -= tankInfo.movex;				
				}
				// Animate wheels (reverse)
				if(++tankInfo.animcount > tankInfo.animdelay)
				{
					// reset counter
					tankInfo.animcount = 0;
					//Animate the sprite
					if(--tankInfo.curframe < 0)
						tankInfo.curframe = tankInfo.lastframe;
				}
			}
			if(((mKeyboardState[DIK_RIGHT] & 0x80)!=0) || ((mKeyboardState[DIK_LEFT] & 0x80) != 0))
			{
				tankIsIdling = false;
			}else
			{
				tankIsIdling = true;
			}

			if(tankIsIdling)
			{
				//Animate wheels
				if(++tankIdleInfo.animcount > tankIdleInfo.animdelay)
				{
					//reset counter
					tankIdleInfo.animcount = 0;
					//Animate the sprite
					if(++tankIdleInfo.curframe > tankIdleInfo.lastframe)
						tankIdleInfo.curframe = 0;
				}
				tankIdleInfo.pos = tankInfo.pos;
			}		
			
		}

		// Fire cannon
		if((mKeyboardState[DIK_SPACE] & 0x80) != 0 && okToFire) // Cannot fire next shot until bomb explodes
		{
			fireInProgress = true;
			tankFireDelayCount = 0;
			tankBlastOnce = false;
			okToFire = false; 
			if(--tankAmmo < 0)
			{
				tankAmmo = 0;
				fireInProgress = false; // Out of ammo, don't fire
			}
		}
		if(fireInProgress)
		{
				tankBlastInfo.animcount = 0;
				if(tankBlastForward)
				{
					if(++tankBlastInfo.curframe > tankBlastInfo.lastframe)
					{
						--tankBlastInfo.curframe;
						tankBlastForward = false;
					}
				}else if(--tankBlastInfo.curframe < 0)
				{
					++tankBlastInfo.curframe;
					tankBlastForward = true;
				}
				tankBlastInfo.pos.x = tankInfo.pos.x + tankInfo.width - 20;
				tankBlastInfo.pos.y = tankInfo.pos.y;
		}

		if(shellFired)
		{
			playDropOnce = true;
			explosionInfo.pos.x = tankInfo.pos.x + 400;
				explosionInfo.animcount = 0;
				if(++explosionInfo.curframe > explosionInfo.lastframe)
				{
					explosionInfo.curframe = 0;
					shellFired = false;
					okToFire = true; // Can take next shot now

				int nestCollisionRange = abs(((int)explosionInfo.pos.x + (int)explosionInfo.center.x)-
				((int)machineGunInfo.pos.x + (int)machineGunInfo.center.x));
				if(nestCollisionRange <= 25) // Nest has been hit!
				{
					machineGunNestDestroyed = true;
					
				}

			}
		}

		if(machineGunNestDestroyed)
		{
			if(++smokeInfo.animcount > smokeInfo.animdelay)
			{
				//reset counter
				smokeInfo.animcount = 0;
				//Animate the smoke
				if(++smokeInfo.curframe > smokeInfo.lastframe)
					smokeInfo.curframe = 0;
			}
			smokeInfo.pos.x = machineGunInfo.pos.x;
			smokeInfo.pos.y = (machineGunInfo.pos.y + machineGunInfo.height) - smokeInfo.center.y;
			flashActive = false;

			if(++machineGunNestDelay >= 100)
			{
				machineGunNestDelay = 100;
				backgroundInfo.pos.x += backgroundInfo.movex * 2;
				machineGunInfo.pos.x += backgroundInfo.movex * 2;
				if(backgroundInfo.pos.x >= saveBackgroundPos)
				{
					gameMode = 'p';
					machineGunNestDelay = 0;
					machineGunTriggerPos = rand() % (backgroundInfo.width - (2*SCREEN_WIDTH)-tankInfo.width);
					machineGunTriggerPos += SCREEN_WIDTH;
					machineGunTriggerPos = -machineGunTriggerPos;
					loopTrigger = loopCount + 1; // Set next occurance to next loop of game
					machineGunNestDestroyed = false;
					drumLoopChannel->stop();
					healthXPos = tankInfo.pos.x + 300;
					healthInfo.pos.x = healthXPos;
					healthExists = true;				
				}
			}
		}

		break;

	case 'm':
		
		// Check for menu item selection
		// Get current position of mouse
		POINT s;
		GetCursorPos(&s);
		// Make current position of mouse relative to client window
		ScreenToClient(m_hWnd, &s);

		// Check for mouse over item
		for(int i=0;i<4;i++)
		{
			menuItemSelected[i] = (s.x >= 200) && (s.x <= 200+512)
				&& (s.y >= 200+(i*72)) && (s.y <= 200+64+(i*72));
		}

		// Mouse click left button
		if (((mMouseState.rgbButtons[DIMOUSE_LEFTBUTTON] & 0x80)!= 0) || (mKeyboardState[DIK_P] & 0x80)!=0)
		{
			if(menuItemSelected[0])
			{
				gameMode = 'p';

				// Set default values for beginning of game
				menuPlayOnce = true;
				playDropOnce = true;
				playExplodeOnce = true;

				menuChannel->stop();

				// reset the game
				tankDamage = 0;
				machineGunDamage = 0;
				tankAmmo = 10;

				// put tank back at beginning
				tankInfo.pos.x = 20;
				tankInfo.pos.y = 450;
				tankInfo.rotation =0;

				// put background back at beginning
				backgroundInfo.pos.x = 0;
				backgroundInfo.pos.y = 0;

				// reset movement of tank and background
				tankInfo.movex = 6;
				backgroundInfo.movex = 6;

				//Reset time
				gameTime = 0.0f;

				// reset score
				score = 0;

				// reset sound
				playOnce = true;
				playOnceGameOver = true;
			}
			if(menuItemSelected[1])
			{
				gameMode = 'i';

			}
			if(menuItemSelected[2]) // High Score menu
			{
				scoreTable->ShowScoreTable(10);

			}
			if(menuItemSelected[3]) gameMode = 'q';
		}

		if ((mKeyboardState[DIK_P] & 0x80)!=0) gameMode = 'p'; // Game Play mode

		break;
	case 'q': // Exit the program
		exit(0);
		break;
	case 'o':

		machineGunFireChannel->isPlaying(&playing);
		if(playing)machineGunFireChannel->stop();
		channelLoop->stop();
		warChannelLoop->stop();
		if(playOnceGameOver)
		{
			result = system->playSound(FMOD_CHANNEL_FREE,gameOver,0,&channel);
			ERRCHECK(result);
			playOnceGameOver = false;
		}

		//Check for delay in animated smoke/Synchronize tank animation
		if(++smokeInfo.animcount > smokeInfo.animdelay)
		{
			//reset counter
			smokeInfo.animcount = 0;
			//Animate the smoke
			if(++smokeInfo.curframe > smokeInfo.lastframe)
				smokeInfo.curframe = 0;
			if(++tankIdleInfo.curframe > tankIdleInfo.lastframe)
				tankIdleInfo.curframe = 0;
		}
		smokeInfo.pos.x = tankInfo.pos.x;
		smokeInfo.pos.y = (tankInfo.pos.y + tankInfo.height) - smokeInfo.center.y;
		tankIdleInfo.pos = tankInfo.pos;

		if(newScore)
		{
			scoreTable->Init(m_hWnd);
			scoreTable->SetPlayerName(L"Player 1");
			scoreTable->PostScore((int)score);
			scoreTable->Flush();
			newScore = false;
			break;
		}

	default:
		break;
	}
}

}

void CDirectXFramework::Render()
{
	// If the device was not created successfully, return
	if(!m_pD3DDevice)
		return;
	//*************************************************************************


	//////////////////////////////////////////////////////////////////////////
	// All draw calls between swap chain's functions, and pre-render and post- 
	// render functions (Clear and Present, BeginScene and EndScene)
	//////////////////////////////////////////////////////////////////////////

	// Clear the back buffer, call BeginScene()
	m_pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET,D3DXCOLOR(0.8f, 0.8f, 0.8f, 0.8f),1.0f,0);

	m_pD3DDevice->BeginScene();
	m_pD3DSprite->Begin(D3DXSPRITE_ALPHABLEND);
	int tankColumns = 8;
	D3DXMATRIX	mTrans;

	switch (gameMode)
	{
	case 'p':
		
		channelLoop->isPlaying(&playing);
		if(!playing)
		{
			result = system->playSound(FMOD_CHANNEL_FREE,soundLoop,true,&channelLoop);
			ERRCHECK(result);

			channelLoop->setLoopCount(-1);
			channelLoop->setPaused(false);
			channelLoop->setVolume(.5);
		}

		warChannelLoop->isPlaying(&playing);
		if(!playing)
		{
			result = system->playSound(FMOD_CHANNEL_FREE,warLoop,true,&warChannelLoop);
			ERRCHECK(result);

			warChannelLoop->setLoopCount(-1);
			warChannelLoop->setPaused(false);
			warChannelLoop->setVolume(.5);
		}

		RECT frameSrc;
	
		D3DXMatrixTranslation(&mTrans,0,0,0);
		m_pD3DSprite->SetTransform(&mTrans);
		m_pD3DSprite->Draw(m_pBackgroundTexture,NULL,NULL,&backgroundInfo.pos,D3DCOLOR_XRGB(255,255,255));

		// Draw Health and ammo
		if (ammoExists){
			m_pD3DSprite->Draw(m_pAmmoTexture,NULL,NULL,&ammoInfo.pos,D3DCOLOR_XRGB(255,255,255));
		}
		if (healthExists)m_pD3DSprite->Draw(m_pHealthTexture,NULL,NULL,&healthInfo.pos,D3DCOLOR_XRGB(255,255,255));


		int count;

		if(tankIsIdling)
		{
			tankMovingChannel->isPlaying(&playing);
			if(playing)tankMovingChannel->stop();

			frameSrc.left = tankIdleInfo.curframe * tankIdleInfo.width;
			frameSrc.top = 0 + (tankDamage * tankIdleInfo.height);
			frameSrc.right = frameSrc.left + tankIdleInfo.width;
			frameSrc.bottom = frameSrc.top + tankIdleInfo.height;
			m_pD3DSprite->Draw(m_pTankIdleTexture,&frameSrc,NULL,&tankIdleInfo.pos,D3DCOLOR_XRGB(255,255,255));

			// Play Idling sound
			tankIdleChannel->isPlaying(&playing);
			if(!playing)
			{
				result = system->playSound(FMOD_CHANNEL_FREE,tankIdle,true,&tankIdleChannel);
				ERRCHECK(result);

				tankIdleChannel->setLoopCount(-1);
				tankIdleChannel->setPaused(false);
			}
		}else
		{
			tankIdleChannel->isPlaying(&playing);
			if(playing)tankIdleChannel->stop();

			tankMovingChannel->isPlaying(&playing);
			if(!playing)
			{
				result = system->playSound(FMOD_CHANNEL_FREE,tankMoving,true,&tankMovingChannel);
				ERRCHECK(result);

				tankMovingChannel->setLoopCount(-1);
				tankMovingChannel->setPaused(false);
			}
			frameSrc.left = tankInfo.curframe * tankInfo.width;
			frameSrc.top = 0 + (tankDamage * tankInfo.height);
			frameSrc.right = frameSrc.left + tankInfo.width;
			frameSrc.bottom = frameSrc.top + tankInfo.height;
			m_pD3DSprite->Draw(m_pTexture,&frameSrc,NULL,&tankInfo.pos,D3DCOLOR_XRGB(255,255,255));
		}

		m_pD3DSprite->Draw(m_pHUDTexture,NULL,NULL,&HUDInfo.pos,D3DCOLOR_XRGB(255,255,255));

		EnemyPlane->Render(&mTrans);
		if(EnemyPlane->IsVisible())
		{
			for(int j=0;j < EnemyPlane->bulletsInBay;++j) {
				EnemyPlane->TheBay[j]->Render(&mTrans);
			}
		}

		if(dropBomb)
		{
			if (bombInfo.pos.y < tankInfo.pos.y)
			{
				m_pD3DSprite->Draw(m_pBombTexture,NULL,NULL,&bombInfo.pos,D3DCOLOR_XRGB(255,255,255));

				// Play drop sound only once
				if(playDropOnce)
				{
					result = system->playSound(FMOD_CHANNEL_FREE,bombDrop,0,&dropBombChannel);
					ERRCHECK(result);
					dropBombChannel->setVolume(.5);
					playDropOnce = false;
				}
			}else
			{
				if(playExplodeOnce)
				{
					dropBombChannel->isPlaying(&playing);
					if(playing) dropBombChannel->stop();

					result = system->playSound(FMOD_CHANNEL_FREE,smallExplosion,0,&explodeBombChannel);
					ERRCHECK(result);
					playExplodeOnce = false;
				}
				frameSrc.left = explosionInfo.curframe * explosionInfo.width;
				frameSrc.top = 0;
				frameSrc.right = frameSrc.left + explosionInfo.width;
				frameSrc.bottom = frameSrc.top + explosionInfo.height;
				m_pD3DSprite->Draw(m_pExplosionTexture,&frameSrc,NULL,&explosionInfo.pos,D3DCOLOR_XRGB(255,255,255));
			}
		}

		break;

	case 'b':
	
		D3DXMatrixTranslation(&mTrans,0,0,0);
		m_pD3DSprite->SetTransform(&mTrans);
		m_pD3DSprite->Draw(m_pBackgroundTexture,NULL,NULL,&backgroundInfo.pos,D3DCOLOR_XRGB(255,255,255));

		warChannelLoop->isPlaying(&playing);
		if(playing)warChannelLoop->stop();
		channelLoop->isPlaying(&playing);
		if(playing)channelLoop->stop();

		drumLoopChannel->isPlaying(&playing);
		if(!playing)
		{
			result = system->playSound(FMOD_CHANNEL_FREE,drumLoop,true,&drumLoopChannel);
			ERRCHECK(result);

			drumLoopChannel->setLoopCount(-1);
			drumLoopChannel->setPaused(false);
			drumLoopChannel->setVolume(.25);
		}

		if(scrollComplete && flashActive && !machineGunNestDestroyed)
		{	// draw muzzle flash
			frameSrc.left = machineGunFlashInfo.curframe * machineGunFlashInfo.width;
			frameSrc.top = 0;
			frameSrc.right = frameSrc.left + machineGunFlashInfo.width;
			frameSrc.bottom = machineGunFlashInfo.height;
			m_pD3DSprite->Draw(m_pMachineGunFlashTexture,&frameSrc,NULL,&machineGunFlashInfo.pos,D3DCOLOR_XRGB(255,255,255));			

			if (!shootOnce)
			{
				result = system->playSound(FMOD_CHANNEL_FREE,machineGunFire,0,&machineGunFireChannel);
				ERRCHECK(result);
				machineGunFireChannel->setLoopCount(0);
				machineGunFireChannel->setPosition(0,FMOD_TIMEUNIT_MS);
				machineGunFireChannel->setVolume(0.5f);
				// machineGunFireChannel->setMode(FMOD_LOOP_OFF);
				shootOnce = true;
			}
			machineGunFireChannel->isPlaying(&playing);
			if(!playing)flashActive = false;

		}

		if(tankIsIdling)
		{
			tankMovingChannel->isPlaying(&playing);
			if(playing)tankMovingChannel->stop();

			frameSrc.left = tankIdleInfo.curframe * tankIdleInfo.width;
			frameSrc.top = 0 + (tankDamage * tankIdleInfo.height);
			frameSrc.right = frameSrc.left + tankIdleInfo.width;
			frameSrc.bottom = frameSrc.top + tankIdleInfo.height;
			m_pD3DSprite->Draw(m_pTankIdleTexture,&frameSrc,NULL,&tankIdleInfo.pos,D3DCOLOR_XRGB(255,255,255));

			// Play Idling sound
			tankIdleChannel->isPlaying(&playing);
			if(!playing)
			{
				result = system->playSound(FMOD_CHANNEL_FREE,tankIdle,true,&tankIdleChannel);
				ERRCHECK(result);

				tankIdleChannel->setLoopCount(-1);
				tankIdleChannel->setPaused(false);
			}
		}else
		{
			tankIdleChannel->isPlaying(&playing);
			if(playing)tankIdleChannel->stop();

			tankMovingChannel->isPlaying(&playing);
			if(!playing)
			{
				result = system->playSound(FMOD_CHANNEL_FREE,tankMoving,true,&tankMovingChannel);
				ERRCHECK(result);

				tankMovingChannel->setLoopCount(-1);
				tankMovingChannel->setPaused(false);
			}
			frameSrc.left = tankInfo.curframe * tankInfo.width;
			frameSrc.top = 0 + (tankDamage * tankInfo.height);
			frameSrc.right = frameSrc.left + tankInfo.width;
			frameSrc.bottom = frameSrc.top + tankInfo.height;
			m_pD3DSprite->Draw(m_pTexture,&frameSrc,NULL,&tankInfo.pos,D3DCOLOR_XRGB(255,255,255));
		}

		m_pD3DSprite->Draw(m_pHUDTexture,NULL,NULL,&HUDInfo.pos,D3DCOLOR_XRGB(255,255,255));
		
		// Draw machine gun nest only if in range
		if (machineGunInfo.pos.x < SCREEN_WIDTH)
		{
			if(!machineGunNestDestroyed)
			{
				// Draw machine gun nest only if on screen
				m_pD3DSprite->Draw(m_pMachineGunTexture,NULL,NULL,&machineGunInfo.pos,D3DCOLOR_XRGB(255,255,255));
			}else
			{
				// Stop the machine gun sound
				machineGunFireChannel->isPlaying(&playing);
				if(playing)machineGunFireChannel->stop();
				// Draw smoke first
				frameSrc.left = smokeInfo.curframe * smokeInfo.width;
				frameSrc.top = 0;
				frameSrc.right = frameSrc.left + smokeInfo.width;
				frameSrc.bottom = frameSrc.top + smokeInfo.height;
				// Draw the smoke
				m_pD3DSprite->Draw(m_pSmokeTexture,&frameSrc,NULL,&smokeInfo.pos,D3DCOLOR_XRGB(255,255,255));
				//Draw the nest
				m_pD3DSprite->Draw(m_pMachineGunNestDestroyedTexture,NULL,NULL,&machineGunInfo.pos,D3DCOLOR_XRGB(255,255,255));
			}
		}
		if(fireInProgress)
		{
			frameSrc.left = tankBlastInfo.curframe * tankBlastInfo.width;
			frameSrc.top = 0;
			frameSrc.right = frameSrc.left + tankBlastInfo.width;
			frameSrc.bottom = frameSrc.top + tankBlastInfo.height;
			m_pD3DSprite->Draw(m_pTankBlastTexture,&frameSrc,NULL,&tankBlastInfo.pos,D3DCOLOR_XRGB(255,255,255));

			if(!tankBlastOnce){
				result = system->playSound(FMOD_CHANNEL_FREE,tankBlast,0,&tankBlastChannel);
				ERRCHECK(result);
				machineGunFireChannel->setLoopCount(0);
				machineGunFireChannel->setPosition(0,FMOD_TIMEUNIT_MS);
				tankBlastOnce = true;
			}
			tankBlastChannel->isPlaying(&playing);
			if(!playing){
				fireInProgress = false;
				shellFired = true;
				playExplodeOnce = true;
				explosionInfo.curframe = 0;
			}
		}

		if(shellFired) // Explosion from tank shell
		{
			if(playExplodeOnce)
			{
				explodeBombChannel->isPlaying(&playing);
				if(playing) explodeBombChannel->stop();

				result = system->playSound(FMOD_CHANNEL_FREE,smallExplosion,0,&explodeBombChannel);
				explodeBombChannel->setLoopCount(0);
				ERRCHECK(result);
				playExplodeOnce = false;
			}
			frameSrc.left = explosionInfo.curframe * explosionInfo.width;
			frameSrc.top = 0;
			frameSrc.right = frameSrc.left + explosionInfo.width;
			frameSrc.bottom = frameSrc.top + explosionInfo.height;
			m_pD3DSprite->Draw(m_pExplosionTexture,&frameSrc,NULL,&explosionInfo.pos,D3DCOLOR_XRGB(255,255,255));
		}
		break;

	case 'm':

		menuChannel->isPlaying(&playing);
		if(!playing)
		{
			result = system->playSound(FMOD_CHANNEL_FREE,menuOpen,true,&menuChannel);
			ERRCHECK(result);

			menuChannel->setLoopCount(-1);
			menuChannel->setPaused(false);
		}
		
		D3DXMatrixTranslation(&mTrans,0,0,0);
		m_pD3DSprite->SetTransform(&mTrans);
		m_pD3DSprite->Draw(m_pMainMenuTexture,NULL,NULL,&mainMenuInfo.pos,D3DCOLOR_XRGB(255,255,255));

		// Draw Menu Items
		for (int i=0;i<4;i++)
		{
			D3DXMatrixTranslation(&mTrans,200, (200 +(i*72)),0);
			m_pD3DSprite->SetTransform(&mTrans);
			if (menuItemSelected[i])
			{
				m_pD3DSprite->Draw(m_pMenuTextureSel[i], NULL, NULL, 0, 0xFFFFFFFF);				
			}else
			{
				m_pD3DSprite->Draw(m_pMenuTexture[i], NULL, NULL, 0, 0xFFFFFFFF);
			}
		}
		break;
	case 'o':
		
		// Stop tank idle sound if it is playing
		tankIdleChannel->isPlaying(&playing);
		if(playing)tankIdleChannel->stop();

		// Stop tank moving sound if it is playing
		tankMovingChannel->isPlaying(&playing);
		if(playing)tankMovingChannel->stop();

		// Stop drumLoop sound from machine gun nest if playing
		drumLoopChannel->isPlaying(&playing);
		if(playing)drumLoopChannel->stop();
		
		frameSrc.left = tankIdleInfo.curframe * tankIdleInfo.width;
		frameSrc.top = 0 + (tankDamage * tankIdleInfo.height);
		frameSrc.right = frameSrc.left + tankIdleInfo.width;
		frameSrc.bottom = frameSrc.top + tankIdleInfo.height;

		D3DXMatrixTranslation(&mTrans,0,0,0);
		m_pD3DSprite->SetTransform(&mTrans);
		
		// Draw the background
		m_pD3DSprite->Draw(m_pBackgroundTexture,NULL,NULL,&backgroundInfo.pos,D3DCOLOR_XRGB(255,255,255));

		// Draw the tank
		m_pD3DSprite->Draw(m_pTankIdleTexture,&frameSrc,NULL,&tankIdleInfo.pos,D3DCOLOR_XRGB(255,255,255));

		frameSrc.left = smokeInfo.curframe * smokeInfo.width;
		frameSrc.top = 0;
		frameSrc.right = frameSrc.left + smokeInfo.width;
		frameSrc.bottom = frameSrc.top + smokeInfo.height;
		// Draw the smoke
		m_pD3DSprite->Draw(m_pSmokeTexture,&frameSrc,NULL,&smokeInfo.pos,D3DCOLOR_XRGB(255,255,255));
		m_pD3DSprite->Draw(m_pHUDTexture,NULL,NULL,&HUDInfo.pos,D3DCOLOR_XRGB(255,255,255));


		// Draw GameOver banner
		m_pD3DSprite->Draw(m_pGameOverTexture,NULL,NULL,&gameOverInfo.pos,D3DCOLOR_XRGB(255,255,255));


		break;
		case 'i':
			D3DXMatrixTranslation(&mTrans,0,0,0);
			m_pD3DSprite->SetTransform(&mTrans);
			m_pD3DSprite->Draw(m_pInstructionsTexture,NULL,NULL,&instructionsInfo.pos,D3DCOLOR_XRGB(255,255,255));
			break;

	default:
		break;
	}

	m_pD3DSprite->Flush();


	if(gameMode == 'p' || gameMode == 'o' || gameMode == 'b')
	{
		// Make static so memory not allocated each call
		static char distanceBuffer[20];
		// RECT D = {370, 558, 200, 600}; // Score
		RECT D = {620,558,700,600};
		// Draw items on HUD
		// Distance display
		sprintf(distanceBuffer, "%6.1f", distance);
		m_pD3DFont->DrawTextA(0, distanceBuffer, -1, &D, DT_NOCLIP, D3DCOLOR_XRGB(255,213,43));

		D.left = 490;
		D.right = 500;
		D.top = 558;
		D.bottom = 600;

		// Time display
		sprintf(distanceBuffer, "%6.1f", gameTime);
		m_pD3DFont->DrawTextA(0, distanceBuffer,-1,&D, DT_NOCLIP,D3DCOLOR_XRGB(255,213,43));

		D.left = 370;
		D.right = 450;
		D.top = 558;
		D.bottom = 600;

		// Score display
		sprintf(distanceBuffer, "%6.1f", score);
		m_pD3DFont->DrawTextA(0, distanceBuffer,-1,&D, DT_NOCLIP,D3DCOLOR_XRGB(255,213,43));

		D.left = 250;
		D.right = 350;
		D.top = 558;
		D.bottom = 600;

		// Ammo Display
		sprintf(distanceBuffer, "%d", tankAmmo);
		m_pD3DFont->DrawTextA(0, distanceBuffer,-1,&D, DT_NOCLIP,D3DCOLOR_XRGB(255,213,43));

		D.left = 150;
		D.right = 250;
		D.top = 558;
		D.bottom = 600;

		// Damage Display
		sprintf(distanceBuffer, "%2d", (tankDamage*100) + machineGunDamage);
		m_pD3DFont->DrawTextA(0, distanceBuffer,-1,&D, DT_NOCLIP,D3DCOLOR_XRGB(255,213,43));



	}
	// Make static so memory not allocated each call
	/* static char buffer[30];

	sprintf(buffer, "Frames Per Second = %3.1f", mFPS);
	// sprintf(distanceBuffer,"%3.1f",distance);
	RECT R = {15, 15, 5, 5};

	

	m_pD3DFont->DrawTextA(0, buffer, -1, &R, DT_NOCLIP, D3DCOLOR_XRGB(255,255,255)); */

	m_pD3DSprite->End();
	m_pD3DDevice->EndScene();

	//Present back buffer
	m_pD3DDevice->Present(0,0,0,0);
	system->update();
}

void CDirectXFramework::Shutdown()
{
	//*************************************************************************
	// Release COM objects in the opposite order they were created in

	// Texture
	SAFE_RELEASE(m_pTexture);
	SAFE_RELEASE(m_pBackgroundTexture);
	SAFE_RELEASE(m_pMainMenuTexture);
	SAFE_RELEASE(m_pExplosionTexture);
	SAFE_RELEASE(m_pBombTexture);

	// Sprite
	SAFE_RELEASE(m_pD3DSprite);

	// Font

	// 3DDevice
	m_pD3DDevice->Release();
		
	//Keyboard and mouse
	mDInput->Release();
	mKeyboard->Unacquire();
	mKeyboard->Release();
	mMouse->Unacquire();
	mMouse->Release();

	/* // Sound resources
	SOUND_SAFE_RELEASE(system);
	SOUND_SAFE_RELEASE(soundLoop);
	SOUND_SAFE_RELEASE(warLoop);
	SOUND_SAFE_RELEASE(gameOver);
	SOUND_SAFE_RELEASE(menuOpen);
	SOUND_SAFE_RELEASE(bombDrop);
	SOUND_SAFE_RELEASE(smallExplosion); */


	//*************************************************************************

}
