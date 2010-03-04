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

#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include "AudioSource.h"
#include "UGens\Summer.h"
#include "interfaces\AudioStream.h"

namespace Minim
{
	class AudioOutput : public AudioSource
	{
	public:
		AudioOutput( AudioOut * out );

	private:
		// UGen is our friend so that it can get to our summer
		friend UGen;
		Summer mSummer;

		// an adapter class that will let us plug the Summer UGen into an AudioOut
		class SummerStream : public AudioStream
		{
		public:
			SummerStream( AudioOutput & out ) : mOutput(out) {}

			// AudioResource impl
			virtual void open() {}
			virtual void close() {}
			virtual const AudioFormat & getFormat() const { return mOutput.getFormat(); }

			virtual void read( MultiChannelBuffer & buffer );
		private:
			AudioOutput & mOutput;
		};

		SummerStream mSummerStream;

	};
};

#endif // AUDIOOUTPUT_H