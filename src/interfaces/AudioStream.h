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

#ifndef AUDIOSTREAM_H
#define AUDIOSTREAM_H

#include "AudioResource.h"

namespace Minim
{

	class AudioStream : public AudioResource
	{
	public:

		/**
		* Reads buffer.getBufferSize() sample frames and puts them into buffer's channels.
		* The provided buffer will be forced to have the same number of channels that this 
		* AudioStream does.
		* 
		* @param buffer The MultiChannelBuffer to fill with audio samples.
		*/
		virtual void read( MultiChannelBuffer & buffer ) = 0;
	};

};

#endif // AUDIOSTREAM_H