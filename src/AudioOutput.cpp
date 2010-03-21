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

#include "AudioOutput.h"
#include "AudioStream.h"
#include "AudioFormat.h"

namespace Minim
{

AudioOutput::AudioOutput(Minim::AudioOut *out)
: AudioSource(out)
, mSummer(this)
, mSummerStream(*this)
{
	out->setAudioStream( &mSummerStream );
	out->open();
}


void AudioOutput::SummerStream::read( MultiChannelBuffer & buffer )
{
	const int nChannels = getFormat().getChannels();
	float tmp[ nChannels ];
	buffer.setChannelCount( nChannels );
	for(int i = 0; i < buffer.getBufferSize(); i++)
	{
		memset(tmp, 0, sizeof(float) * nChannels);
		mOutput.mSummer.tick( tmp, nChannels );
		for(int c = 0; c < nChannels; c++)
		{
			buffer.getChannel(c)[i] = tmp[c];
		}
	}
}



} // namespace Minim