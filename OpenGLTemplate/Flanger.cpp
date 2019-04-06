#include "Flanger.h"




Flanger::Flanger(int sizeOfFilter, int flangerEffectSize, float flangerWaverSpeed) : _flangerEffectSize(flangerEffectSize), _flangerWaverSpeed(flangerWaverSpeed), _filterSize(sizeOfFilter)
{
	// Get mid point
	_currentFlangePoint = round(sizeOfFilter / 2);
	_miFlangePoint = _currentFlangePoint;
	// Create filter with given size
	flangerFilter = new std::vector<double>(sizeOfFilter);
	for (int i = 0; i < sizeOfFilter; i++)
	{
		(*flangerFilter)[i] = 0.;
	}

	// Set the first point and flange point to 0.5
	(*flangerFilter)[0] = 0.5;
	(*flangerFilter)[_currentFlangePoint] = 0.5;

}


Flanger::~Flanger()
{
	delete flangerFilter;
}

void Flanger::Update(float deltaTime)
{
	// Update the filter after moving the flange point based on sin

	(*flangerFilter)[_currentFlangePoint] = 0;

	_currentFlangePoint = round(_flangerEffectSize * (sin(deltaTime) + _miFlangePoint));

	(*flangerFilter)[_currentFlangePoint] = 0.5;

}
