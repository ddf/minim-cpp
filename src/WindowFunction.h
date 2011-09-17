/*
 *  WindowFunction.h
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef WINDOWFUNCTION_H
#define WINDOWFUNCTION_H


#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES // required in windows to get constants like M_PI
#endif

#include <math.h>

#ifndef TWO_PI
#define TWO_PI 6.28318531f
#endif

namespace Minim 
{

	class WindowFunction
	{
	protected:
		
		int length;
		
	public:
		
		WindowFunction() 
		: length(0)
		{}
		
		virtual ~WindowFunction() {}
		
		/** 
		 * Apply the window function to a sample buffer.
		 * 
		 * @param samples a sample buffer
		 */
		void apply( float * samples, const int length );
		
		/**
		 * Apply the window to a portion of this sample buffer,
		 * given an offset from the beginning of the buffer 
		 * and the number of samples to be windowed.
		 */
		void apply(float * samples, const int offset, const int length);
		
		/** 
		 * Generates the curve of the window function.
		 * 
		 * @param length the length of the window
		 * @return the shape of the window function
		 */
		float * generateCurve( int length );

		
		virtual float value(int length, int index) = 0;
	};
	
	class RectangularWindow : public WindowFunction
	{
		virtual float value(int length, int index) { return 1.0f; }
	};
	
	class HammingWindow : public WindowFunction
	{
		virtual float value(int length, int index) 
		{
			return 0.54f - 0.46f * cosf(TWO_PI * index / (length - 1));
		}
	};
	
}

#endif // WINDOWFUNCTION_H