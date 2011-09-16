/*
 *  WindowFunction.cpp
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "WindowFunction.h"


void Minim::WindowFunction::apply( float * samples, const int slength )
{
	length = slength;
	
	for (int n = 0; n < slength; n ++) 
	{
		samples[n] *= value(slength, n);
	}
}

void Minim::WindowFunction::apply(float * samples, const int offset, const int slength )
{
	length = slength;
	
	for(int n = offset; n < offset + slength; ++n)
	{
		samples[n] *= value(length, n - offset);
	}
}

float * Minim::WindowFunction::generateCurve( int length )
{
	float * samples = new float[length];
	for (int n = 0; n < length; n++) 
	{
		samples[n] = value(length, n);  
	}
	return samples;
}