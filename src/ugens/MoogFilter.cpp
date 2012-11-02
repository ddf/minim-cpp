/*
 *  MoogFilter.cpp
 *  Unity-iPhone
 *
 *  Created by Damien Di Fede on 8/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "MoogFilter.h"
#include <string>
#include <math.h>

Minim::MoogFilter::MoogFilter( float cutoffFrequencyInHz, float normalizedResonance, Minim::MoogFilter::Type filterType )
: UGen()
, audio( *this, AUDIO )
, frequency( *this, CONTROL )
, resonance( *this, CONTROL )
, type( filterType )
, coeff( NULL )
, coeffLength( 0 )
{
	frequency.setLastValue(cutoffFrequencyInHz);
	resonance.setLastValue(normalizedResonance);
}

Minim::MoogFilter::~MoogFilter()
{
	if ( coeff )
	{
		for( int i = 0; i < coeffLength; ++i )
		{
			delete[] coeff[i];
		}
		
		delete[] coeff;
		coeff = NULL;
	}
}

void Minim::MoogFilter::channelCountChanged()
{
	const int channelCount = getAudioChannelCount();
	//printf("MoogFilter channel count is %d.\n", channelCount);
	if ( coeff && coeffLength != channelCount )
	{
		for( int i = 0; i < coeffLength; ++i )
		{
			delete[] coeff[i];
		}
		
		delete[] coeff;
		coeff = NULL;
	}
	
	if ( coeff == NULL )
	{
		coeff = new float*[channelCount];
		for( int i = 0; i < channelCount; ++i )
		{
			coeff[i] = new float[5];
			memset(coeff[i], 0, sizeof(float)*5);
		}
		coeffLength = channelCount;
	}
}

void Minim::MoogFilter::uGenerate( float * out, const int numChannels )
{
	// Set coefficients given frequency & resonance [0.0...1.0]
	float t1, t2; // temporary buffers
	float normFreq = frequency.getLastValue() / ( sampleRate() * 0.5f );
	float rez = constrain( resonance.getLastValue(), 0.f, 1.f );
	
	float q = 1.0f - normFreq;
	float p = normFreq + 0.8f * normFreq * q;
	float f = p + p - 1.0f;
	q = rez * ( 1.0f + 0.5f * q * ( 1.0f - q + 5.6f * q * q ) );
	
	const float* input = audio.getLastValues();
	
	for ( int i = 0; i < numChannels; ++i )
	{
		// Filter (in [-1.0...+1.0])
		float* b = coeff[i];
		float in = constrain( input[i], -1, 1 ); // hard clip
		//float in = atanf(input[i]) * M_2_PI; // soft clip
		
		in -= q * b[4]; // feedback
		
		t1 = b[1];
		b[1] = ( in + b[0] ) * p - b[1] * f;
		
		t2 = b[2];
		b[2] = ( b[1] + t1 ) * p - b[2] * f;
		
		t1 = b[3];
		b[3] = ( b[2] + t2 ) * p - b[3] * f;
		
		b[4] = ( b[3] + t1 ) * p - b[4] * f;
		b[4] = b[4] - b[4] * b[4] * b[4] * 0.166667f; // clipping
        
        // inelegantly squash denormals
        if ( isnan(b[4]) )
        {
            memset(b, 0, sizeof(float)*5);
        }
		
		b[0] = in;
        
        switch( type )
        {
            case HP:
                out[i] = in - b[4];
                break;
                
            case LP:
                out[i] = b[4];
                break;
                
            case BP:
                out[i] = 3.0f * (b[3] - b[4]);
                break;
        }
	}
}