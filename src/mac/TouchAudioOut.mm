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
#import	 <AudioUnit/AudioUnitProperties.h>
#import  <AudioUnit/AudioOutputUnit.h>
#import  <AudioToolbox/AudioServices.h>
#include "CodeTimer.h"

const int kOutputBus = 0;

TouchAudioOut::TouchAudioOut( const Minim::AudioFormat & format, int bufferSize )
: mAudioFormat(format)
, mBufferSize(bufferSize)
, mBuffer( format.getChannels(), bufferSize )
{
	// iPhone apparently has preferred settings, which are the values
	// behind the comments. but we go ahead and set what the format says.
	mStreamDesc.mSampleRate = 44100.0;
	mStreamDesc.mFormatID = kAudioFormatLinearPCM;
	mStreamDesc.mFormatFlags  = kAudioFormatFlagsAudioUnitCanonical;
	mStreamDesc.mBytesPerPacket = sizeof(AudioUnitSampleType);
	mStreamDesc.mFramesPerPacket = 1;
	mStreamDesc.mBytesPerFrame = sizeof(AudioUnitSampleType);
	mStreamDesc.mChannelsPerFrame = 1;
	mStreamDesc.mBitsPerChannel = 8 * sizeof(AudioUnitSampleType);
	mStreamDesc.mReserved = 0;
}

TouchAudioOut::~TouchAudioOut()
{
	close();
}

void displayErrorAndExit( NSString* message, OSStatus status )
{
	if (status != noErr) 
	{
		NSLog(@"%@: %@", message,
			  [[NSError errorWithDomain:NSOSStatusErrorDomain
								   code:status
							   userInfo:nil] localizedDescription]);
	} 
	else 
	{
		NSLog(message);
	}
	exit(1);
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
	if (outputComponent == NULL) 
	{
		displayErrorAndExit( @"AudioComponentFindNext", 0 );
	}
	
	// Get audio units
	status = AudioComponentInstanceNew(outputComponent, &mAudioUnit);
	if (status) 
	{
		displayErrorAndExit(@"AudioComponentInstanceNew", status);
	}
	
	// Enable playback
	UInt32 enableIO = 1;
	status = AudioUnitSetProperty(mAudioUnit,
								  kAudioOutputUnitProperty_EnableIO,
								  kAudioUnitScope_Output,
								  kOutputBus,
								  &enableIO,
								  sizeof(UInt32));
	if (status) 
	{
		displayErrorAndExit(@"AudioUnitSetProperty EnableIO (out)", status);
	}
	
	// Apply format
	status = AudioUnitSetProperty(mAudioUnit,
								  kAudioUnitProperty_StreamFormat,
								  kAudioUnitScope_Input,
								  kOutputBus,
								  &mStreamDesc,
								  sizeof(AudioStreamBasicDescription));
	if (status) 
	{
		displayErrorAndExit(@"AudioUnitSetProperty StreamFormat", status);
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
	if (status) 
	{
		displayErrorAndExit(@"AudioUnitSetProperty SetRenderCallback", status);
	} 
	
	status = AudioUnitInitialize( mAudioUnit );
	if (status) 
	{
		displayErrorAndExit(@"AudioUnitInitialize", status);
	}

	resumeProcessing();
}

void TouchAudioOut::pauseProcessing()
{
	AudioOutputUnitStop(mAudioUnit);
}

void TouchAudioOut::resumeProcessing()
{
	OSStatus status = AudioOutputUnitStart( mAudioUnit );
	if (status) 
	{
		displayErrorAndExit(@"AudioOutputUnitStart", status);
	}
}

void TouchAudioOut::close()
{
	AudioOutputUnitStop(mAudioUnit);
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

	
	// assert( output->mStream );
	// assert( output->mListener );

	// read from our stream
	Minim::MultiChannelBuffer& buffer = output->mBuffer;
	const int bufferSize = buffers->mBuffers[0].mDataByteSize / sizeof(SInt32);
	buffer.setBufferSize( bufferSize );
	
	{
		// CodeTimer streamTimer("renderCallback: output->mStream->read");
		output->mStream->read( buffer );
	}
	
	// assert(buffers->mNumberBuffers == buffer.getChannelCount());
	// CodeTimer timer("renderCallback: copy to data");
	for( int i = 0; i < buffers->mNumberBuffers; i++)
	{
		AudioBuffer & outputBuffer = buffers->mBuffers[i];
		SInt32* data = (SInt32*)outputBuffer.mData;

		int samples = outputBuffer.mDataByteSize / sizeof(SInt32);
		// assert( samples == buffer.getBufferSize() );
		// assumes power of two!
		const float * channel = buffer.getChannel(i);
		for (int s = 0; s < samples; s += 8)
		{
			// there's got to be a faster way to make this conversion.
			// need to go down the floating-point magic rat hole some day.
			data[s]   = channel[s]   * 16777216L;
			data[s+1] = channel[s+1] * 16777216L;
			data[s+2] = channel[s+2] * 16777216L;
			data[s+3] = channel[s+3] * 16777216L;
			data[s+4] = channel[s+4] * 16777216L;
			data[s+5] = channel[s+5] * 16777216L;
			data[s+6] = channel[s+6] * 16777216L;
			data[s+7] = channel[s+7] * 16777216L;
		}
	}
	
	// output->mListener->samples( buffer );
	
	return noErr;
	
}


