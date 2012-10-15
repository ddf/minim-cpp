//
//  TouchServiceProvider.mm
//  MinimTouch
//
//  Created by Damien Di Fede on 3/6/10.
//  Copyright 2010 Compartmental. All rights reserved.
//

#include "AudioSystem.h"
#include "TouchServiceProvider.h"
#include "TouchAudioOut.h"
#include "Wavetable.h"
#include "TouchAudioRecordingStream.h"
#include "CASampleRecorder.h"
#include "AudioSource.h"

extern void displayErrorAndExit( NSString* message, OSStatus status );

// helper for creating a URL to a file, since that's what the Ext audio file stuff needs
static CFURLRef getURLForFile( const char * filename )
{
    // don't try to lookup the empty string because 
    // it will actually result in a valid file URL
    if ( strlen(filename) == 0 )
    {
        return NULL;
    }
    
	NSString * filePath = nil;
	CFURLRef fileURL = NULL;
	
	// check for a pull path first
	if ( filename[0] == '/' )
	{
		filePath = [NSString stringWithUTF8String:filename];
	}
	
	// not a full path, check the bundle.
	if ( filePath == nil )
	{	
		NSString * path    = [NSString stringWithUTF8String:filename];
		NSString * name    = [path stringByDeletingPathExtension];
		NSString * ext     = [path pathExtension];
		filePath  = [[NSBundle mainBundle] pathForResource:name ofType:ext];
	}
	
	// not in the bundle? look in the documents folder
	if ( filePath == nil )
	{
		NSString *rootPath = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
		if ( rootPath )
		{
			filePath = [rootPath stringByAppendingPathComponent:[NSString stringWithUTF8String:filename]];
		}
	}
	
	// if it exists, create the URL
	if ( filePath )
	{
		fileURL = CFURLCreateWithFileSystemPath(kCFAllocatorDefault, (CFStringRef)filePath, kCFURLPOSIXPathStyle, false);
	}
	else 
	{
		Minim::error("Unable to get the URL for a file!");
	}
	
	// caller is expected to release this.
	return fileURL;
}

#if TARGET_OS_IPHONE
TouchServiceProvider::TouchServiceProvider( AudioSessionParameters & rParameters )
{
	OSStatus status;
	
	status = AudioSessionInitialize( rParameters.interruptRunLoop, 
									 rParameters.interruptRunLoopMode, 
									 rParameters.interruptListener, 
									 rParameters.interruptUserData
									);
	
	if ( status )
	{
		displayErrorAndExit( @"Couldn't initialize an audio sesson.", status );
	}
	
	UInt32 category = rParameters.audioCategory;
	status = AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
	
	if ( status )
	{
		displayErrorAndExit(@"Couldn't set the AudioCategory to MediaPlayback.", status);
	}
	
	if ( rParameters.outputBufferDuration > 0.f )
	{
		Float32 preferredBufferSize = rParameters.outputBufferDuration;
		status = AudioSessionSetProperty( kAudioSessionProperty_PreferredHardwareIOBufferDuration, 
										 sizeof(preferredBufferSize), 
										 &preferredBufferSize
										 );
	
		if ( status )
		{
			displayErrorAndExit( @"Couldn't set the preferred hardware IO buffer duration.", status );
		}
	}
	
	Float32 checkBufferSize(0.f);
	UInt32	dataSize = sizeof(checkBufferSize);
	status = AudioSessionGetProperty( kAudioSessionProperty_PreferredHardwareIOBufferDuration,
									 &dataSize,
									 &checkBufferSize
									 );
	
	if ( status )
	{
		displayErrorAndExit( @"Couldn't get the preferred hardware IO buffer duration.", status );
	}
	
	rParameters.outputBufferDuration = checkBufferSize;
	
	if ( rParameters.setActiveImmediately )
	{
		status = AudioSessionSetActive(true);
	
		if ( status )
		{
			displayErrorAndExit(@"Couldn't set the audio session active!", status);
		}
	}
	
	// We really need to do the no-interp optimization on iPhone!
	Minim::Wavetable::s_opt = true;
}
#endif // TARGET_OS_IPHONE

////////////////////////////////////////////////////////////////
void TouchServiceProvider::stop()
{
#if TARGET_OS_IPHONE
	AudioSessionSetActive(false);
#endif
}

////////////////////////////////////////////////////////////////
Minim::AudioOut * TouchServiceProvider::getAudioOutput( const Minim::AudioFormat & outputFormat, int outputBufferSize )
{
	return new TouchAudioOut( outputFormat, outputBufferSize );
}

////////////////////////////////////////////////////////////////
Minim::AudioRecordingStream * TouchServiceProvider::getAudioRecordingStream( const char * filename, int bufferSize, bool bInMemory )
{
	// we disregard bufferSize and inMemory for now.
	CFURLRef fileURL = getURLForFile(filename);
	
	// only make something if we found it
	if ( fileURL )
	{
		Minim::AudioRecordingStream * pRecordingStream = new TouchAudioRecordingStream( fileURL, bufferSize );
		return pRecordingStream;
	}
	
	return NULL;
}

/////////////////////////////////////////////////////////////////
Minim::SampleRecorder * TouchServiceProvider::getSampleRecorder( Minim::AudioSource * sourceToRecord, const char * fileName, const bool buffered )
{
	NSString * pathToFile = nil;
	
	// first see if they've provided a full path
	if ( fileName[0] == '/' )
	{
		pathToFile = [NSString stringWithUTF8String:fileName];
	}
	else // otherwise, save to the documents directory
	{
		NSString * documentsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
		if ( documentsDirectory )
		{
			pathToFile = [documentsDirectory stringByAppendingPathComponent: [NSString stringWithUTF8String:fileName]];
		}
	}

	
	if ( pathToFile )
	{
		
		Minim::SampleRecorder * pRecorder = new CASampleRecorder( pathToFile, sourceToRecord->getFormat() );
		
		return pRecorder;
	}
	
	return NULL;
}
