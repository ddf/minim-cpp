#pragma once
#include "AudioOut.h"
#include "MultiChannelBuffer.h"
#include "RtAudio.h"
#include "BMutex.hpp"

class Minim::AudioStream;
class Minim::AudioListener;

class RtAudioOut : public Minim::AudioOut
{
public:
	RtAudioOut( const Minim::AudioFormat & outputFormat, int outputBufferSize );
	virtual ~RtAudioOut(void);

	// AudioOut implementation
	virtual const int bufferSize() const { return m_bufferSize; }
	virtual void setAudioStream( Minim::AudioStream * stream );
    virtual void setAudioListener( Minim::AudioListener * listen );
	virtual const Minim::MultiChannelBuffer & getOutputBuffer() const { return m_buffer; }
	virtual void pauseProcessing();
	virtual void resumeProcessing();

	// AudioResource implementation
	virtual void open();
	virtual void close();
	virtual const Minim::AudioFormat & getFormat() const { return m_format; }

private:

	static int renderCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames,
         double streamTime, RtAudioStreamStatus status, void *userData );

	BMutex						m_mutex;
	Minim::AudioFormat			m_format;
	Minim::MultiChannelBuffer	m_buffer;
	int							m_bufferSize;
	Minim::AudioStream*			m_stream;
	Minim::AudioListener*		m_listener;
	RtAudio						m_out;
};

