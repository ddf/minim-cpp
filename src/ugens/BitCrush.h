//
//  BitCrush.h
//  MinimTouch
//
//  Created by Damien Di Fede on 5/19/12.
//

#ifndef Minim_BitCrush_h
#define Minim_BitCrush_h

#include "UGen.h"

namespace Minim 
{

    class BitCrush : public UGen
    {
    public:
        
        BitCrush( float bitResolution, float bitRate );
        
        /**
         * The audio input is where audio that gets bit-crushed should be patched. 
         * However, you don't need to patch directly to this input, patching to
         * the UGen itself will accomplish the same thing.
         */
        UGenInput audio;
        
        /**
         * Control the bit resolution with another UGen by patching to bitRes. Values that 
         * make sense for this start at 1 and go up to whatever the actual resolution of 
         * the incoming audio is (typically 32).
         */
        UGenInput bitRes;
        
        /**
         * Control the bit rate with another UGen by patch to bitRate.
         * Values that make sense for this start at 1 and go up to whatever the
         * sample rate of your AudioOutput are (typically 44100)
         */
        UGenInput bitRate;
        
    protected:
        
        virtual void uGenerate( float* out, const int numChannels );
        
        virtual void channelCountChanged();
        
    private:
        
        float* sampledFrame;
        int    sampleCounter;
    };
    
};

#endif // Minim_BitCrush_h

