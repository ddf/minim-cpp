/*
 *  Pan.cpp
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 6/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "Pan.h"

#define  _USE_MATH_DEFINES // required in windows to get constants like M_PI
#include <math.h>

#include <cassert>

namespace Minim
{
	Pan::Pan( const float panValue )
	: UGen(1)
	, pan( *this, CONTROL )
	, m_audio( NULL )
	{
		pan.setLastValue(panValue);
	}
	
	Pan::~Pan()
	{
	}
	
	void Pan::addInput( UGen * in )
	{
		m_audio = in;
		m_audio->setAudioChannelCount(1);
	}
	
	void Pan::removeInput( UGen * in )
	{
		if ( m_audio == in )
		{
			m_audio = NULL;
		}
	}
	
	void Pan::sampleRateChanged()
	{
		if ( m_audio != NULL )
		{
			m_audio->setSampleRate( sampleRate() );
		}
	}
	
	void Pan::channelCountChanged()
	{
		assert( getAudioChannelCount() == 2 && "Pan should be ticked by a STEREO ouput! It doesn't make sense in any other context!" );
	}
	
	void Pan::uGenerate( float * sampleFrame, const int numChannels ) 
	{
		// tick our audio as MONO because that's what a Pan is for!
		float sample( 0.f );
		if ( m_audio != NULL )
		{
			m_audio->tick(&sample, 1);
		}
		
		// if we don't have stereo output, we just do nothing and pass-through the audio.
		if ( numChannels != 2 )
		{
			fill( sampleFrame, sample, numChannels );
			return;
		}
		
		const float panValue = pan.getLastValue();
		
		// formula swiped from the MIDI spcification: http://www.midi.org/about-midi/rp36.shtml
		// Left Channel Gain [dB] = 20*log (cos (Pi/2* max(0,CC#10 ñ 1)/126)
		// Right Channel Gain [dB] = 20*log (sin (Pi /2* max(0,CC#10 ñ 1)/126)
		
		// dBvalue = 20.0 * log10 ( linear );
		// dB = 20 * log (linear)
		
		// conversely...
		// linear = pow ( 10.0, (0.05 * dBvalue) );
		// linear = 10^(dB/20)
		
		const float normBalance = (panValue + 1.f) * 0.5f;
		
		// note that I am calculating amplitude directly, by using the linear value
		// that the MIDI specification suggests inputing into the dB formula.
		const float leftAmp = cosf( M_PI_2 * normBalance );
		const float rightAmp = sinf( M_PI_2 * normBalance);
		
		sampleFrame[0] = sample * leftAmp;
		sampleFrame[1] = sample * rightAmp;
	}
}