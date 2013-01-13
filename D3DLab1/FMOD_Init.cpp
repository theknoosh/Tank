	//////////////////////////////////////////////////////////////////////////
	// Initialize FMOD
	//////////////////////////////////////////////////////////////////////////
	// The following code is taken from the "Platform Specific Issues" section 
	// and the "Getting Started" section of the FMOD documentation.


/**************************************************************************
/**************************************************************************
 
 This code should be pasted in CDirectXFramework.h

/**************************************************************************
/**************************************************************************/

#include "fmod.hpp"
#pragma comment(lib, "fmodex_vc.lib")

#define ERRCHECK(x) if(x != FMOD_OK) {MessageBox(m_hWnd, (LPCWSTR)x, L"Fmod Error", MB_OK);}
FMOD::System    *system; 
FMOD_RESULT      result; 
unsigned int     version; 
int              numdrivers; 
FMOD_SPEAKERMODE speakermode; 
FMOD_CAPS        caps; 
char             name[256]; 

	
/**************************************************************************
/**************************************************************************
 
	This code should be pasted in CDirectXFramework::Init()
 
/**************************************************************************
/**************************************************************************/

	/*
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
		result = system->getDriverCaps(0, &caps, 0, 0, &speakermode);
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

	// Load a sound effect from file
	// FMOD_DEFAULT flag represents: FMOD_LOOP_OFF | FMOD_2D | FMOD_HARDWARE.
	result = system->createSound("tada.wav", FMOD_DEFAULT, 0, &sound);
	ERRCHECK(result);
