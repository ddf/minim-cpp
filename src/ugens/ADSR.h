//
//  ADSR.h
//  MinimTouch
//
//  Created by Damien Di Fede on 12/9/11.
//  Copyright (c) 2011 Damien Di Fede. All rights reserved.
//

#ifndef _ADSR_h
#define _ADSR_h

#include "UGen.h"

namespace Minim 
{
    class AudioOutput;
    
    class ADSR : public UGen
    {
    public: // inputs
        
        UGenInput audio;
        
    public:
        
        explicit ADSR( float maxAmp = 1.0f, float attTime = 1.0f, float decTime = 1.0f, float susLvl = 0.0f, float relTime = 1.0f, float befAmp = 0.0f, float aftAmp = 0.0f );
        
        // set all the parameters for the envelope
        void setParameters( float maxAmp, float attTime, float decTime, float susLvl, float relTime, float befAmp, float aftAmp );
        
        // begin the attack-decay-sustain
        void noteOn();
        // begin the release
        void noteOff();
        
        void unpatchAfterRelease( AudioOutput* from );
        void unpatchAfterRelease( UGen* from );
        
    protected:
        
        virtual void sampleRateChanged()
        {
            timeStepSize = 1.0f / sampleRate();
        }
        
        virtual void uGenerate( float* channels, int numChannels );
        
    private:
        
        // envelope state
        float       beforeAmplitude;
        float       afterAmplitude;
        float       maxAmplitude;
        float       amplitude;
        float       attackTime;
        float       decayTime;
        float       sustainLevel;
        float       releaseTime;
        float       timeStepSize;
        float       timeFromOn;
        float       timeFromOff;
        
        // on or off?
        bool        isTurnedOn;
        bool        isTurnedOff;
        
        // keep track of what we should unpatch from
        bool         bUnpatchAfterRelease;
        AudioOutput* outputToUnpatch;
        UGen*        ugenOutputToUnpatch;
        
    };
}

#endif
