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

#ifndef MULTICHANNELBUFFER_H
#define MULTICHANNELBUFFER_H

namespace Minim
{
	class MultiChannelBuffer
	{
	public:
		MultiChannelBuffer();
		MultiChannelBuffer( int numChannels, int bufferSize );
		MultiChannelBuffer & operator=(const MultiChannelBuffer & other);
		~MultiChannelBuffer();

		inline int getBufferSize() const { return mBufferSize; }
		void setBufferSize( const int bufferSize );
		
		inline int getChannelCount() const { return mChannelCount; }
		void setChannelCount( const int numChannels );

		float * getChannel( const int channelNum );
		const float * getChannel( const int channelNum ) const;
		
		void makeSilence();

	private:
		
		void deleteChannels();

		int mBufferSize;

		typedef float* Buffer;
		Buffer *mChannels;
		int		mChannelCount;
	};
};

#endif // MULTICHANNELBUFFER_H