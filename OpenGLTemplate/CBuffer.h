#pragma once

// Fix negative value

template <class T>
class CBuffer 
{
public:
	CBuffer() { _bufferSize = size; }
	CBuffer(int bufferSize) : _bufferSize(bufferSize) { tail = -1; }
	~CBuffer() {}
	
	void push_back(double val);

	double atPosition(int index);

	int getTail();

private:
	static const int size = 21;
	int _bufferSize = size;
	int tail = -1;
	
	T values[];
};

template <class T>
int CBuffer<T>::getTail()
{
	return tail;
}

template <class T>
double CBuffer<T>::atPosition(int index)
{
	// If trying to read value not assigned
	if (index > tail)
		return values[tail % size];

	return values[index % size];
}

template <class T>
void CBuffer<T>::push_back(double val)
{
	// Add value to correct position
	values[tail % size] = val;

	// Update tail value
	tail++;

}