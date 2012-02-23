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

#ifndef UGEN_H
#define UGEN_H

#ifndef NULL
#define NULL 0
#endif

namespace Minim
{

	class AudioOutput;
/**
 * The UGen class is an abstract class which is intended to be the basis for
 * all UGens in Minim.  
 * @author Damien Di Fede, Anderson Mills
 */
class UGen
{
protected:
	/**
	 * This enum is used to specify the InputType of the UGenInput
	 * @author Anderson Mills
	 */
	// jam3: enum is automatically static so it can't be in the nested class
	enum InputType { CONTROL, AUDIO };
	
	/**
	 * This inner class, UGenInput, is used to connect the output of other UGens to this UGen
	 * @author Anderson Mills
	 */
	class UGenInput
	{
	public:
		UGenInput(UGen & outer, InputType type, float defaultValue = 0.0f);
		~UGenInput();
		
		inline InputType getInputType() const { return mInputType; }
		const char * getInputTypeAsString() const;
		
		inline bool isPatched() const { return mIncoming != 0; }
        
        // returns next input in the list, if there is one
		inline UGenInput* tick() { if ( mIncoming ) mIncoming->tick(mLastValues, mChannelCount); return mNextInput; }
        
        // for when we just need to iterate
        inline UGenInput* next() { return mNextInput; }
		
		inline void setSampleRate( float sampleRate ) { mIncoming->setSampleRate(sampleRate); }
		
        void setIncomingUGen( UGen * inUGen );
		
		inline UGen * getIncomingUGen() const { return mIncoming; }
		
		inline UGen & getOuterUGen() const { return mOuterUGen; }

		// channel info
		inline int getChannelCount() const { return mChannelCount; }
		void setChannelCount( const int numChannels ); // will set size of last values array, if necessary.
		
		// last values, use getChannelCount() as the array length.
		inline const float * getLastValues() const { return mLastValues; }
		inline float * getLastValues() { return mLastValues; }
		// shortcut to the first value in the last values array. used alot to get the value of CONTROL inputs.
		inline float getLastValue() const { return mLastValues[0]; }
		// typically used to set the value of a CONTROL input directly if nothing is patched to it.
		inline void setLastValue( const float val ) { mLastValues[0] = val; }
	
	private:
		
		UGen & mOuterUGen;
		UGen * mIncoming;
		InputType mInputType;
		int mChannelCount;
		float * mLastValues; // set based on channel count.
        UGenInput* mNextInput; // who is the next input in the list?
	};  // ends the UGenInput inner class
	
public:
	/**
	 * Constructor for a UGen.  
	 */
	explicit UGen();
	virtual ~UGen();
	
	/**
	 * Connect the output of this UGen to the first input of connectToUGen. Doing so will chain these 
	 * two UGens together, causing them to generate sound at the same time when the end of 
	 * chain is patched to an AudioOutput.
	 * 
	 * @param connectToUGen The UGen to connect to.
	 * @return connectToUGen is returned so that you can chain patch calls. For example: 
	 * <pre>
	 * sine.patch(gain).patch(out);
	 * </pre>
	 */
	// ddf: this is final because we never want people to override it.
	UGen & patch( UGen & connectToUGen );

	/**
	 * Connect the output of this UGen to a specific input of connecToUGen.
	 * @param connectToInput
	 * @return cennectToInput.getOuterUGen()
	 */
	UGen & patch(UGenInput & connectToInput);

	/**
	 * Patch the output of this UGen to the provided AudioOuput. Doing so will immediately 
	 * result in this UGen and all UGens patched into it to begin generating audio.
	 * 
	 * @param output The AudioOutput you want to connect this UGen to.
	 */
	void patch( AudioOutput & output );

	/**
	 * Unpatch the output of this output from the provided AudioOutput. This causes this UGen
	 * and all UGens patched into it to stop generation audio if AudioOutput is not potched 
	 * somewhere else in this chain.
	 * 
	 * @param output The AudioOutput this UGen should be disconnected from.
	 */
	void unpatch( AudioOutput & output );

	/**
	 * Remove this UGen as the input to the connectToUGen.
	 * @param connectToUGen
	 */
	void unpatch( UGen & disconnectFrom );
	
protected:
	// ddf: Protected because users of UGens should never call this directly.
	//      Sub-classes can override this to control what happens when something
	//      is patched to them. See the Summer class.
	virtual void addInput( UGen * input );
	

	// This currently does nothing, but is overridden in Summer.
	virtual void removeInput( UGen * input );

public:
	
	/**
	 * Generates one sample frame for this UGen. 
	 * 
	 * @param channels
	 *    An array that represents one sample frame. To generate a mono signal, 
	 *    pass an array of length 1, if stereo an array of length 2, and so on.
	 *    How a UGen deals with multi-channel sound will be implementation dependent.
	 */
	void tick(float * channels, const int numChannels);

protected:
	
	/**
	 * Implement this method when you extend UGen.
	 * @param channels
	 */
	virtual void uGenerate(float * channels, const int numChannels) = 0;
	
public:
	
	// Just return the lastValues
	inline const float * getLastValues() const
	{
		return mLastValues;
	}

protected:
	
	/**
	 * Override this method in your derived class to receive a notification
	 * when the sample rate of your UGen has changed. You might need to do 
	 * this to recalculate sample rate dependent values, such as the 
	 * step size for an oscillator.
	 *
	 */
	virtual void sampleRateChanged()
	{
		// default implementation does nothing.
	}
	
	/**
	 * Override this method in your derived class to receive notification
	 * when the channel count of your UGen has changed.
	 */
	virtual void channelCountChanged()
	{
		// default implementation does nothing.
	}
	
	inline float sampleRate() const { return mSampleRate; }
	
	/**
	 * Helper function for sub-classes that want to set 
	 * all channels of a sample frame to the same value.
	 */
	static void fill( float * sampleFrame, const float value, const int numChannels );
    
    /**
     * Helper function for sub-classes that want to accumulate.
     */
    static void accum( float * accumFrame, const float * sampleFrame, const int numChannels, const float scale = 1.0f );

public:
	
	/**
	 * Set the sample rate for this UGen.
	 * 
	 * @param newSampleRate the sample rate this UGen should generate at.
	 */
	// ddf: changed this to public because Summer needs to be able to call it
	//      on all of its UGens when it has its sample rate set by being connected 
	//      to an AudioOuput. Realized it's not actually a big deal for people to 
	//      set the sample rate of any UGen they create whenever they want. In fact, 
	//      could actually make total sense to want to do this with something playing 
	//      back a chunk of audio loaded from disk. Made this final because it should 
	//      never be overridden. If sub-classes need to know about sample rate changes 
	//      the should override sampleRateChanged()
	void setSampleRate(float newSampleRate);
	
	/**
	 * Let this UGen know how many channels of audio you will be asking it for. This will be called 
	 * automatically when a UGen is patched to an AudioOuput and propagated to all UGenInputs of 
	 * type AUDIO.
	 * 
	 * @param numberOfChannels how many channels of audio you will be generating with this UGen
	 */
	void setAudioChannelCount( int numberOfChannels );
	
	inline int getAudioChannelCount() const { return mChannelCount; }
	
	/**
	 *  Prints all inputs connected to this UGen (for debugging)
	 */
	void printInputs() const;
    
    bool isPatched() const { return mNumOutputs > 0; }

private:
	// all of this UGen's inputs, accessible in linked-list fashion
	UGenInput *     mInputs;
    // how many last values were generated most recently
	int             mChannelCount;
    // last values generated by this UGen
	float *         mLastValues;
	// sampleRate of this UGen
	float           mSampleRate;
	// number of outputs connected to this UGen
	int             mNumOutputs;
	// counter for the currentTick with respect to the number of Outputs
	int             mCurrentTick;
};

};

#endif // UGEN_H