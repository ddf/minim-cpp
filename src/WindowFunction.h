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
		float * generateCurve( int length ) const;

		
		virtual float value(int length, int index) const = 0;
	};
	
	class RectangularWindow : public WindowFunction
	{
		float value(int length, int index) const override { return 1.0f; }
	};
	
	class TriangularWindow : public WindowFunction
	{
		float value(int length, int index) const override
		{
			return 2.f / length * (length / 2.f - fabs(index - (length - 1) / 2.f));
		}
	};
	
	class HammingWindow : public WindowFunction
	{
		float value(int length, int index) const override
		{
			return 0.54f - 0.46f * cosf(TWO_PI * index / (length - 1));
		}
	};

	class HannWindow : public WindowFunction
	{
		float value(int length, int index) const override
		{
			return 0.5f * (1.f - (float)cosf(TWO_PI * index / (length - 1.f)));
		}
	};

	class BartlettHannWindow : public WindowFunction
	{
		float value(int length, int index) const override
		{
			return (float)(0.62f - 0.48f * fabs((float)index / (length - 1) - 0.5f) - 0.38f * cos(TWO_PI * index / (length - 1)));
		}
	};
	
}

#endif // WINDOWFUNCTION_H
