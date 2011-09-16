/*
 *  FourierTransform.h
 *  MinimOSX
 *
 *  Created by Damien Di Fede on 9/14/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#include <math.h>
#include "WindowFunction.h"

#ifndef TWO_PI
#define TWO_PI (2*PI)
#endif

namespace Minim 
{

	class FourierTransform
	{
	public:
		/** A constant indicating no window should be used on sample buffers. Also referred as a <a href="http://en.wikipedia.org/wiki/Window_function#Rectangular_window">Rectangular window</a>. */
		static RectangularWindow NONE;
		/** A constant indicating a <a href="http://en.wikipedia.org/wiki/Window_function#Hamming_window">Hamming window</a> should be used on sample buffers. */
		static HammingWindow HAMMING;
		
	protected:
		
		static const int LINAVG = 1;
		static const int LOGAVG = 2;
		static const int NOAVG = 3;
		
		int					m_timeSize;
		int					m_spectrumSize;
		int					m_sampleRate;
		float				m_bandWidth;
		float *				m_real;
		float *				m_imag;
		float *				m_spectrum;
		float *				m_averages;
		int					m_averagesSize;
		int					m_whichAverage;
		int					m_octaves;
		int					m_avgPerOctave;
		WindowFunction *	m_windowFunction;
		
		/**
		 * Construct a FourierTransform that will analyze sample buffers that are
		 * <code>ts</code> samples long and contain samples with a <code>sr</code>
		 * sample rate.
		 * 
		 * @param ts
		 *          the length of the buffers that will be analyzed
		 * @param sr
		 *          the sample rate of the samples that will be analyzed
		 */
		FourierTransform(int ts, float sr);
		virtual ~FourierTransform();
		
		void setComplex( const float * r, const float * i);
		
		// fill the spectrum array with the amps of the data in real and imag
		// used so that this class can handle creating the average array
		// and also do spectrum shaping if necessary
		void fillSpectrum();
		
	public:
		
		/**
		 * Sets the object to not compute averages.
		 * 
		 */
		inline void noAverages()
		{
			if ( m_averages )
			{
				delete [] m_averages;
				m_averages = 0;
			}
			
			m_whichAverage = NOAVG;
		}
		
		/**
		 * Sets the number of averages used when computing the spectrum and spaces the
		 * averages in a linear manner. In other words, each average band will be
		 * <code>specSize() / numAvg</code> bands wide.
		 * 
		 * @param numAvg
		 *          how many averages to compute
		 */
		void linAverages(int numAvg);
		
		/**
		 * Sets the number of averages used when computing the spectrum based on the
		 * minimum bandwidth for an octave and the number of bands per octave. For
		 * example, with audio that has a sample rate of 44100 Hz,
		 * <code>logAverages(11, 1)</code> will result in 12 averages, each
		 * corresponding to an octave, the first spanning 0 to 11 Hz. To ensure that
		 * each octave band is a full octave, the number of octaves is computed by
		 * dividing the Nyquist frequency by two, and then the result of that by two,
		 * and so on. This means that the actual bandwidth of the lowest octave may
		 * not be exactly the value specified.
		 * 
		 * @param minBandwidth
		 *          the minimum bandwidth used for an octave
		 * @param bandsPerOctave
		 *          how many bands to split each octave into
		 */
		void logAverages(int minBandwidth, int bandsPerOctave);
		
		/**
		 * Sets the window to use on the samples before taking the forward transform.
		 * If an invalid window is asked for, an error will be reported and the
		 * current window will not be changed.
		 * 
		 * @param windowFunction
		 */
		
		inline void window( WindowFunction & windowFunc )
		{
			m_windowFunction = &windowFunc;
		}
		
	protected:
		
		inline void doWindow( float * samples, const int length )
		{
			m_windowFunction->apply( samples, length );
		}
		
	public:
		
		/**
		 * Returns the length of the time domain signal expected by this transform.
		 * 
		 * @return the length of the time domain signal expected by this transform
		 */
		inline int timeSize() const
		{
			return m_timeSize;
		}
		
		/**
		 * Returns the size of the spectrum created by this transform. In other words,
		 * the number of frequency bands produced by this transform. This is typically
		 * equal to <code>timeSize()/2 + 1</code>, see above for an explanation.
		 * 
		 * @return the size of the spectrum
		 */
		inline int specSize() const
		{
			return m_spectrumSize;
		}
		
		/**
		 * Returns the amplitude of the requested frequency band.
		 * 
		 * @param i
		 *          the index of a frequency band
		 * @return the amplitude of the requested frequency band
		 */
		inline float getBand( int i ) const
		{
			if (i < 0) i = 0;
			if (i > m_spectrumSize - 1) i = m_spectrumSize - 1;
			return m_spectrum[i];
		}
		
		/**
		 * Returns the width of each frequency band in the spectrum (in Hz). It should
		 * be noted that the bandwidth of the first and last frequency bands is half
		 * as large as the value returned by this function.
		 * 
		 * @return the width of each frequency band in Hz.
		 */
		inline float getBandWidth() const
		{
			return m_bandWidth;
		}
		
		/**
		 * Sets the amplitude of the <code>i<sup>th</sup></code> frequency band to
		 * <code>a</code>. You can use this to shape the spectrum before using
		 * <code>inverse()</code>.
		 * 
		 * @param i
		 *          the frequency band to modify
		 * @param a
		 *          the new amplitude
		 */
		virtual void setBand(int i, float a) = 0;
		
		/**
		 * Scales the amplitude of the <code>i<sup>th</sup></code> frequency band
		 * by <code>s</code>. You can use this to shape the spectrum before using
		 * <code>inverse()</code>.
		 * 
		 * @param i
		 *          the frequency band to modify
		 * @param s
		 *          the scaling factor
		 */
		virtual void scaleBand(int i, float s) = 0;
		
		/**
		 * Returns the index of the frequency band that contains the requested
		 * frequency.
		 * 
		 * @param freq
		 *          the frequency you want the index for (in Hz)
		 * @return the index of the frequency band that contains freq
		 */
		int freqToIndex(float freq) const;
		
		/**
		 * Returns the middle frequency of the i<sup>th</sup> band.
		 * @param i
		 *        the index of the band you want to middle frequency of
		 */
		float indexToFreq(int i) const;
		
		/**
		 * Returns the center frequency of the i<sup>th</sup> average band.
		 * 
		 * @param i
		 *     which average band you want the center frequency of.
		 */
		float getAverageCenterFrequency(int i) const;
		
		
		/**
		 * Gets the amplitude of the requested frequency in the spectrum.
		 * 
		 * @param freq
		 *          the frequency in Hz
		 * @return the amplitude of the frequency in the spectrum
		 */
		inline float getFreq(float freq) const
		{
			return getBand(freqToIndex(freq));
		}
		
		/**
		 * Sets the amplitude of the requested frequency in the spectrum to
		 * <code>a</code>.
		 * 
		 * @param freq
		 *          the frequency in Hz
		 * @param a
		 *          the new amplitude
		 */
		inline void setFreq(float freq, float a)
		{
			setBand(freqToIndex(freq), a);
		}
		
		/**
		 * Scales the amplitude of the requested frequency by <code>a</code>.
		 * 
		 * @param freq
		 *          the frequency in Hz
		 * @param s
		 *          the scaling factor
		 */
		inline void scaleFreq(float freq, float s)
		{
			scaleBand(freqToIndex(freq), s);
		}
		
		/**
		 * Returns the number of averages currently being calculated.
		 * 
		 * @return the length of the averages array
		 */
		inline int avgSize() const
		{
			return m_averagesSize;
		}
		
		/**
		 * Gets the value of the <code>i<sup>th</sup></code> average.
		 * 
		 * @param i
		 *          the average you want the value of
		 * @return the value of the requested average band
		 */
		inline float getAvg(int i) const
		{
			if (m_averagesSize > 0)
			{
				return m_averages[i];
			}
			return 0;
		}
		
		/**
		 * Calculate the average amplitude of the frequency band bounded by
		 * <code>lowFreq</code> and <code>hiFreq</code>, inclusive.
		 * 
		 * @param lowFreq
		 *          the lower bound of the band
		 * @param hiFreq
		 *          the upper bound of the band
		 * @return the average of all spectrum values within the bounds
		 */
		inline float calcAvg(float lowFreq, float hiFreq) const
		{
			int lowBound = freqToIndex(lowFreq);
			int hiBound = freqToIndex(hiFreq);
			float avg = 0;
			for (int i = lowBound; i <= hiBound; i++)
			{
				avg += m_spectrum[i];
			}
			avg /= (hiBound - lowBound + 1);
			return avg;
		}
		
		/**
		 * Get the Real part of the Complex representation of the spectrum.
		 */
		float * getSpectrumReal()
		{
			return m_real;
		}
		
		/**
		 * Get the Imaginary part of the Complex representation of the spectrum.
		 */
		float * getSpectrumImaginary()
		{
			return m_imag;
		}
		
		
		/**
		 * Performs a forward transform on <code>buffer</code>.
		 * 
		 * @param buffer
		 *          the buffer to analyze
		 */
		virtual void forward( float * buffer) = 0;
		
		/**
		 * Performs a forward transform on values in <code>buffer</code>.
		 * 
		 * @param buffer
		 *          the buffer of samples
		 * @param startAt
		 *          the index to start at in the buffer. there must be at least timeSize() samples
		 *          between the starting index and the end of the buffer. If there aren't, an
		 *          error will be issued and the operation will not be performed.
		 *          
		 */
		inline void forward( float * buffer, int startAt)
		{
			forward(buffer+startAt);
		}
		
		/**
		 * Performs a forward transform on <code>buffer</code>.
		 * 
		 * @param buffer
		 *          the buffer to analyze
		 */
//		inline void forward(const AudioBuffer & buffer)
//		{
//			forward(buffer.toArray());
//		}
		
		/**
		 * Performs a forward transform on <code>buffer</code>.
		 * 
		 * @param buffer
		 *          the buffer of samples
		 * @param startAt
		 *          the index to start at in the buffer. there must be at least timeSize() samples
		 *          between the starting index and the end of the buffer.
		 */
//		inline void forward(const AudioBuffer & buffer, int startAt)
//		{
//			forward(buffer.toArray(), startAt);
//		}
		
		/**
		 * Performs an inverse transform of the frequency spectrum and places the
		 * result in <code>buffer</code>.
		 * 
		 * @param buffer
		 *          the buffer to place the result of the inverse transform in
		 */
		virtual void inverse(float * buffer) = 0;
		
		/**
		 * Performs an inverse transform of the frequency spectrum and places the
		 * result in <code>buffer</code>.
		 * 
		 * @param buffer
		 *          the buffer to place the result of the inverse transform in
		 */
//		inline void inverse(const AudioBuffer & buffer)
//		{
//			inverse(buffer.toArray());
//		}
		
		/**
		 * Performs an inverse transform of the frequency spectrum represented by
		 * freqReal and freqImag and places the result in buffer.
		 * 
		 * @param freqReal
		 *          the real part of the frequency spectrum
		 * @param freqImag
		 *          the imaginary part the frequency spectrum
		 * @param buffer
		 *          the buffer to place the inverse transform in
		 */
		inline void inverse(const float * freqReal, const float * freqImag, float * buffer)
		{
			setComplex(freqReal, freqImag);
			inverse(buffer);
		}
	};
};