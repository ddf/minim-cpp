#include "libsndSampleRecorder.h"
#include "Logging.h"

libsndSampleRecorder::libsndSampleRecorder( Minim::AudioSource* sourceToRecord, const char * recordingPath )
	: m_sndFile(0)
	, m_filePath( recordingPath )
	, m_writeBuffer( 0 )
	, m_writeBufferLength( 0 )
	, m_bRecording( false )
{
	m_fileInfo.channels = sourceToRecord->getFormat().getChannels();
	m_fileInfo.samplerate = (int)sourceToRecord->getFormat().getSampleRate();
	m_fileInfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
	// buffer sizes of record sources may vary so we want a write buffer that can hold double 
	// what the source thinks it's going to have
	m_writeBufferLength = sourceToRecord->buffer().getBufferSize()*sourceToRecord->buffer().getChannelCount()*2;
}


libsndSampleRecorder::~libsndSampleRecorder(void)
{
	closeFile();
}

void libsndSampleRecorder::openFile()
{
	if ( !m_sndFile )
	{
		m_sndFile = sf_open( filePath(), SFM_WRITE, &m_fileInfo );

		if ( m_sndFile )
		{
			m_writeBuffer = new float[ m_writeBufferLength ];
		}
		else 
		{
			Minim::error( sf_strerror(m_sndFile) );
		}
	}
}

void libsndSampleRecorder::closeFile()
{
	endRecord();

	sf_close( m_sndFile );
	m_sndFile = NULL;

	delete[] m_writeBuffer;
	m_writeBuffer = NULL;
}

void libsndSampleRecorder::samples( const Minim::MultiChannelBuffer& buffer )
{
	if ( m_bRecording )
	{
		const int frames = buffer.getBufferSize();
		const int channels = buffer.getChannelCount();
		// interleave into write buffer
		for( int i = 0; i < frames; ++i )
		{
			for( int c = 0; c < channels; ++c )
			{
				m_writeBuffer[i*channels + c] = buffer.getSample( c, i );
			}
		}
		// write it .. check to see they are all written?
		sf_writef_float( m_sndFile, m_writeBuffer, frames );
	}
}

void libsndSampleRecorder::beginRecord()
{
	openFile();
	m_bRecording = true;
}

void libsndSampleRecorder::endRecord()
{
	m_bRecording = false;
}

void libsndSampleRecorder::save()
{
	endRecord();

	sf_write_sync( m_sndFile );
	
	closeFile();
}
