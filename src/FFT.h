/*
 *  FFT.h
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include "FourierTransform.h"

namespace Minim
{
	class FFT : public FourierTransform
	{
	public:
		/**
		 * Constructs an FFT that will accept sample buffers that are
		 * <code>timeSize</code> long and have been recorded with a sample rate of
		 * <code>sampleRate</code>. <code>timeSize</code> <em>must</em> be a
		 * power of two. This will throw an exception if it is not.
		 * 
		 * @param timeSize
		 *          the length of the sample buffers you will be analyzing
		 * @param sampleRate
		 *          the sample rate of the audio you will be analyzing
		 */
		FFT(int timeSize, float sampleRate);
		virtual ~FFT();
		
		inline void setSampleRate(float sr) { m_sampleRate = sr; m_bandWidth = (2.0f / m_timeSize) * (sr / 2.0f); }
		
		virtual void scaleBand(int i, float s);
		virtual void setBand(int i, float a);
		
		virtual void forward( float * buffer );
		
		/**
		 * Performs a forward transform on the passed buffers.
		 * 
		 * @param buffReal the real part of the time domain signal to transform
		 * @param buffImag the imaginary part of the time domain signal to transform
		 */
		virtual void forward( const float * buffReal, const float * buffImag );
		
		virtual void inverse( float * buffer );
		
	private:
		
		// performs an in-place fft on the data in the real and imag arrays
		// bit reversing is not necessary as the data will already be bit reversed
		void fft();
		
		// table used for bit-reversing arrays
		int * reverse;
		
		void buildReverseTable();
		
		// copies the values in the samples array into the real array
		// in bit reversed order. the imag array is filled with zeros.
		void bitReverseSamples(float * samples, int startAt);
		
		// bit reverse real[] and imag[]
		void bitReverseComplex();
		
		// lookup tables
		
		float * sinlookup;
		float * coslookup;
		
		inline float sinlu(int i)
		{
			return sinlookup[i];
		}
		
		inline float coslu(int i)
		{
			return coslookup[i];
		}
		
		void buildTrigTables();

		// arrays used for bit-reversing our complex data.
		float *	m_reverseReal;
		float * m_reverseImag;
	};
};
