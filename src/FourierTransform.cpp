/*
 *  FourierTransform.cpp
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "FourierTransform.h"

#include <string>

Minim::RectangularWindow Minim::FourierTransform::NONE;
Minim::HammingWindow Minim::FourierTransform::HAMMING;
Minim::HannWindow Minim::FourierTransform::HANN;
Minim::BartlettHannWindow Minim::FourierTransform::BARTLETHANN;

Minim::FourierTransform::FourierTransform(int ts, float sr)
: m_timeSize( ts )
, m_spectrumSize( ts/2 + 1 )
, m_sampleRate( sr )
, m_bandWidth( (2.0f /ts) * (sr / 2.0f) )
, m_real(NULL)
, m_imag(NULL)
, m_spectrum(NULL)
, m_averages(NULL)
, m_averagesSize(0)
, m_whichAverage(NOAVG)
, m_octaves(0)
, m_avgPerOctave(0)
, m_windowFunction(&Minim::FourierTransform::NONE)
{
	m_real	 = new float[m_timeSize];
	m_imag	 = new float[m_timeSize];
	m_spectrum = new float[m_spectrumSize];
}

Minim::FourierTransform::~FourierTransform()
{
	delete [] m_real;
	delete [] m_imag;
	delete [] m_spectrum;
	
	if ( m_averages )
	{
		delete [] m_averages;
	}
}

void Minim::FourierTransform::setComplex( const float * r, const float * i)
{
	memcpy(m_real, r, m_timeSize*sizeof(float));
	memcpy(m_imag, i, m_timeSize*sizeof(float));
}

void Minim::FourierTransform::fillSpectrum()
{
	for (int i = 0; i < m_spectrumSize; i++)
	{
		m_spectrum[i] = sqrtf(m_real[i] * m_real[i] + m_imag[i] * m_imag[i]);
	}
	
	if (m_whichAverage == LINAVG)
	{
		int avgWidth = (int) m_spectrumSize / m_averagesSize;
		for (int i = 0; i < m_averagesSize; i++)
		{
			float avg = 0;
			int j;
			for (j = 0; j < avgWidth; j++)
			{
				int offset = j + i * avgWidth;
				if (offset < m_spectrumSize)
				{
					avg += m_spectrum[offset];
				}
				else
				{
					break;
				}
			}
			avg /= j + 1;
			m_averages[i] = avg;
		}
	}
	else if (m_whichAverage == LOGAVG)
	{
		for (int i = 0; i < m_octaves; i++)
		{
			float lowFreq, hiFreq, freqStep;
			if (i == 0)
			{
				lowFreq = 0;
			}
			else
			{
				lowFreq = (m_sampleRate / 2) / powf(2, (float)m_octaves - i);
			}
			hiFreq = (m_sampleRate / 2) / powf(2, (float)m_octaves - i - 1);
			freqStep = (hiFreq - lowFreq) / m_avgPerOctave;
			float f = lowFreq;
			for (int j = 0; j < m_avgPerOctave; j++)
			{
				int offset = j + i * m_avgPerOctave;
				m_averages[offset] = calcAvg(f, f + freqStep);
				f += freqStep;
			}
		}
	}
}

void Minim::FourierTransform::linAverages( int numAvg )
{
	if ( numAvg > m_spectrumSize / 2 )
	{
		printf("The number of averages for this transform can be at most %d.\n", m_spectrumSize/2);
		return;
	}
	else
	{
		if ( m_averages )
		{
			delete [] m_averages;
		}
		m_averages = new float[numAvg];
	}
	m_whichAverage = LINAVG;
}

void Minim::FourierTransform::logAverages( int minBandwidth, int bandsPerOctave )
{
	float nyq = (float) m_sampleRate / 2.0f;
	m_octaves = 1;
	while ((nyq /= 2) > minBandwidth)
	{
		m_octaves++;
	}
	m_avgPerOctave = bandsPerOctave;
	if ( m_averages )
	{
		delete [] m_averages;
	}
	m_averages = new float[m_octaves * bandsPerOctave];
	m_whichAverage = LOGAVG;
}

int Minim::FourierTransform::freqToIndex( float freq ) const
{
	// special case: freq is lower than the bandwidth of spectrum[0]
	if (freq < getBandWidth() / 2) return 0;
	// special case: freq is within the bandwidth of spectrum[spectrum.length - 1]
	if (freq > m_sampleRate / 2 - getBandWidth() / 2) return m_spectrumSize - 1;
	// all other cases
	float fraction = freq / m_sampleRate;
	// roundf is not available in windows, so we do this
	int i = (int)floorf( (float)m_timeSize * fraction + 0.5f );
	return i;
}

float Minim::FourierTransform::indexToFreq(int i) const
{
	float bw = getBandWidth();
	// special case: the width of the first bin is half that of the others.
	//               so the center frequency is a quarter of the way.
	if ( i == 0 ) return bw * 0.25f;
	// special case: the width of the last bin is half that of the others.
	if ( i == m_spectrumSize - 1 ) 
	{
		float lastBinBeginFreq = (m_sampleRate / 2) - (bw / 2);
		float binHalfWidth = bw * 0.25f;
		return lastBinBeginFreq + binHalfWidth;
	}
	// the center frequency of the ith band is simply i*bw
	// because the first band is half the width of all others.
	// treating it as if it wasn't offsets us to the middle 
	// of the band.
	return i*bw;
}

float Minim::FourierTransform::getAverageCenterFrequency(int i) const
{
	if ( m_whichAverage == LINAVG )
	{
		// an average represents a certain number of bands in the spectrum
		int avgWidth = (int)m_spectrumSize / m_averagesSize;
		// the "center" bin of the average, this is fudgy.
		int centerBinIndex = i*avgWidth + avgWidth/2;
		return indexToFreq(centerBinIndex);
		
	}
	else if ( m_whichAverage == LOGAVG )
	{
		// which "octave" is this index in?
		int octave = i / m_avgPerOctave;
		// which band within that octave is this?
		int offset = i % m_avgPerOctave;
		float lowFreq, hiFreq, freqStep;
		// figure out the low frequency for this octave
		if (octave == 0)
		{
			lowFreq = 0;
		}
		else
		{
			lowFreq = (m_sampleRate / 2) / powf(2, (float)m_octaves - octave);
		}
		// and the high frequency for this octave
		hiFreq = (m_sampleRate / 2) / powf(2, (float)m_octaves - octave - 1);
		// each average band within the octave will be this big
		freqStep = (hiFreq - lowFreq) / m_avgPerOctave;
		// figure out the low frequency of the band we care about
		float f = lowFreq + offset*freqStep;
		// the center of the band will be the low plus half the width
		return f + freqStep/2;
	}
	
	return 0;
}

