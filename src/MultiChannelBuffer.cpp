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

#include "MultiChannelBuffer.h"
#include <cassert>

namespace Minim
{

MultiChannelBuffer::MultiChannelBuffer()
: mBufferSize(0)
{
}

MultiChannelBuffer::MultiChannelBuffer( int numChannels, int bufferSize )
: mBufferSize(bufferSize)
{
	setChannelCount( numChannels );
}

MultiChannelBuffer::~MultiChannelBuffer()
{
}

////////////////////////////////////////////////////////////
void MultiChannelBuffer::setBufferSize( const int bufferSize )
{
	if ( bufferSize != mBufferSize )
	{
		for( std::vector<Buffer>::iterator itr = mChannels.begin(); itr != mChannels.end(); ++itr )
		{
			itr->resize( bufferSize );
		}

		mBufferSize = bufferSize;
	}
}

////////////////////////////////////////////////////////////
void MultiChannelBuffer::setChannelCount( int numChannels )
{
	if ( numChannels != (int)mChannels.size() )
	{
		mChannels.resize( numChannels );
		for( std::vector<Buffer>::iterator itr = mChannels.begin(); itr != mChannels.end(); ++itr )
		{
			itr->resize( mBufferSize );
		}
	}
}

////////////////////////////////////////////////////////////
std::vector<float> & MultiChannelBuffer::getChannel( const int channelNum )
{
	assert( (channelNum < (int)mChannels.size()) && "MultiChannelBuffer tried to get a channel number that doesn't exist!" );

	return mChannels[channelNum];
}

////////////////////////////////////////////////////////////
void MultiChannelBuffer::setChannel(int channelNum, const std::vector<float> &samples)
{
	Buffer channel = getChannel( channelNum );
	assert( (channel.size() == samples.size()) && "MultiChannelBuffer setChannel passed a vector of samples that is the wrong size!" );
	channel = samples;
}

} // namespace Minim