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

#ifndef AUDIOFORMAT_H
#define AUDIOFORMAT_H

namespace Minim
{
	class AudioFormat
	{
	public:
		AudioFormat( float sampleRate, int channels );
		
		inline int		getChannels() const { return mChannels; }
		inline float	getFrameRate() const { return mFrameRate; }
		inline int		getFrameSize() const { return mFrameSize; }
		inline float	getSampleRate() const { return mSampleRate; }
		inline int		getSampleSizeInBits() const { return mSampleSizeInBits; }
		inline bool		isBigEndian() const	{ return mBigEndian; }
		
	protected:
		AudioFormat()
			: mChannels(0)
			, mFrameRate(0)
			, mFrameSize(0)
			, mSampleRate(0)
			, mSampleSizeInBits(0)
			, mBigEndian(0)
		{}

		int		mChannels; // how many channels
		float	mFrameRate; // the number of frames recorded per second
		int		mFrameSize; // the number of bytes in each frame
		float	mSampleRate; // the number of samples recorded per second
		int		mSampleSizeInBits; // number of bits in a single sample
		bool	mBigEndian;	// whether or not the audio is stored as big-endian or little-endian
	};
};

#endif // AUDIOFORMAT_H