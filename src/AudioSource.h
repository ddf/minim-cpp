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

#ifndef AUDIOSOURCE_H
#define AUDIOSOURCE_H

#include "AudioListener.h"
#include "AudioOut.h"
#include "MultiChannelBuffer.h"
#include "AudioFormat.h"

namespace Minim
{
	class AudioSource
	{
	public:
		virtual ~AudioSource();

		const MultiChannelBuffer & buffer() const { return mSampleBuffer; }

		float sampleRate() const;

		const AudioFormat & getFormat() const { return mOutput->getFormat(); }

		void close();
		
		void pauseProcessing() { mOutput->pauseProcessing(); }
		void resumeProcessing() { mOutput->resumeProcessing(); }

	protected:
		AudioSource( AudioOut * out );

	private:
		class OutputListener : public AudioListener
		{
		public:
			OutputListener( MultiChannelBuffer & targetBuffer )
				: mTargetBuffer( targetBuffer )
			{}

			void samples( const MultiChannelBuffer & inputBuffer )
			{
				mTargetBuffer = inputBuffer;
			}

		private:
			MultiChannelBuffer & mTargetBuffer;
		};

		AudioOut *		   mOutput;
		// OutputListener	   mListener;
		const MultiChannelBuffer & mSampleBuffer;
	};
};

#endif // AUDIOSOURCE_H