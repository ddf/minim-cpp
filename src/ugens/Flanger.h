//
//  Flanger.h
//  MinimTouch
//
//  Created by Damien Di Fede on 2/14/12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#ifndef MinimTouch_Flanger_h
#define MinimTouch_Flanger_h

#include "UGen.h"

namespace Minim 
{
    /**
     * A Flanger is a specialized kind of delay that uses an LFO (low frequency
     * oscillator) to vary the amount of delay applied to each sample. This causes a
     * sweeping frequency kind of sound as the signal reinforces or cancels itself
     * in various ways. In particular the peaks and notches created in the frequency
     * spectrum are related to each other in a linear harmonic series. This causes
     * the spectrum to look like a comb and should be apparent in the visualization.
     * <p>
     * Inputs for the Flanger are:
     * <ul>
     * <li>delay (in milliseconds): the minimum amount of delay applied to an incoming sample</li>
     * <li>rate (in Hz): the frequency of the LFO</li>
     * <li>depth (in milliseconds): the maximum amount of delay added onto delay by the LFO</li>
     * <li>feedback: how much of delayed signal should be fed back into the effect</li>
     * <li>dry: how much of the uneffected input should be included in the output</li>
     * <li>wet: how much of the effected signal should be included in the output</li>
     * </ul>
     * <p>
     * A more thorough description can be found on wikipedia:
     * http://en.wikipedia.org/wiki/Flanging
     * <p>
     * Author: Damien Di Fede
     */

    class Flanger : public UGen
    {
    public:
        /**
         * Where the input goes.
         */
        UGenInput	audio;
        
        /**
         * How much does the flanger delay the incoming signal. Used as the low
         * value of the modulated delay amount.
         */
        UGenInput	delay;
        
        /**
         * The frequency of the LFO applied to the delay.
         */
        UGenInput	rate;
        
        /**
         * How many milliseconds the LFO increases the delay by at the maximum.
         */
        UGenInput	depth;
        
        /**
         * How much of the flanged signal is fed back into the effect.
         */
        UGenInput	feedback;
        
        /**
         * How much of the dry signal is added to the output.
         */
        UGenInput	dry;
        
        /**
         * How much of the flanged signal is added to the output.
         */
        UGenInput	wet;
        
        /**
         * Construct a Flanger by specifying all initial values.
         * 
         * @param delayLength
         *            the minimum delay applied to incoming samples (in milliseconds)
         * @param lfoRate
         *            the frequency of the the LFO
         * @param delayDepth
         *            the maximum amount added to the delay by the LFO (in milliseconds)
         * @param feedbackAmplitude 
         * 			  the amount of the flanged signal fed back into the effect
         * @param dryAmplitude
         * 			  the amount of incoming signal added to the output
         * @param wetAmplitude
         * 			  the amount of the flanged signal added to the output
         */
        Flanger(float delayLength, float lfoRate, float delayDepth, float feedbackAmplitude, float dryAmplitude, float wetAmplitude);
        
        virtual ~Flanger();

		// zeroes the delay buffer, resets the write frame and restarts the lfo
		void reset();
        
    protected:
        virtual void sampleRateChanged();
        virtual void channelCountChanged();
        
        virtual void uGenerate( float* out, const int numChannels );
        
    private:
        
        void resetBuffer();
        
        // updates our step size based on the current frequency
        void updateStepSize();
        
        // clamps rate for us
        float getRate() const;
        
        float*		delayBuffer;
        int			writeFrame;
        int			bufferFrameLength;
        
        // ////////////
        // LFO
        // ////////////
        
        // where we will sample our waveform, moves between [0,1]
        float		step;
        // the step size we will use to advance our step
        float		stepSize;
        // what was our frequency from the last time we updated our step size
        // stashed so that we don't do more math than necessary
        float		prevFreq;
        // 1 / sampleRate, which is used to calculate stepSize
        float		oneOverSampleRate;
    };
    
}

#endif
