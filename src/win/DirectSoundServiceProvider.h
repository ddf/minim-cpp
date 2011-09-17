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

#include "..\interfaces\ServiceProvider.h"

namespace Minim
{
	class DirectSoundServiceProvider : public ServiceProvider
	{
	public:
	  DirectSoundServiceProvider();
	  virtual ~DirectSoundServiceProvider();
	  
	  virtual void start();
	  virtual void stop();
	  
	  virtual void debugOn();
	  virtual void debugOff();
	  
	  virtual AudioRecordingStream * getAudioRecordingStream( const char * filename, int bufferSize, bool inMemory );
	  virtual AudioStream * getAudioInput( const AudioFormat & inputFormat );
	  virtual AudioOut * getAudioOutput( const AudioFormat & outputFormat, int outputBufferSize );
	  virtual AudioSample * getAudioSample( const char * filename, int bufferSize );
	  virtual AudioSample * getAudioSample( MultiChannelBuffer * samples, const AudioFormat & format, int bufferSize );
	  virtual SampleRecorder * getSampleRecorder( AudioSource * sourceToRecord, const char * saveTo, const bool buffered ) { return 0; }
	};
};