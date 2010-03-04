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

#include "Summer.h"
#include "..\AudioOutput.h"
#include <algorithm>

namespace Minim
{

Summer::Summer()
: mOutput(NULL)
{
}

Summer::Summer(AudioOutput * out)
: mOutput(out)
{
	
}

///////////////////////////////////////////////////
void Summer::uGenerate(float * channels, int numChannels)
{
	// if we were constructed with an output
	// we need to tick that output's noteManager!
	if ( mOutput != NULL )
	{
		// TODO
		// out->noteManager.tick();
	}

	// TODO: this is really the road to fragmentation city
	//       so is probably a better idea to reuse an array
	//       on the class. I'd declare this on the stack
	//       but then I'd be hardcoding the max number of 
	//       channels a Summer will ever handle.
	float * tmp = new float[numChannels];
	std::vector<UGen*>::iterator itr = mUGens.begin();
	for(; itr != mUGens.end(); ++itr)
	{
		memset(tmp, 0, sizeof(float) * numChannels);
		UGen * u = *itr;
		u->tick( tmp, numChannels );
		for(int c = 0; c < numChannels; c++)
		{
			channels[c] += tmp[c];
		}
	}
	delete [] tmp;

	// now remove anything that's marked itself for removal
	for(itr = mToRemove.begin(); itr != mToRemove.end(); ++itr)
	{
		mUGens.erase( std::find(mUGens.begin(), mUGens.end(), *itr) );
	}
}

} // namespace Minim