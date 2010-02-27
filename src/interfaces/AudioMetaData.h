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


#ifndef AUDIOMETADATA_H
#define AUDIOMETADATA_H

namespace Minim
{

/**
 * AudioMetaData provides information commonly found in ID3 tags. 
 * However, other audio formats, such as Ogg, can contain
 * similar information. So rather than refer to this information
 * as ID3Tags or similar, I simply call it metadata. This base 
 * class returns the empty string or -1 from all methods and
 * derived classes are expected to simply override the methods
 * that they have information for. This is a little less brittle
 * than using an interface because later on new properties can 
 * be added without breaking existing code.
 */

	class AudioMetaData
	{
	public:
		/**
		 * @return the length of the recording in milliseconds.
		 */
		virtual int length() const { return -1; }
		
		/**
		 * @return the name of the file / URL of the recording.
		 */
		virtual const char * fileName() const { return ""; }
		
		/**
		 * @return the title of the recording
		 */
		virtual const char * title() const { return ""; }
		
		/**
		 * @return the author or the recording
		 */
		virtual const char * author() const { return ""; }
		
		virtual const char * album() const { return ""; }
		
		virtual const char * date() const {	return ""; }
		
		virtual const char * comment() const { return ""; }
		
		virtual int track() const { return -1; }
		
		virtual const char * genre() const { return ""; }
		
		virtual const char * copyright() const { return ""; }
		
		virtual const char * disc() const {	return ""; }
		
		virtual const char * composer() const {	return ""; }
		
		virtual const char * orchestra() const { return ""; }
		
		virtual const char * publisher() const { return "";	}
		
		virtual const char * encoded() const { return ""; }

	protected:
		AudioMetaData() {}
		virtual ~AudioMetaData() {}
		AudioMetaData( const AudioMetaData & ) {}
		// TODO: operator= don't remember exactly what this looks like, don't want to look it up right now.
	};

};

#endif // AUDIOMETADATA_H