using System;


public class Cbuffer <T>
{
    private int bufferSize;
    private int tail;
    private T[] values;

	public Class1(int bufferSize)
	{
        this.bufferSize = bufferSize;
	}

    public void PushBack(double val)
    {
        // Add value to correct position
        values[tail % bufferSize] = val;

        // Update tail value
        tail++;
    }

    public AtPosition(int index)
    {
        // If trying to read value not assigned
        if (index > bufferSize)
            return;

        return values[index % bufferSize];
    }


}
