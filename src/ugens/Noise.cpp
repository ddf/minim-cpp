/*
 *  Noise.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 3/7/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Noise.h"
#include <math.h>
#include <stdlib.h> // for rand()

namespace Minim 
{

Noise::Noise( float amp, Tint tint )
: mAmp(amp)
, mTint(tint)
, mLastOutput(0.f)
, mBrownCutoffFreq(100.f)
, mBrownAmpCorr(6.2f)
, mBrownAlpha(0.f)
{
	if ( tint == eTintPink )
	{
		initPink();
	}
}

//////////////////////////////////////////
void Noise::sampleRateChanged()
{
	float dt = 1.0f/sampleRate();
	float RC = 1.0f/( 2.0f*(float)M_PI*mBrownCutoffFreq );
	mBrownAlpha = dt/( RC + dt );
}
	
//////////////////////////////////////////
// TODO: using rand() is not really a good idea here, need a better solution
void Noise::uGenerate( float * pChannels, int numChannels )
{
	float n;
	switch (mTint) 
	{
			// BROWN is a 1/f^2 spectrum (20db/decade, 6db/octave).
			// There is some disagreement as to whether
			// brown and red are the same, but here they are.
		case eTintBrown :
		case eTintRed :
		{
			// I admit that I'm using the filter coefficients and 
			// amplitude correction from audacity, a great audio editor.
			float normRand = (float)rand() / RAND_MAX;
			assert( normRand >= 0 && normRand <= 1 );
			n = mAmp*(2.0f*normRand - 1.0f);
			n = mBrownAlpha*n + ( 1 - mBrownAlpha )*mLastOutput;
			mLastOutput = n;
			n *= mBrownAmpCorr;
		}
			break;
			// PINK noise has a 10db/decade (3db/octave) slope
		case eTintPink :
			n = mAmp*pink();
			break;
		case eTintWhite :
		default :
		{
			float normRand = (float)rand() / (float)RAND_MAX;
			assert( normRand >= 0 && normRand <= 1 );
			n = mAmp*(2.0f*normRand - 1.0f);
		}
			break;
	}
	for(int i = 0; i < numChannels; i++)
	{
		pChannels[i] = n;
	}
}
	
//////////////////////////////////////////
void Noise::initPink()
{
	mMaxKey = 0x1f;
	mRange = 128;
	mMaxSumEver = 90;
	mKey = 0;
	for (int i = 0; i < NUMWHITEVALUES; i++)
	{
		mWhiteValues[i] = random() % (mRange / NUMWHITEVALUES);
	}
}
	
//////////////////////////////////////////
float Noise::pink()
{
	int last_key = mKey;
	float sum;
	
	mKey++;
	if (mKey > mMaxKey) 
	{
		mKey = 0;
	}
	// Exclusive-Or previous value with current value. This gives
	// a list of bits that have changed.
	int diff = last_key ^ mKey;
	sum = 0.f;
	for (int i = 0; i < NUMWHITEVALUES; i++)
	{
	    // If bit changed get new random number for corresponding
	    // white_value
	    if ((diff & (1 << i)) != 0)
	    {
			mWhiteValues[i] = random() % (mRange / NUMWHITEVALUES);;
	    }
	    sum += mWhiteValues[i];
	}
	if (sum > mMaxSumEver) 
	{
		mMaxSumEver = sum;
	}
	sum = 2.f * (sum / mMaxSumEver) - 1.f;
	return sum;
}

} // namespace Minim