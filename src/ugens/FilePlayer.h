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
#include "BMutex.hpp"

namespace Minim 
{
	
	class FilePlayer : public UGen 
	{
	public:
		// FilePlayer will open this stream on construction.
		// It will close and delete this stream on destruction.
		FilePlayer( AudioRecordingStream * pReadStream );
		virtual ~FilePlayer();
        
        UGenInput amplitude;
        UGenInput rate;
		
		inline void play() { m_pStream->play(); }
		inline void pause() { m_pStream->pause(); }
		inline bool isPlaying() const { return m_pStream->isPlaying(); }
        
        inline void loop( int howMany ) { m_pStream->loop(howMany); }
		inline bool isLooping() const { return m_pStream->isLooping(); }
		
		void                setMillisecondPosition( const unsigned int pos );
		unsigned int        getMillisecondPosition() const;
		inline unsigned int getMillisecondLength() const { return m_pStream->getMillisecondLength(); }
        
        unsigned long       getSampleFramePosition() const;
        long                getSampleFrameLength() const { return m_streamFrameLength; }
        
        inline const char * fileName() const
		{
			if ( m_pStream )
			{
				return m_pStream->getMetaData().fileName();
			}
			return "";
		}
        
        inline int bufferSize() const
        {
            return m_buffer.getBufferSize();
        }
        
        // assumes that outSamples is large enough to hold bufferSize() samples.
        void copyBufferChannel( float * outSamples, const int channel );
		
	protected:
		virtual void uGenerate( float * channels, const int numberOfChannels );
		
	private:
		// fill the buffer and reset our output position
		void fillBuffer();
		
		// conversion functions
		inline unsigned int millisToFrames( const unsigned int millis ) const
		{
			return (unsigned int)( (float)millis / 1000.f * sampleRate() );
		}
		
		inline unsigned int framesToMillis( const unsigned int frames ) const
		{
			return (unsigned int)( (float)frames / sampleRate() * 1000.f );
		}
		
		// the stream we read from
		AudioRecordingStream *  m_pStream;
		
		// we read in entire buffers from our stream at a time
		MultiChannelBuffer      m_buffer;
		
		// we keep track of where we should uGenerate from
		float                   m_outputPosition;
        
        // how long the stream is
		long                    m_streamFrameLength;
        
        // mutex for locking when in functions that access our stream or buffer
        BMutex                  m_mutex;
	};
	
	
}

#endif // FILEPLAYER_H
