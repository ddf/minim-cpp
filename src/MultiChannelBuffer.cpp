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
#include <string.h> // for memcpy

namespace Minim
{

	MultiChannelBuffer::MultiChannelBuffer()
	: mBufferSize(0)
	, mChannels(NULL)
	, mChannelCount(0)
	{
	}

	MultiChannelBuffer::MultiChannelBuffer( int numChannels, int bufferSize )
	: mBufferSize(bufferSize)
	, mChannels(NULL)
	, mChannelCount(0)
	{
		setChannelCount( numChannels );
	}

	MultiChannelBuffer::MultiChannelBuffer( const MultiChannelBuffer & other )
	: mBufferSize(other.mBufferSize)
	, mChannels(NULL)
	, mChannelCount(0)
	{
		setChannelCount( other.mChannelCount );
		for(int i = 0; i < mChannelCount; i++)
		{
			memcpy(mChannels[i], other.mChannels[i], sizeof(float)*mBufferSize);
		}
	}

	MultiChannelBuffer::~MultiChannelBuffer()
	{
		deleteChannels();
	}
		
	MultiChannelBuffer & MultiChannelBuffer::operator=( const MultiChannelBuffer & other )
	{
		if ( mBufferSize != other.mBufferSize )
		{
			setBufferSize(other.mBufferSize);
		}
		
		if ( mChannelCount != other.mChannelCount ) 
		{
			setChannelCount(other.mChannelCount);
		}
		
		for(int i = 0; i < mChannelCount; i++)
		{
			memcpy(mChannels[i], other.mChannels[i], sizeof(float)*mBufferSize);
		}
		
		return *this;
	}
		
	void MultiChannelBuffer::deleteChannels()
	{	
		if ( mChannels )
		{
			for(int i = 0; i < mChannelCount; ++i)
			{
				if ( mChannels[i] )
				{
					delete [] mChannels[i];
				}
			}
			delete [] mChannels;
			mChannels = NULL;
		}
	}

	////////////////////////////////////////////////////////////
	void MultiChannelBuffer::setBufferSize( const int bufferSize )
	{
		if ( bufferSize != mBufferSize )
		{
			if ( mChannels )
			{
				for(int i = 0; i < mChannelCount; ++i)
				{
					if ( mChannels[i] )
					{
						delete [] mChannels[i];
					}
					mChannels[i] = new float[bufferSize];
				}
			}
			mBufferSize = bufferSize;
		}
	}

	////////////////////////////////////////////////////////////
	void MultiChannelBuffer::setChannelCount( int numChannels )
	{
		if ( numChannels !=  mChannelCount )
		{
			deleteChannels();
			mChannels = new Buffer[numChannels];
			mChannelCount = numChannels;
			for(int i = 0; i < mChannelCount; ++i)
			{
				mChannels[i] = new float[mBufferSize];
			}
		}
	}
		
	////////////////////////////////////////////////////////////
	void MultiChannelBuffer::makeSilence()
	{
		for(int i = 0; i < mChannelCount; ++i )
		{
			memset(mChannels[i], 0, sizeof(float) * mBufferSize);
		}
	}

	////////////////////////////////////////////////////////////
	float * MultiChannelBuffer::getChannel( const int channelNum )
	{
		assert( (channelNum < mChannelCount) && "MultiChannelBuffer tried to get a channel number that doesn't exist!" );

		return mChannels[channelNum];
	}
		
	const float * MultiChannelBuffer::getChannel( const int channelNum ) const
	{
		assert( (channelNum < mChannelCount) && "MultiChannelBuffer tried to get a channel number that doesn't exist!" );
			
		return mChannels[channelNum];
	}
	
	float MultiChannelBuffer::getSample( const int inChannel, const int sampleNum ) const
	{
		return getChannel(inChannel)[sampleNum];
	}
	
	float MultiChannelBuffer::getSample( const int inChannel, const float sampleNum ) const
	{
		const int lowSample = (int)sampleNum;
		const int hiSample  = lowSample + 1;
		const float lerpVal = sampleNum - lowSample;
		
		return mChannels[inChannel][lowSample] * (1.f - lerpVal) + mChannels[inChannel][hiSample] * lerpVal;
	}

} // namespace Minim