#include "RtAudioOut.h"
#include "AudioStream.h"
#include "AudioListener.h"


RtAudioOut::RtAudioOut( const Minim::AudioFormat & outputFormat, int outputBufferSize )
	: m_format( outputFormat )
	, m_buffer( outputFormat.getChannels(), outputBufferSize )
	, m_bufferSize( outputBufferSize )
	, m_stream( 0 )
	, m_listener( 0 )
	, m_out()
	, m_description( "RtAudioOut: " )
{
}


RtAudioOut::~RtAudioOut(void)
{
}

void RtAudioOut::setAudioStream( Minim::AudioStream* stream )
{
	BMutexLock lock( m_mutex );
	m_stream = stream;
}

void RtAudioOut::setAudioListener( Minim::AudioListener* listener )
{
	BMutexLock lock( m_mutex );
	m_listener = listener;
}

void RtAudioOut::open(void)
{
	if ( m_out.isStreamOpen() ) return;

	RtAudio::StreamParameters parameters;
	parameters.deviceId = m_out.getDefaultOutputDevice();
	parameters.nChannels = m_format.getChannels();
	parameters.firstChannel = 0;

	RtAudio::StreamOptions options;
	options.flags |= RTAUDIO_MINIMIZE_LATENCY;
	options.numberOfBuffers = 2;

	unsigned int sampleRate = (unsigned int)m_format.getSampleRate();
	unsigned int bufferFrames = m_bufferSize;

	parameters.deviceId = m_out.getDefaultOutputDevice();
	bufferFrames = m_bufferSize;
	try
	{
		m_out.openStream( &parameters, NULL, RTAUDIO_FLOAT32, sampleRate, &bufferFrames, &RtAudioOut::renderCallback, (void*)this, &options );
	}
	catch( RtError& e )
	{
		m_description += "FAILED TO OPEN OUTPUT: ";
		m_description += e.getMessage();
		m_bufferSize = 0;
	}

	if ( m_out.isStreamOpen() )
	{
		m_bufferSize = bufferFrames;
		try
		{
			m_out.startStream();

			RtAudio::DeviceInfo info = m_out.getDeviceInfo(parameters.deviceId);
			char desc[512];
			sprintf( desc, "%s with buffer size %d", info.name.c_str(), m_bufferSize );
			m_description += desc;
		}
		catch( RtError& e )
		{
			m_description += "FAILED TO START OUTPUT: ";
			m_description += e.getMessage();
			m_bufferSize   = 0;
		}
	}
}

void RtAudioOut::close(void)
{
	BMutexLock lock( m_mutex );

	try 
	{
		// Stop the stream
		m_out.stopStream();
		
		if ( m_out.isStreamOpen() )
		{
			m_out.closeStream();
		}
	}
	catch (RtError& e) 
	{
		e.printMessage();
	}
}

void RtAudioOut::pauseProcessing()
{
	if ( !m_out.isStreamRunning() ) return;

	BMutexLock lock( m_mutex );

	try
	{
		m_out.stopStream();
	}
	catch( RtError& e )
	{
		e.printMessage();
	}
}

void RtAudioOut::resumeProcessing()
{
	if ( m_out.isStreamRunning() ) return;

	BMutexLock lock( m_mutex );

	try
	{
		m_out.startStream();
	}
	catch( RtError& e )
	{
		e.printMessage();
	}
}

int RtAudioOut::renderCallback( void *outputBuffer, void *inputBuffer, unsigned int nBufferFrames, double streamTime, RtAudioStreamStatus status, void *userData )
{
	RtAudioOut* out = (RtAudioOut*)userData;
	float * buffer = (float*)outputBuffer;

	BMutexLock lock( out->m_mutex );

	out->m_buffer.setBufferSize( nBufferFrames );
	out->m_bufferSize = nBufferFrames;

	if ( out->m_stream )
	{
		const int channels = out->m_format.getChannels();
		out->m_stream->read( out->m_buffer );
		for( unsigned int i = 0; i < nBufferFrames; ++i )
		{
			for( int c = 0; c < channels; ++c )
			{
				buffer[i*channels + c] = out->m_buffer.getSample(c,(int)i);
			}
		}
	}
	else 
	{
		memset( buffer, 0, nBufferFrames*out->m_format.getChannels()*sizeof(float) );
	}

	if ( out->m_listener )
	{
		out->m_listener->samples( out->m_buffer );
	}

	return 0;
}
