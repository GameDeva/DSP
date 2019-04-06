#pragma once

// Fix negative value

class CBuffer 
{
public:
	CBuffer();
	CBuffer(int bufferSize);
	~CBuffer();
	
	void push_back(double val);

	double atPosition(int index);

	int getTail();

private:
	static const int size = 21;
	int _bufferSize = size;
	int tail = -1;
	
	double values[];

};