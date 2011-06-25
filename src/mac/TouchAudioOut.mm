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
, mListener( NULL )
{
	// iPhone apparently has preferred settings, which are the values
	// behind the comments. but we go ahead and set what the format says.
	mStreamDesc.mSampleRate = format.getSampleRate(); // 44100.f
	mStreamDesc.mFormatID = kAudioFormatLinearPCM;
	
	// when working with fixed point, which we do on iOS, we need canonical flags
	// as well as the field indicating how many bits are used to represent audio.
	// turns out that if you don't have that extra flag, audio will work, but it
	// will be much quieter than you expect, probably because the system thinks you
	// are using more bits to represent the audio than you actually are.
	// Also, we use kAudioFormatFlagsCanonical here and not kAudioFormatFlagsAudioUnitCanonical
	// because the latter flag is non-interleaved, but we need to interleave for stereo output.
	// Using the flags below is consistent with the code found in CAStreamBasicDescription::SetAUCanonical.
#if CA_PREFER_FIXED_POINT
	mStreamDesc.mFormatFlags = kAudioFormatFlagsCanonical | (kAudioUnitSampleFractionBits << kLinearPCMFormatFlagsSampleFractionShift);
#else
	mStreamDesc.mFormatFlags = kAudioFormatFlagsCanonical;
#endif
	
	mStreamDesc.mBytesPerPacket = format.getFrameSize(); // sizeof(AudioUnitSampleType);
	mStreamDesc.mFramesPerPacket = 1;
	mStreamDesc.mBytesPerFrame = format.getFrameSize(); // sizeof(AudioUnitSampleType);
	mStreamDesc.mChannelsPerFrame = format.getChannels(); // 1
	mStreamDesc.mBitsPerChannel = format.getSampleSizeInBits(); // 8 * sizeof(AudioUnitSampleType);
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
		NSLog(@"%@\n", message);
	}
	
	// exit(1);
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
	const int channelCount = buffer.getChannelCount();
	const int bufferSize = buffers->mBuffers[0].mDataByteSize / sizeof(SInt32) / buffer.getChannelCount();
	buffer.setBufferSize( bufferSize );
	
	{
		//CodeTimer streamTimer("renderCallback: output->mStream->read", 0);
		output->mStream->read( buffer );
	}
	
	// assert(buffers->mNumberBuffers == buffer.getChannelCount());
	// CodeTimer timer("renderCallback: copy to data");
	for( int i = 0; i < buffers->mNumberBuffers; i++)
	{
		if ( i == 0 )
		{
			AudioBuffer & outputBuffer = buffers->mBuffers[i];
			SInt32* data = (SInt32*)outputBuffer.mData;

			for( int c = 0; c < channelCount; ++c)
			{
				float * channel = buffer.getChannel(c);
				for (int s = 0; s < bufferSize; ++s)
				{
					// there's got to be a faster way to make this conversion.
					// need to go down the floating-point magic rat hole some day.
					const float fsamp = channel[s];
					const SInt32 sampleValue = fsamp * 16777216L;
					data[s*channelCount + c] = sampleValue;
				}
			}
		}
		else
		{
			buffers->mBuffers[i].mDataByteSize = 0;
		}
	}
	
	if ( output->mListener )
	{
		output->mListener->samples( buffer );
	}
	
	return noErr;
	
}


