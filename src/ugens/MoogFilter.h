/*
 *  MoogFilter.h
 *  Unity-iPhone
 *
 *  Created by Damien Di Fede on 8/11/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "UGen.h"

namespace Minim
{
	class MoogFilter : public UGen
	{
	public:
		enum Type
		{
			HP,
			LP
		};
		
		MoogFilter( float cutoffFrequencyInHz, float normalizedResonance, Type filterType );
		virtual ~MoogFilter();
		
		UGenInput audio;
		UGenInput frequency;
		UGenInput resonance;
		Type	  type;
		
	protected:
		virtual void channelCountChanged();
		virtual void uGenerate( float * out, const int numChannels );
		
		
	private:
		inline float constrain( float value, float min, float max )
		{
			if ( value < min ) return min;
			if ( value > max ) return max;
			return value;
		}
		
		float**	coeff;
		int		coeffLength;
	};
};