/*
 *  FFT.cpp
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "FFT.h"
#include <math.h>
#include <cassert>
#include <stdio.h>
#include <string>

Minim::FFT::FFT(int timeSize, float sampleRate)
: FourierTransform(timeSize,sampleRate)
{
	assert( (timeSize & (timeSize - 1))==0 && "FFT: timeSize must be a power of two." );
	buildReverseTable();
	buildTrigTables();
}

Minim::FFT::~FFT()
{
	delete [] reverse;
	delete [] sinlookup;
	delete [] coslookup;
}

void Minim::FFT::scaleBand(int i, float s)
{
	if (s < 0)
	{
		printf("Can't scale a frequency band by a negative value.");
		return;
	}
	
	m_real[i] *= s;
	m_imag[i] *= s;
	m_spectrum[i] *= s;
	
	if (i != 0 && i != m_timeSize / 2)
	{
		m_real[m_timeSize - i] = m_real[i];
		m_imag[m_timeSize - i] = -m_imag[i];
	}
}

void Minim::FFT::setBand(int i, float a)
{
	if (a < 0)
	{
		printf("Can't set a frequency band to a negative value.");
		return;
	}
	
	if (m_real[i] == 0 && m_imag[i] == 0)
	{
		m_real[i] = a;
		m_spectrum[i] = a;
	}
	else
	{
		m_real[i] /= m_spectrum[i];
		m_imag[i] /= m_spectrum[i];
		m_spectrum[i] = a;
		m_real[i] *= m_spectrum[i];
		m_imag[i] *= m_spectrum[i];
	}
	if (i != 0 && i != m_timeSize / 2)
	{
		m_real[m_timeSize - i] = m_real[i];
		m_imag[m_timeSize - i] = -m_imag[i];
	}
}

void Minim::FFT::forward( float * buffer )
{
//	if (buffer.length != timeSize)
//	{
//		Minim
//		.error("FFT.forward: The length of the passed sample buffer must be equal to timeSize().");
//		return;
//	}
	
	doWindow( buffer, m_timeSize );
	// copy samples to real/imag in bit-reversed order
	bitReverseSamples(buffer, 0);
	// perform the fft
	fft();
	// fill the spectrum buffer with amplitudes
	fillSpectrum();
}

void Minim::FFT::forward( const float * buffReal, const float * buffImag )
{
//	if (buffReal.length != timeSize || buffImag.length != timeSize)
//	{
//		Minim
//		.error("FFT.forward: The length of the passed buffers must be equal to timeSize().");
//		return;
//	}
	
	setComplex(buffReal, buffImag);
	bitReverseComplex();
	fft();
	fillSpectrum();
}

void Minim::FFT::inverse( float * buffer )
{
//	if (buffer.length > real.length)
//	{
//		Minim
//		.error("FFT.inverse: the passed array's length must equal FFT.timeSize().");
//		return;
//	}
	
	// conjugate
	for (int i = 0; i < m_timeSize; i++)
	{
		m_imag[i] *= -1;
	}
	bitReverseComplex();
	fft();
	// copy the result in real into buffer, scaling as we do
	for (int i = 0; i < m_timeSize; i++)
	{
		buffer[i] = m_real[i] / m_timeSize;
	}
}

void Minim::FFT::fft()
{
	for (int halfSize = 1; halfSize < m_timeSize; halfSize *= 2)
	{
		// float k = -(float)Math.PI/halfSize;
		// phase shift step
		// float phaseShiftStepR = (float)Math.cos(k);
		// float phaseShiftStepI = (float)Math.sin(k);
		// using lookup table
		float phaseShiftStepR = coslu(halfSize);
		float phaseShiftStepI = sinlu(halfSize);
		// current phase shift
		float currentPhaseShiftR = 1.0f;
		float currentPhaseShiftI = 0.0f;
		for (int fftStep = 0; fftStep < halfSize; fftStep++)
		{
			for (int i = fftStep; i < m_timeSize; i += 2 * halfSize)
			{
				int off = i + halfSize;
				float tr = (currentPhaseShiftR * m_real[off]) - (currentPhaseShiftI * m_imag[off]);
				float ti = (currentPhaseShiftR * m_imag[off]) + (currentPhaseShiftI * m_real[off]);
				m_real[off] = m_real[i] - tr;
				m_imag[off] = m_imag[i] - ti;
				m_real[i] += tr;
				m_imag[i] += ti;
			}
			float tmpR = currentPhaseShiftR;
			currentPhaseShiftR = (tmpR * phaseShiftStepR) - (currentPhaseShiftI * phaseShiftStepI);
			currentPhaseShiftI = (tmpR * phaseShiftStepI) + (currentPhaseShiftI * phaseShiftStepR);
		}
	}
}

void Minim::FFT::buildReverseTable()
{
	int N = m_timeSize;
	reverse = new int[N];
	
	// set up the bit reversing table
	reverse[0] = 0;
	for (int limit = 1, bit = N / 2; limit < N; limit <<= 1, bit >>= 1)
	{
		for (int i = 0; i < limit; i++)
		{
			reverse[i + limit] = reverse[i] + bit;
		}
	}
}

void Minim::FFT::bitReverseSamples(float * samples, int startAt)
{
	for (int i = 0; i < m_timeSize; ++i)
	{
		m_real[i] = samples[ startAt + reverse[i] ];
		m_imag[i] = 0.0f;
	}
}

void Minim::FFT::bitReverseComplex()
{
	float * revReal = new float[m_timeSize];
	float * revImag = new float[m_timeSize];
	for (int i = 0; i < m_timeSize; i++)
	{
		revReal[i] = m_real[reverse[i]];
		revImag[i] = m_imag[reverse[i]];
	}
	
	memcpy(m_real, revReal, m_timeSize);
	memcpy(m_imag, revImag, m_timeSize);
	
	delete [] revReal;
	delete [] revImag;
}

void Minim::FFT::buildTrigTables()
{
	int N = m_timeSize;
	sinlookup = new float[N];
	coslookup = new float[N];
	for (int i = 0; i < N; i++)
	{
		sinlookup[i] = sinf(-M_PI / i);
		coslookup[i] = cosf(-M_PI / i);
	}
}
