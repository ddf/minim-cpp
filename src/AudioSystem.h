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

#ifndef AUDIOSYSTEM_H
#define AUDIOSYSTEM_H

#include "ServiceProvider.h"

namespace Minim
{
	typedef void(*LogFunc)(const char *);

	void setErrorLog( LogFunc func );
	void setDebugLog( LogFunc func );

	void error(const char * errorMsg);
	void debug(const char * debugMsg);

	/// You must use an instance of AudioSystem to acquire all audio generating 
	/// related classes.

	class AudioSample;
	class AudioPlayer;
	class AudioOutput;
	class AudioInput;
	class AudioStream;
	class MultiChannelBuffer;
	class SampleRecorder;
	class AudioSource;

	class AudioSystem
	{
	public:

		AudioSystem( const int outputBufferSize );

		/// will delete the service provider
		~AudioSystem();

		  /**
		   * Turns on debug messages.
		   */
		 void debugOn();

		  /**
		   * Turns off debug messages.
		   * 
		   */
		 void debugOff();

		  /**
		   * Stops Minim.
		   * 
		   * A call to this method should be placed inside of the stop() function of
		   * your sketch. We expect that implemenations of the Minim 
		   * interface made need to do some cleanup, so this is how we 
		   * tell them it's time. 
		   * 
		   */
		  void stop() { mServiceProvider->stop(); }

		  /**
		   * Creates an {@link AudioSample} using the provided samples and AudioFormat,
		   * with the desired output buffer size.
		   * 
		   * @param samples
		   *          the samples to use
		   * @param format
		   *          the format to play them back at
		   * @param bufferSize
		   *          the output buffer size to use 
		   */
		  AudioSample * createSample(MultiChannelBuffer * samples, const AudioFormat & format, const int outputBufferSize = 1024)
		  {
			return mServiceProvider->getAudioSample(samples, format, outputBufferSize);
		  }
		  
		  /**
		   * Loads the requested file into an {@link AudioSample}.
		   * 
		   * @param filename
		   *          the file or URL that you want to load
		   * @param bufferSize
		   *          the sample buffer size you want
		   * @return an <code>AudioSample</code> with a sample buffer of the requested size
		   */
		  AudioSample * loadSample( const char * filename, const int bufferSize = 1024 )
		  {
			return mServiceProvider->getAudioSample(filename, bufferSize);
		  }

		  /**
		   * Loads the requested file into an {@link AudioPlayer} with 
		   * the request buffer size.
		   * 
		   * @param filename
		   *          the file or URL you want to load
		   * @param bufferSize
		   *          the sample buffer size you want
		   *          
		   * @return an <code>AudioPlayer</code> with a sample buffer of the requested size
		   */
		  AudioPlayer * loadFile( const char * filename, const int bufferSize = 1024 );
		  
		  /**
		   * Creates and AudioRecordingStream that you can use to read from the file yourself, 
		   * rather than wrapping it in an AudioPlayer that does the work for you.
		   * 
		   * @param filename the file to load
		   * @param bufferSize the maximum number of sample frames that can be read from the stream at once.
		   * @param inMemory whether or not the file should be cached in memory as it is read
		   * @return and AudioRecordingStream that you can use to read from the file.
		   */
		  AudioRecordingStream * loadFileStream( const char * filename, int bufferSize, bool inMemory )
		  {
			  return mServiceProvider->getAudioRecordingStream(filename, bufferSize, inMemory);
		  }
		
		  /**
		   * Loads the requested file into the provided MultiChannelBuffer.
		   *
		   * Returns the sample rate of the audio, or 0 if the load failed.
		   */
		  float loadFileIntoBuffer( const char * filename, MultiChannelBuffer & buffer );

		  /**
		   * Creates an {@link AudioRecorder} that will use <code>source</code> as its 
		   * record source and that will save to the file name specified. The format of the 
		   * file will be inferred from the extension in the file name. If the extension is 
		   * not a recognized file type, this will return null. Be aware that if you choose 
		   * buffered recording the call to {@link AudioRecorder#save()} will block until 
		   * the entire buffer is written to disk. In the event that the buffer is very large, 
		   * your sketch will noticably hang. 
		   * 
		   * @param source
		   *          the <code>Recordable</code> object you want to use as a record source
		   * @param fileName
		   *          the name of the file to record to
		   * @param buffered
		   *          whether or not to use buffered recording
		   *          
		   * @return an <code>AudioRecorder</code> for the record source
		   */
//		   AudioRecorder * createRecorder(AudioSource * sourceToRecord, const char * fileName, const bool buffered)
//		   {
//			   SampleRecorder rec = mimp.getSampleRecorder(source, fileName, buffered);
//			   if ( rec != null )
//			   {
//				   return new AudioRecorder(source, rec);
//			   }
//			   else
//			   {
//				   error("Couldn't create a SampleRecorder.");
//			   }
//			   return null;
//		   }
		
		SampleRecorder * createRecorder( AudioSource * sourceToRecord, const char * fileName, const bool buffered )
		{
			return mServiceProvider->getSampleRecorder(sourceToRecord, fileName, buffered);
		}

		  AudioInput * getAudioInput( const AudioFormat & inputFormat, int outputBufferSize );
  
		  AudioStream * getAudioInputStream( const AudioFormat & inputFormat )
		  {
			  return mServiceProvider->getAudioInput( inputFormat );
		  }

		  AudioOutput * getAudioOutput( const AudioFormat & outputFormat, int outputBufferSize );

	private:

		// the service provider we'll delegate to for the concrete implementations
		// of the interfaces we need.
		class ServiceProvider * mServiceProvider;

	};

};

#endif // AUDIOSYSTEM_H