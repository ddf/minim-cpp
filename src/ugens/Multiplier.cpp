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
	: UGen()
	, m_mult(mult)
	, audio( new UGenInput( *this, AUDIO ) )
	, amplitude( new UGenInput( *this, CONTROL ) )
	{
	}
	
	Multiplier::~Multiplier()
	{
		delete audio;
		delete amplitude;
	}
	
	void Multiplier::uGenerate( float * channels, int numChannels )
	{
		for(int i = 0; i < numChannels; i++)
		{
			float tmp = audio->getLastValues()[i];
			if ( !amplitude->isPatched() )
			{
				tmp *= m_mult;
			} 
			else 
			{
				tmp *= amplitude->getLastValues()[i];
			}
			channels[i] = tmp;
		}
	}
}