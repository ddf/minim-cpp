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

#ifndef MINIMSERVICEPROVIDER_H
#define MINIMSERVICEPROVIDER_H

namespace Minim
{

	class AudioFormat;
	class AudioStream;
	class AudioRecordingStream;
	class AudioOut;
	class AudioSample;
	class AudioSource;
	class MultiChannelBuffer;
	class SampleRecorder;

	/**
	 * <code>MinimServiceProvider</code> is the interface that an audio implementation must 
	 * provide to <code>Minim</code>, to guarantee that it can provide all of the functionality 
	 * that Minim promises. All of the interfaces in this package define functionality in the 
	 * most minimal way possible, to make it easy for people write their own implementations, since 
	 * much of what Minim provides can be done so without regard for the details of audio intput and 
	 * output. If you write your own implementation of this interface, or if you are using one that 
	 * someone else has written, all you must do is pass an instantiation of it to the Minim 
	 * constructor. That Minim object will then delegate most of the work to the implementation. 
	 * 
	 * @author ddf
	 *
	 */

	class ServiceProvider
	{
	public:
	  
	  virtual ~ServiceProvider() {}

	  /**
	   * Called inside the Minim constructor. Implementations should load any libraries and 
	   * resources they need at this time.
	   */
	  virtual void start() = 0;
	  
	  /**
	   * Called when <code>stop()</code> is called by the Minim object that owns this. 
	   * Implementations should release all resources and stop all Threads at this time.
	   *
	   */
	  virtual void stop() = 0;
	  
	  /**
	   * Tells the implementation it should produce debug output, if that's something it does.
	   *
	   */
	  virtual void debugOn() = 0;
	  
	  /**
	   * Tells the implementation it should not produce debug output.
	   *
	   */
	  virtual void debugOff() = 0;
	  
	  /**
	   * Should return an {@link AudioRecordingStream} that will stream the file requested. The filename 
	   * could be a URL, an absolute path, or just a filename that the user expects the system to find 
	   * in their sketch somewhere.
	   * 
	   * @param filename the name of the file to load into the AudioRecordingStream
	 * @param bufferSize the bufferSize to use in memory (implementations are free to ignore this, if they must)
	 * @param inMemory TODO
	   * @return an AudioRecording stream that will stream the file
	   */
	  virtual AudioRecordingStream * getAudioRecordingStream( const char * filename, int bufferSize, bool inMemory ) = 0;
	  
	  /**
	   * Should return an {@link  AudioStream} with the requested parameters. What Minim is 
	   * expecting this stream to be reading from is the active audio input of the computer, 
	   * such as the microphone or line-in.
	   */
	  virtual AudioStream * getAudioInput( const AudioFormat & inputFormat ) = 0;
	  
	  /**
	   * Should return an {@link AudioOut} that can be used to generate audio that will 
	   * be heard through the computer's speakers.
	   */
	  virtual AudioOut * getAudioOutput( const AudioFormat & outputFormat, int outputBufferSize ) = 0;
	  
	  /**
	   * Should return an {@link AudioSample} that will load the requested file into memory.
	   * 
	   * @param filename the name of the file to load, this might be a URL, an absolute path, or a 
	   * file that the user expects the implementation to find in their sketch somewhere.
	   * @param bufferSize how big the output buffer used for playing the sample should be
	   * @return an AudioSample that contains the file
	   */
	  virtual AudioSample * getAudioSample( const char * filename, int bufferSize ) = 0;
	  
	  /**
	   * Should return an {@link AudioSample} that will store the provided samples.
	   * 
	   * @param left
	   *          the left channel of the stereo sample
	   * @param right
	   *          the right channel of a stereo sample
	   * @param bufferSize
	   *          how large the output buffer should be
	   * @return
	   *          an AudioSample that contains the samples
	   */
	  virtual AudioSample * getAudioSample( MultiChannelBuffer * samples, const AudioFormat & format, int bufferSize ) = 0;
	  
	  /**
	   * Should return a {@link SampleRecorder} that can record the <code>source</code> in a 
	   * buffered (in-memory) or non-buffered (streamed) manner, to the file specified by <code>saveTo</code> 
	   * @param source the audio source that should be recorded
	   * @param saveTo the file to save the recorded audio to
	   * @param buffered whether or not to buffer all recorded audio in memory or stream directly to the file
	   * @return an appropriate SampleRecorder
	   */
	  virtual SampleRecorder * getSampleRecorder( AudioSource * sourceToRecord, const char * saveTo, const bool buffered ) = 0;
	};

};

#endif // MINIMSERVICEPROVIDER_H