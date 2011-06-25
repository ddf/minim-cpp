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

#ifndef SAMPLERECORDER_H
#define SAMPLERECORDER_H

#include "AudioListener.h"

namespace Minim
{
	class AudioRecordingStream;

	class SampleRecorder : public AudioListener
	{
	public:
		/**
		* Returns the full path to the file this is saving to, if it exists.
		* If this is not saving to a file, an empty <code>String</code> will be 
		* returned.
		*   
		* @return the full path to the file or an empty string
		*/
		virtual const char * filePath() const = 0;

		/**
		* Begins recording.
		*
		*/
		virtual void beginRecord() = 0;

		/**
		* Halts recording.
		*
		*/
		virtual void endRecord() = 0;

		/**
		* Returns the current record state.
		* 
		* @return true if this is recording
		*/
		virtual bool isRecording() const = 0;

		/**
		* Saves the recorded samples, probably to disk. 
		* Returns the recorded audio as an AudioRecordingStream.
		*
		*/
		virtual void save() = 0;
	};

};

#endif // SAMPLERECORDER_H