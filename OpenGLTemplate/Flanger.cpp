#include "Flanger.h"

// Constructor to create a flanger object
Flanger::Flanger(int sizeOfFilter, int flangerEffectSize, float flangerWaverSpeed) : _flangerEffectSize(flangerEffectSize), _flangerWaverSpeed(flangerWaverSpeed), _filterSize(sizeOfFilter)
{
	// Get mid point
	_currentFlangePoint = (int)round(sizeOfFilter / 2);
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

	change = 0;

}


Flanger::~Flanger()
{
	delete flangerFilter;
}

void Flanger::Update(float deltaTime)
{
	// Update the filter after moving the flange point based on sin

	// Sets old point to 0
	(*flangerFilter)[_currentFlangePoint] = 0;

	// Move flange point based on speed and deltatime
	change += _flangerWaverSpeed * deltaTime;

	// Get the current flange point between the flange effect size with the mid point being where the sin oscillated between. 
	_currentFlangePoint = (int)round((_flangerEffectSize * (sin(change))) + _miFlangePoint);

	// Sets new moved point to 0.5
	(*flangerFilter)[_currentFlangePoint] = 0.5;

}
