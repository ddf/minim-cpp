/*
 *   Author: Damien Di Fede <ddf@compartmental.net>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as published
 *   by the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef NOISEUGEN_H
#define NOISEUGEN_H

#include "UGen.h"

namespace Minim 
{
#define NUMWHITEVALUES 6
	
	class Noise : public UGen 
	{
	public:
		enum Tint
		{
			eTintWhite = 0,
			eTintPink,
			eTintRed,
			eTintBrown,
			eTintCount
		};
		
		Noise( float fAmplitude, Tint eTint );
		
		/**
		 * Patch to this to control the amplitude of the noise with another UGen.
		 */
		UGenInput amplitude;
		
		UGenInput offset;
		
		inline Tint getTint() const { return mTint; }
		inline void setTint( const Tint noiseType )
		{
			if ( mTint != noiseType )
			{
				if ( noiseType == eTintPink )
				{
					initPink();
				}
				mTint = noiseType;
			}
		}
		
	protected:
		
		// override
		virtual void sampleRateChanged();
		
		virtual void uGenerate( float * channels, int numChannels );
		
	private:
		
		// initialize data for generating pink noise
		void	initPink();
		// generate the next pink noise value
		float	pink();
		
		Tint	mTint;
		float	mLastOutput;
		
		float	mBrownCutoffFreq;
		float	mBrownAlpha;
		float	mBrownAmpCorr;
		
		int		mMaxKey;
		int		mKey;
		int		mRange;
		
		float	mWhiteValues[NUMWHITEVALUES];
		float	mMaxSumEver;
	};
	
}

#endif // NOISEUGEN_H

