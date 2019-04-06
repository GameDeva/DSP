#pragma once
#include "./include/fmod_studio/fmod.hpp"
#include "./include/fmod_studio/fmod_errors.h"
#include <vector>

class Flanger
{
public:
	Flanger(int sizeOfFilter, int flangerEffectSize, float flangerWaverSpeed);
	~Flanger();

	std::vector<double> &getFlangerFilter() { return *flangerFilter; }
	void Update(float deltaTime);

private:
	float change;
	int _filterSize;
	int _miFlangePoint;
	int _currentFlangePoint;
	int _flangerEffectSize;
	int _flangerWaverSpeed;
	std::vector<double> *flangerFilter;


};

