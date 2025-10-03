/*
  ==============================================================================

    WrattDelay.cpp
    Created: 7 Jul 2017 1:45:54pm
    Author:  wrattchri1

  ==============================================================================
*/

#include "WrattDelay.h"
#include "Definitions.h"

namespace wratt_dsp {

	Delay::Delay()
		:
		feedback(0.5),
		mix(0.3f),
		delayLength(300),
		sampleRate(44100),
		maxDelayLen(44100)
	{
		delayLine.reserve(static_cast<size_t>(msToSamps(maxDelayLen)));
		for (long sample = 0; sample < msToSamps(maxDelayLen); sample++)
		{
			delayLine.push_back(0.0);
		}
		readPos = 0;
		delayLengthSamps = static_cast<unsigned long>(msToSamps(static_cast<double>(delayLength)));
		writePos = static_cast<int>(static_cast<unsigned long>(readPos) + delayLengthSamps - 1UL);
	}

	void Delay::setDelay(unsigned long p_delayLen)
	{
		if (p_delayLen < static_cast<unsigned long>(maxDelayLen))
		{
			delayLength = p_delayLen;
		}
		else
		{
			delayLength = static_cast<unsigned long>(maxDelayLen - 1);
		}
		delayLengthSamps = static_cast<unsigned long>(msToSamps(static_cast<double>(delayLength)));
	}

	float Delay::tick(float p_input)
	{
		float output = delayLine[static_cast<size_t>(readPos)];
		delayLine[static_cast<size_t>(writePos)] = p_input + output * feedback;
		readPos++;

		if (static_cast<unsigned long>(readPos) >= delayLengthSamps)
		{
			readPos -= static_cast<int>(delayLengthSamps);
		}
		writePos++;
		if (static_cast<unsigned long>(writePos) >= delayLengthSamps)
		{
			writePos -= static_cast<int>(delayLengthSamps);
		}

		output = delayLine[static_cast<size_t>(readPos)] * mix + p_input * (1.0f - mix);
		return output;
	}
}
