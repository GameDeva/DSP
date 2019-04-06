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



FMOD_RESULT F_CALLBACK DSPCallback(FMOD_DSP_STATE * dsp_state, float * inbuffer, float * outbuffer, unsigned int length, int inchannels, int * outchannels)
{
	FMOD::DSP *thisdsp = (FMOD::DSP *)dsp_state->instance;

	Info *dspInfo;

	// float inbufferReverse[length];

	FMOD_RESULT result = thisdsp->getUserData((void **)&dspInfo);
	FmodErrorCheck(result);

	for (unsigned int samp = 0; samp < length; samp++)
	{
		for (int chan = 0; chan < *outchannels; chan++)
		{

			 dspInfo->cBuffer->atPosition(0);

			/*
			This DSP filter just halves the volume!
			Input is modified, and sent to output.
			*/

			// Add value to circular buffer			
			 dspInfo->cBuffer->push_back(inbuffer[(samp * inchannels) + chan]);

			double sum = 0;

			// For each value in the reversed set of coefficients apply convolution to the circular buffer
			int currentTail = dspInfo->cBuffer->getTail();
			int coeffSize = dspInfo->coefficientsList->size();
			for (int i = 0; i < coeffSize; i++)
			{
				if (currentTail < 0)
					break;

				sum += dspInfo->cBuffer->atPosition(currentTail) * (*dspInfo->coefficientsList)[i];
				currentTail--;
			}

			 //^ from the last point we added 
			 //Write the output to the out buffer
			
			// outbuffer[(samp * *outchannels) + chan] = sum;
			outbuffer[(samp * *outchannels) + chan] = inbuffer[(samp * inchannels) + chan];
		}
	}

	return FMOD_OK;
}

CAudio::CAudio()
{
}

CAudio::~CAudio()
{
	delete dspInfo;
}

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
	


	result = m_FmodSystem->set3DSettings(1.0f, 5.0f, .5f);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;


	dspInfo = new Info();

	dspInfo->coefficientsList = new std::vector<double>();
	maxCoefficientsList = new std::vector<double>();

	importFilter(*maxCoefficientsList);
	*dspInfo->coefficientsList = *maxCoefficientsList;

	int size = maxCoefficientsList->size();
	dspInfo->cBuffer = new CBuffer<double>(size);
	minCoefficientsList = new std::vector<double>(size);
	for (int i = 0; i < size; i++)
	{
		(*minCoefficientsList)[i] = 1.;
	}


	// Create the DSP effect
	{
		FMOD_DSP_DESCRIPTION dspdesc;
		memset(&dspdesc, 0, sizeof(dspdesc));

		strncpy_s(dspdesc.name, "My first DSP unit", sizeof(dspdesc.name));
		dspdesc.numinputbuffers = 1;
		dspdesc.numoutputbuffers = 1;
		dspdesc.read = DSPCallback;
		dspdesc.userdata = dspInfo;
		// dspdesc.setparameterdata()

		result = m_FmodSystem->createDSP(&dspdesc, &m_dsp);
		FmodErrorCheck(result);

		if (result != FMOD_OK)
			return false;
	}


	//// Occlusion
	//FMOD_VECTOR wallPoly[4];
	//ToFMODVector(glm::vec3(0,0,0), &wallPoly[0]);
	//ToFMODVector(glm::vec3(0,1,0), &wallPoly[1]);
	//ToFMODVector(glm::vec3(1,1,0), &wallPoly[2]);
	//ToFMODVector(glm::vec3(1,0,0), &wallPoly[3]);

	//m_FmodSystem->createGeometry(1, 4, &wall);
	//
	//int polyIndex = 0;
	//FMOD_VECTOR f;
	//ToFMODVector(glm::vec3(50, 100, 50), &f);
	//wall->addPolygon(1.0f, 1.0f, TRUE, 4, wallPoly, &polyIndex);
	//wall->setScale(&f);
	//ToFMODVector(glm::vec3(50, 0, 0), &wallPosition);
	//wall->setPosition(&wallPosition);
	//wall->setActive(TRUE);

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
	// 4) play through 3D channel
	m_eventChannel->setMode(FMOD_3D);
	// 6) set the position to be the horse's position
	result = m_eventChannel->set3DAttributes(0, 0, 0);
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

	// 4) play through 3D channel
	m_musicChannel->setMode(FMOD_3D);
	 // 6) set the position to be the horse's position
	result = m_musicChannel->set3DAttributes(0, 0, 0);
	FmodErrorCheck(result);
	if (result != FMOD_OK)
		return false;

	m_musicChannel->addDSP(0, m_dsp);

	return true;
}

void CAudio::Update(float currentFilterLerpValue, CCamera *camera)
{
	// Apply Dynamic filter
	getlerpValuesBetween(*maxCoefficientsList, *minCoefficientsList, *dspInfo->coefficientsList, currentFilterLerpValue);
	
	// 3D sound
	glm::mat4 mat = camera->GetViewMatrix();
	// update the listener's position with the camera position
	ToFMODVector(camera->GetPosition(), &camPos);
	glm::vec3 fd = glm::vec3(-mat[1].x, mat[1].y, mat[1].z);
	ToFMODVector(fd, &camFwd);
	glm::vec3 up = glm::vec3(-mat[2].x, mat[2].y, mat[2].z);
	ToFMODVector(up, &camUp);
	
	// ToFMODVector(camera->GetPosition(), &camPos);
	// ToFMODVector(camera->GetUpVector(), &camUp);
	// ToFMODVector(camera->GetView(), &camFwd);
	result = m_FmodSystem->set3DListenerAttributes(0, &camPos, NULL, &camFwd, &camUp);
	// FmodErrorCheck(result);
	//FMOD_VECTOR f;
	//ToFMODVector(glm::vec3(0, 0, 0), &f);
	//result = m_musicChannel->set3DAttributes(&f, 0, 0);
	FmodErrorCheck(result);

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
void CAudio::getlerpValuesBetween(const std::vector<double> &minV, const std::vector<double> &maxV, std::vector<double> &out, float percent)
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
void CAudio::CreateWall(glm::vec3 &position, float width, float height)
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
	geometry->setActive(TRUE);
}

