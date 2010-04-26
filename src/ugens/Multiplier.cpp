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
	, m_mult(mult)
	, audio( this, AUDIO )
	, amplitude( this, CONTROL )
	{
	}
	
	Multiplier::~Multiplier()
	{
	}
	
	void Multiplier::uGenerate( float * channels, int numChannels )
	{
		for(int i = 0; i < numChannels; i++)
		{
			float tmp = audio.getLastValues()[i];
			if ( amplitude.isPatched()  )
			{
				tmp *= amplitude.mIncoming->getLastValues()[i];
			} 
			else 
			{
				tmp *= m_mult;
			}
			channels[i] = tmp;
		}
	}
}