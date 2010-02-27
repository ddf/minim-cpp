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
	class AudioSignal;
	class AudioStream;
	class AudioEffect;
	class AudioListener;

	class AudioOut : public AudioResource
	{
	public:
	  /**
	   * @return the size of the buffer used by this output.
	   */
	  virtual int bufferSize() const = 0;
	  
	  /**
	   * Sets the AudioSignal that this output will use to generate sound.
	   * 
	   * @param signal
	   *          the AudioSignal used to generate sound
	   */
	  virtual void setAudioSignal( AudioSignal * signal ) = 0;
	  
	  /**
	   * Sets the AudioStream that this output will use to generate sound.
	   * @param stream
	   */
	  virtual void setAudioStream( AudioStream * stream ) = 0;

	  /**
	   * Sets the AudioEffect to apply to the signal.
	   * 
	   * @param effect
	   *          the AudioEffect to apply to the signal
	   */
	  virtual void setAudioEffect( AudioEffect * effect ) = 0;
	  
	  /**
	   * Sets the AudioListener that will have sound broadcasted to it as the output generates.
	   * @param listen
	   */
	  virtual void setAudioListener( AudioListener * listen ) = 0;
	  
	  /**
	   * Pauses processing of the signal or stream attached to this output.
	   */
	  virtual void pauseProcessing() = 0;
	  
	  /**
	   * Resumes processing of the signal or stream attached to this output.
	   */
	  virtual void resumeProcessing() = 0;
}

};

#endif // AUDIOOUT_H