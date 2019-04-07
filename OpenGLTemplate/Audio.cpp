#include "Audio.h"

#pragma comment(lib, "lib/fmod_vc.lib")

/*
I've made these two functions non-member functions
*/

// Check for error
void FmodErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		const char *errorString = FMOD_ErrorString(result);
		// MessageBox(NULL, errorString, "FMOD Error", MB_OK);
		// Warning: error message commented out -- if headphones not plugged into computer in lab, error occurs
	}
}

// Callback that will apply the filter or the flanger filter on the samples of music in the assigned fmod system
FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE * dsp_state, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int * outchannels)
{
	// Access the instance of the dsp the callback is called by
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;
	// Create a local pointer to the userdata that we want to persist between calls
	Info *dspInfo;

	// Get the user data and assign the local pointer to it 
	FMOD_RESULT result = thisdsp->getUserData((void **)&dspInfo);
	FmodErrorCheck(result);

	CBuffer *currentCbuffer;
	// Whether flanger is used or not, assign the relevant cbuffer to be used in the convolution
	if (dspInfo->flangerOn)
		currentCbuffer = dspInfo->cBufferFlange;
	else
		currentCbuffer = dspInfo->cBuffer;

	for (unsigned int samp = 0; samp < length; samp++)
	{
		for (int chan = 0; chan < *outchannels; chan++)
		{
			// Get the 
			currentCbuffer->atPosition(0);

			// Add value to circular buffer			
			currentCbuffer->push_back(inbuffer[(samp * inchannels) + chan]);

			double sum = 0;

			// For each value in the reversed set of coefficients apply convolution to the circular buffer
			int currentTail = currentCbuffer->getTail();
			int coeffSize = dspInfo->coefficientsList->size();
			for (int i = 0; i < coeffSize; i++)
			{
				if (currentTail < 0)
					break;

				sum += currentCbuffer->atPosition(currentTail) * (*dspInfo->coefficientsList)[i];
				currentTail--;
			}

			//^ from the last point we added 
			//Write the output to the out buffer

			outbuffer[(samp * *outchannels) + chan] = sum;
			// outbuffer[(samp * *outchannels) + chan] = inbuffer[(samp * inchannels) + chan];
		}
	}

	return FMOD_OK;
}



CAudio::CAudio()
{
}

// Destructor should destroy all dynamically allocated memory
CAudio::~CAudio()
{
	delete dspInfo;
	delete minCoefficientsList;
	delete maxCoefficientsList;
}

// Initialises the audio object with an fmod system, and all the other data needed to play the sound and music
bool CAudio::Initialise()
{
	// Create an FMOD system
	result = FMOD::System_Create(&m_FmodSystem);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// Initialise the system
	result = m_FmodSystem->init(32, FMOD_INIT_3D_RIGHTHANDED, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	// Set 3D settings of the system to have, dopplerscale = 1, distancefactor = 5, rolloffscale = 0.5
	result = m_FmodSystem->set3DSettings(1.0f, 5.0f, .5f);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;


	// Create an info object that will be persisted between dsp callbacks
	dspInfo = new Info();

	dspInfo->coefficientsList = new std::vector<double>();
	maxCoefficientsList = new std::vector<double>();

	// Create new flanger object with filterSize = 100, effectsize = 5, and waverspeed = 7
	flanger = new Flanger(100, 5, 7.0f);
	dspInfo->cBufferFlange = new CBuffer(100); // Create a cbuffer to store values when convolving with the samples in the callback
	dspInfo->flangerOn = true; // Set to true to begin with flanger
	// Since we are using the flanger, the coeff list will be the flanger filter's current coeff list
	*dspInfo->coefficientsList = flanger->getFlangerFilter();

	// Import all the values from the text file on the the vectorlist of doubles which will be the maxCoeff list. 
	importFilter(*maxCoefficientsList);
	
	int size = maxCoefficientsList->size();
	dspInfo->cBuffer = new CBuffer(size); // Create cbuffer with appropriate size
	minCoefficientsList = new std::vector<double>(size);
	// All values of min coefficent list must be 0 except the first one
	for (int i = 0; i < size; i++)
	{
		if (i == 0)
			(*minCoefficientsList)[i] = 1.;
		else
			(*minCoefficientsList)[i] = 0.;
	}


	// Create the DSP effect
	{
		FMOD_DSP_DESCRIPTION dspdesc;
		memset(&dspdesc, 0, sizeof(dspdesc));

		strncpy_s(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
		dspdesc.numinputbuffers = 1;
		dspdesc.numoutputbuffers = 1;
		dspdesc.read = DSPCallback;
		dspdesc.userdata = dspInfo; // Assign data to be persisted to the userdata of the dsp
		// dspdesc.setparameterdata()

		result = m_FmodSystem->createDSP(&dspdesc, &m_dsp);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}

	return true;

}

// Load an event sound
bool CAudio::LoadEventSound(char *filename)
{
	result = m_FmodSystem->createSound(filename, FMOD_LOOP_OFF, 0, &m_eventSound);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	return true;


}

// Play an event sound
bool CAudio::PlayEventSound()
{
	result = m_FmodSystem->playSound(m_eventSound, NULL, false, &m_eventChannel);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	// play through 3D channel
	m_eventChannel->setMode(FMOD_3D);
	FMOD_VECTOR soundpos;
	// Initially place the sound at the centre, reassigned later on
	ToFMODVector(glm::vec3(205, 0, -125), &soundpos);
	result = m_eventChannel->set3DAttributes(&soundpos, 0, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;
	return true;
}


// Load a music stream
bool CAudio::LoadMusicStream(char *filename)
{
	result = m_FmodSystem->createStream(filename, NULL | FMOD_LOOP_NORMAL, 0, &m_music);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;
	return true;

}

// Play a music stream
bool CAudio::PlayMusicStream()
{
	result = m_FmodSystem->playSound(m_music, NULL, false, &m_musicChannel);
	FmodErrorCheck(result);

	if (result != FMOD_OK)
		return false;

	// play through 3D channel
	m_musicChannel->setMode(FMOD_3D);
	// set the position to be at the centre of the hut
	FMOD_VECTOR soundpos;
	ToFMODVector(glm::vec3(205, 0, -125), &soundpos);
	result = m_musicChannel->set3DAttributes(&soundpos, 0, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	m_musicChannel->addDSP(0, m_dsp);

	return true;
}

void CAudio::Update(float deltaTime, float currentFilterLerpValue, CCamera *camera, glm::vec3 soundPosition)
{
	// 3D sound
	glm::mat4 mat = camera->GetViewMatrix();
	// update the listener's position with the camera position
	ToFMODVector(camera->GetPosition(), &camPos);
	glm::vec3 fd = glm::vec3(-mat[1].x, mat[1].y, mat[1].z);
	ToFMODVector(fd, &camFwd);
	glm::vec3 up = glm::vec3(-mat[2].x, mat[2].y, mat[2].z);
	ToFMODVector(up, &camUp);

	result = m_FmodSystem->set3DListenerAttributes(0, &camPos, NULL, &camFwd, &camUp);
	FmodErrorCheck(result);

	// set the event sound position to be the sound position given from the game (i.e. horse's position)
	FMOD_VECTOR soundpos;
	ToFMODVector(soundPosition, &soundpos);
	result = m_eventChannel->set3DAttributes(&soundpos, 0, 0);
	FmodErrorCheck(result);

	FmodErrorCheck(result);

	// Based on if the flanger toggle is on or off, update the dspInfo's coefficient values 
	if (toggleFlanger)
	{
		// Update the current flanger
		flanger->Update(deltaTime);
		// Set the current flanger's filter values 
		*dspInfo->coefficientsList = flanger->getFlangerFilter();
		// Let the callback know to use the correct CBuffer 
		dspInfo->flangerOn = true;
	}
	else 
	{
		// Apply Dynamic filter based on the lerp value
		LerpBetween( *minCoefficientsList, *maxCoefficientsList, *dspInfo->coefficientsList, currentFilterLerpValue);
		// Let the callback know to use the cbuffer for the lerped filter
		dspInfo->flangerOn = false;
	}

	// Update the our fmod system
	m_FmodSystem->update();
}

// Loads all the coeffcients into a vector of doubles 
void CAudio::importFilter(std::vector<double> &output)
{
	FILE* f = fopen("resources\\Filters\\lowPassFilter.txt", "r");

	double d;

	while (1)
	{
		int val = fscanf(f, "%lf", &d);

		if (val != EOF)
			output.push_back(d);
		else
			break;
	}

	fclose(f);

}

// 1st - min, 2nd - max, 3rd - output to write to, 4th lerpvalue
void CAudio::LerpBetween(const std::vector<double> &minV, const std::vector<double> &maxV, std::vector<double> &out, float percent)
{

	int size = 21;
	for (int i = 0; i < size; i++) {
		out[i] = minV[i] + (maxV[i] - minV[i]) * percent;
	}
}


// Not useful anymore, faster to just inline lol
double CAudio::getLerpValue(double d1, double d2, float percent)
{
	return d1 - (d2 - d1) * percent;
}

void CAudio::ToFMODVector(glm::vec3 &glVec3, FMOD_VECTOR *fmodVec)
{
	fmodVec->x = glVec3.x;
	fmodVec->y = glVec3.y;
	fmodVec->z = glVec3.z;
}

/*
  This method creates the occlusion wall. Note that the poly must be *convex*!
*/
void CAudio::CreateWall(glm::vec3 &position, glm::vec3 &up, glm::vec3 &fwd, float width, float height)
{
	FMOD::Geometry *geometry;
	result = m_FmodSystem->createGeometry(1, 4, &geometry);
	FmodErrorCheck(result);
	if (result != FMOD_OK) {
		return;
	}

	float halfWidth = width / 2;
	FMOD_VECTOR wallPoly[4];
	wallPoly[0] = { -halfWidth, 0.0f, 0.0f };
	wallPoly[1] = { -halfWidth, height, 0.0f };
	wallPoly[2] = { halfWidth, height, 0.0f };
	wallPoly[3] = { halfWidth, 0.0f, 0.0f };
	int polyIndex = 0;
	result = geometry->addPolygon(1.0f, 1.0f, TRUE, 4, wallPoly, &polyIndex);
	FmodErrorCheck(result);
	if (result != FMOD_OK) {
		return;
	}

	FMOD_VECTOR wallPosition;
	ToFMODVector(position, &wallPosition);
	geometry->setPosition(&wallPosition);
	FMOD_VECTOR wallUp;
	ToFMODVector(up, &wallUp);
	FMOD_VECTOR wallFwd;
	ToFMODVector(fwd, &wallFwd);
	geometry->setRotation(&wallFwd, &wallUp);
	geometry->setActive(TRUE);
}

