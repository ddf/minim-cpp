/*
 *  Constant.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 8/26/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "UGen.h"

namespace Minim 
{

	class Constant : public UGen
	{
	public:
		Constant( const float val = 1.0f )
		: UGen()
		, value( *this, CONTROL )
		{
			value.setLastValue( val );
		}
		
		UGenInput value;
		
	protected:
		
		void uGenerate( float* channels, const int numChannels ) 
		{
			fill( channels, value.getLastValue(), numChannels );
		} 
	
	};
	
}