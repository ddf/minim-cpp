/*
 *  TickRate.h
 *  MinimTouch
 *
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

#ifndef TICKRATE_H
#define TICKRATE_H

#include "UGen.h"

namespace Minim
{
	class TickRate : public UGen
	{
	public:
		TickRate( const float tickRate = 1.f );
		virtual ~TickRate();
		
		UGenInput value;
		
		void setInterpolation( const bool doInterpolate ) { m_bInterpolate = doInterpolate; }
		bool isInterpolating() const { return m_bInterpolate; }
		
	protected:
		// UGen overrides
		virtual void addInput( UGen * input );
		virtual void removeInput( UGen * input );
		virtual void sampleRateChanged();
		virtual void channelCountChanged();
		virtual void uGenerate( float * sampleFrame, const int numberOfChannels );
		
	private:
		UGen *  m_pAudio;
		float * m_currentSampleFrame;
		float * m_nextSampleFrame;
		int		m_sampleFrameSize;
		float   m_sampleCount;
		bool    m_bInterpolate;
		
	};
}

#endif // TICKRATE_H