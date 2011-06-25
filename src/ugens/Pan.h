/*
 *  Pan.h
 *  Minim
 *
 *  Created by Damien Di Fede on 6/25/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef PANUGEN_H
#define PANUGEN_H

#include "UGen.h"

namespace Minim 
{
	class Pan : public UGen
	{
	public:
		Pan( const float panValue );
		virtual ~Pan();
		
		// the current Pan value, between -1 and 1
		UGenInput pan;
		
		virtual void channelCountChanged();
		
	protected:
		
		virtual void addInput( UGen * input );
		virtual void removeInput( UGen * input );
		virtual void sampleRateChanged();
		virtual void uGenerate( float * sampleFrame, const int numChannels );
		
	private:
		
		UGen * m_audio;
		
	};
}

#endif // PANUGEN_H
