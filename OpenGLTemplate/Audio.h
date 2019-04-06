#pragma once
#include <windows.h>									// Header File For The Windows Library
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include <vector>
#include "CBuffer.h"
#include "Camera.h"

// #include "Common.h"

struct Info
{
	CBuffer<double> *cBuffer;
	std::vector<double> *coefficientsList;

	~Info()
	{
		delete cBuffer;
		delete coefficientsList;
	}

};

class CAudio
{
public:
	CAudio();
	~CAudio();
	bool Initialise();
	bool LoadEventSound(char *filename);
	bool PlayEventSound();
	bool LoadMusicStream(char *filename);
	bool PlayMusicStream();
	void Update(float currentFilterLerpValue, CCamera *camera);

	void  CreateWall(glm::vec3 &position, float width, float height);

	Info *dspInfo;
	std::vector<double> *maxCoefficientsList;
	std::vector<double> *minCoefficientsList;
private:

	FMOD_RESULT result;
	FMOD::System *m_FmodSystem;	// the global variable for talking to FMOD
	FMOD::Sound *m_eventSound;
	FMOD::Channel *m_eventChannel;

	FMOD::Sound *m_music;
	FMOD::Channel *m_musicChannel;

	FMOD::DSP *m_dsp;

	// Lerp between 2 double values by percent
	double getLerpValue(double d1, double d2, float percent); 

	// Both vectors must be of same size
	void getlerpValuesBetween(const std::vector<double> &v1, const std::vector<double> &v2, std::vector<double> &out, float percent);

	void importFilter(std::vector<double> &output);
	void ToFMODVector(glm::vec3 &glVec3, FMOD_VECTOR *fmodVec);

	// fmod vectors
	FMOD_VECTOR camPos;
	FMOD_VECTOR camUp;
	FMOD_VECTOR camFwd;

	//
	FMOD::Geometry *wall;
	FMOD_VECTOR wallPosition;

};
