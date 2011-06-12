/*
 *  Multiplier.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 3/17/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "Multiplier.h"

namespace Minim
{
	
	Multiplier::Multiplier( float mult )
	: UGen(2)
	, audio( *this, AUDIO )
	, amplitude( *this, CONTROL )
	{
		amplitude.setLastValue(mult);
	}
	
	Multiplier::~Multiplier()
	{
	}
	
	void Multiplier::uGenerate( float * channels, int numChannels )
	{
		const float amp = amplitude.getLastValue();
		for(int i = 0; i < numChannels; i++)
		{
			channels[i] = audio.getLastValues()[i] * amp;
		}
	}
}