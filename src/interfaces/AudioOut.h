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

#ifndef AUDIOOUT_H
#define AUDIOOUT_H

#include "AudioResource.h"

namespace Minim
{
	class AudioOut : public AudioResource
	{
	public:
	  virtual ~AudioOut() {}

	  /**
	   * @return the size of the buffer used by this output.
	   */
	  virtual const int bufferSize() const = 0;
	  
	  /**
	   * Sets the AudioStream that this output will read from to 
	   * produce samples to shove into the output.
	   *
	   * @param stream
	   */
	  virtual void setAudioStream( class AudioStream * stream ) = 0;
  
	  /**
	   * Sets the AudioListener that will have sound broadcasted to it as the output generates it.

	   * @param listen
	   */
	  virtual void setAudioListener( class AudioListener * listen ) = 0;
		
	  virtual const class MultiChannelBuffer & getOutputBuffer() const = 0;
	  
	  /**
	   * Pauses processing of the signal or stream attached to this output.
	   */
	  virtual void pauseProcessing() = 0;
	  
	  /**
	   * Resumes processing of the signal or stream attached to this output.
	   */
	  virtual void resumeProcessing() = 0;
	};

};

#endif // AUDIOOUT_H