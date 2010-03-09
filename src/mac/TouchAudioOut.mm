//
//  TouchAudioOut.mm
//  MinimTouch
//
//  Created by Damien Di Fede on 3/7/10.
//  Copyright 2010 __MyCompanyName__. All rights reserved.
//

#include "TouchAudioOut.h"
#include "AudioStream.h"
#include "MultiChannelBuffer.h"
#include "AudioListener.h"

const int kOutputBus = 0;

TouchAudioOut::TouchAudioOut( const Minim::AudioFormat & format, int bufferSize )
: mAudioFormat(format)
, mBufferSize(bufferSize)
, mBuffer( format.getChannels(), bufferSize )
{
	// iPhone apparently has preferred settings, which are the values
	// behind the comments. but we go ahead and set what the format says.
	mStreamDesc.mSampleRate = format.getSampleRate(); // 44100.0;
	mStreamDesc.mFormatID = kAudioFormatLinearPCM;
	mStreamDesc.mFormatFlags  = kAudioFormatFlagsAudioUnitCanonical;
	mStreamDesc.mBytesPerPacket = format.getChannels() * sizeof(AudioUnitSampleType);
	mStreamDesc.mFramesPerPacket = 1;
	mStreamDesc.mBytesPerFrame = sizeof(AudioUnitSampleType);
	mStreamDesc.mChannelsPerFrame = format.getChannels(); // 1;
	mStreamDesc.mBitsPerChannel = 8 * sizeof(AudioUnitSampleType);
	mStreamDesc.mReserved = 0;
}

TouchAudioOut::~TouchAudioOut()
{
	close();
}


void TouchAudioOut::open() 
{
	if ( mStream )
	{
		mStream->open();
	}
	
	OSStatus status;
	// Describe audio component
	AudioComponentDescription desc;
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_RemoteIO;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	
	// Get component
	AudioComponent outputComponent = AudioComponentFindNext(NULL, &desc);
	if (outputComponent == NULL) {
//		[AudioOutput displayErrorAndExit:@"AudioComponentFindNext"
//							   errorCode:0];
	}
	
	// Get audio units
	status = AudioComponentInstanceNew(outputComponent, &mAudioUnit);
	if (status) {
//		[AudioOutput displayErrorAndExit:@"AudioComponentInstanceNew"
//							   errorCode:status];
	}
	
	// Enable playback
	UInt32 enableIO = 1;
	status = AudioUnitSetProperty(mAudioUnit,
								  kAudioOutputUnitProperty_EnableIO,
								  kAudioUnitScope_Output,
								  kOutputBus,
								  &enableIO,
								  sizeof(UInt32));
	if (status) {
//		[AudioOutput displayErrorAndExit:@"AudioUnitSetProperty EnableIO (out)"
//							   errorCode:status];
	}
	
	// Apply format
	status = AudioUnitSetProperty(mAudioUnit,
								  kAudioUnitProperty_StreamFormat,
								  kAudioUnitScope_Input,
								  kOutputBus,
								  &mStreamDesc,
								  sizeof(AudioStreamBasicDescription));
	if (status) {
//		[AudioOutput displayErrorAndExit:@"AudioUnitSetProperty StreamFormat"
//							   errorCode:status];
	}
	
	AURenderCallbackStruct callback;
	callback.inputProc = &TouchAudioOut::renderCallback;
	callback.inputProcRefCon = this;
	
	// Set output callback
	status = AudioUnitSetProperty(mAudioUnit,
								  kAudioUnitProperty_SetRenderCallback,
								  kAudioUnitScope_Global,
								  kOutputBus,
								  &callback,
								  sizeof(AURenderCallbackStruct));
	if (status) {
//		[AudioOutput displayErrorAndExit:@"AudioUnitSetProperty SetRenderCallback"
//							   errorCode:status];
	} 
	
	status = AudioUnitInitialize( mAudioUnit );
	if (status) {
//		[AudioOutput displayErrorAndExit:@"AudioUnitInitialize"
//							   errorCode:status];
	}
	
	status = AudioOutputUnitStart( mAudioUnit );
	if (status) {
//		[AudioOutput displayErrorAndExit:@"AudioOutputUnitStart"
//							   errorCode:status];
	}
}

void TouchAudioOut::close()
{
	// TODO stop output processing, release resources.
	AudioUnitUninitialize(mAudioUnit);
	
	if ( mStream )
	{
		mStream->close();
		mStream = NULL;
	}
	mListener = NULL;
}

///////////////////////////////////////////
void TouchAudioOut::setAudioStream( Minim::AudioStream * stream )
{
	assert( stream && "Can't set a NULL AudioStream!" );
	mStream = stream;
}

void TouchAudioOut::setAudioListener( Minim::AudioListener * listen )
{
	assert( listen && "Can't set a NULL AudioListener!" );
	mListener = listen;
}

///////////////////////////////////////////
OSStatus TouchAudioOut::renderCallback( void                        *inRefCon,
									    AudioUnitRenderActionFlags  *ioActionFlags,
									    const AudioTimeStamp        *inTimeStamp,
									    UInt32                      inBusNumber,
									    UInt32                      inNumberFrames,
									    AudioBufferList             *buffers
									   )
{
	assert( kOutputBus == inBusNumber );
	TouchAudioOut * output = static_cast<TouchAudioOut*> (inRefCon);
	
	assert( output->mStream );
	assert( output->mListener );

	// read from our stream
	Minim::MultiChannelBuffer& buffer = output->mBuffer;
	const int bufferSize = buffers->mBuffers[0].mDataByteSize / sizeof(SInt32);
	buffer.setBufferSize( bufferSize );
	output->mStream->read( buffer );
	
	assert(buffers->mNumberBuffers == buffer.getChannelCount());
	
	for( int i = 0; i < buffers->mNumberBuffers; i++)
	{
		AudioBuffer & outputBuffer = buffers->mBuffers[i];
		SInt32* data = (SInt32*)outputBuffer.mData;

		int samples = outputBuffer.mDataByteSize / sizeof(SInt32);
		// assert( samples == buffer.getBufferSize() );
		for (int s = 0; s < samples; ++s) 
		{
			data[s] = buffer.getChannel(i)[s] * 16777216L;
		}
	}
	
	output->mListener->samples( buffer );
	
	return noErr;
	
}


