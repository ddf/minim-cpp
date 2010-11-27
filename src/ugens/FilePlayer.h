/*
 *  FilePlayer.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 11/25/10.
 *  Copyright 2010 Compartmental. All rights reserved.
 *
 */

#ifndef FILEPLAYER_H
#define FILEPLAYER_H

#include "UGen.h"
#include "AudioRecordingStream.h"
#include "MultiChannelBuffer.h"

namespace Minim 
{
	
	class FilePlayer : public UGen 
	{
	public:
		// FilePlayer will open this stream on construction.
		// It will close and delete this stream on destruction.
		FilePlayer( AudioRecordingStream * pReadStream );
		virtual ~FilePlayer();
		
		inline void play() { m_pStream->play(); }
		inline void pause() { m_pStream->pause(); }
		inline bool isPlaying() { return m_pStream->isPlaying(); }
		
	protected:
		virtual void uGenerate( float * channels, const int numberOfChannels );
		
	private:
		// the stream we read from
		AudioRecordingStream * m_pStream;
		
		// we read in entire buffers from our stream at a time
		MultiChannelBuffer m_buffer;
		
		// we keep track of where we should uGenerate from
		int m_outputPosition;
	};
	
	
}

#endif // FILEPLAYER_H
