#include "CBuffer.h"

CBuffer::CBuffer() { _bufferSize = size; }

CBuffer::CBuffer(int bufferSize) : _bufferSize(bufferSize) { tail = -1; }

CBuffer::~CBuffer() {}

void CBuffer::push_back(double val)
{
	// Add value to correct position
	values[tail % size] = val;

	// Update tail value
	tail++;
}

double CBuffer::atPosition(int index)
{
	// If trying to read value not assigned
	if (index > tail)
		return values[tail % size];

	return values[index % size];
}

int CBuffer::getTail() { return tail; }