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

#include "AudioSource.h"
#include "AudioOut.h"

namespace Minim
{

AudioSource::AudioSource( AudioOut * out )
: mOutput(out)
, mListener( mSampleBuffer )
{
	out->setAudioListener( &mListener );
}

AudioSource::~AudioSource()
{
	delete mOutput;
}

float AudioSource::sampleRate() const
{
	return mOutput->getFormat().getSampleRate();
}

void AudioSource::close()
{
	mOutput->close();
}


} // namespace Minim