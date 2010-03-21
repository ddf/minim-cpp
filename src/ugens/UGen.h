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
	// TODO make this package-protected if IIRFilter can be brougt into the ugens package
	class UGenInput
	{
	public:
	    /**
	     *  This constructor generates a UGenInput of the specified type.
	     * @param it
	     */
	    explicit UGenInput( UGen & outerUGen, InputType inputType = AUDIO );

	    /**
	     *  returns the inputType
	     * @return
	     */
		InputType getInputType() const { return mInputType; }

	    /**
	     *  returns the outer UGen of which this is an input.
	     * @return
	     */
		UGen & getOuterUGen() {	return mOuterUGen; }
		const UGen & getOuterUGen() const { return mOuterUGen; }
	    
	    /**
	     *  returns the UGen which is giving values to this input.
	     * @return
	     */
		UGen & getIncomingUGen() { return *mIncoming; }
		const UGen & getIncomingUGen() const { return *mIncoming; }

	    /**
	     *  set the UGen which is giving values to this input
	     * @param in
	     */
		void setIncomingUGen( UGen * in ) { mIncoming = in; }

	    /**
	     *  returns true if this input has an incoming UGen
	     * @return
	     */
	    bool isPatched() const { return (mIncoming); }

	    /**
	     *  returns the last values provided to this input from it's incoming UGen
	     * @return
	     */
		const float * getLastValues() const
		{
			return getIncomingUGen().getLastValues();
		}

	    /**
	     *  return the inputType as a string (for debugging)
	     * @return
	     */
	    const char * getInputTypeAsString() const;

	    /**
	     *  print information about this UGenInput (for debugging)
	     */
	    void printInput() const;

	private:
		UGen & mOuterUGen;
		UGen * mIncoming;
		InputType mInputType;
	};  // ends the UGenInput inner class
	
public:
	/**
	 * Constructor for a UGen.  
	 */
	explicit UGen();
	explicit UGen( const int numOfInputs );
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
	const float * getLastValues() const
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
	
	float sampleRate() const { return mSampleRate; }

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
	 *  Prints all inputs connected to this UGen (for debugging)
	 */
	void printInputs() const;

private:
	// all of this UGen's inputs
	UGenInput **mInputs;
	// how many inputs we have
	int			mInputCount;
	
    // last values generated by this UGen
	float * mLastValues;
	// how many last values were generated most recently
	int		mLastValuesLength;
	// sampleRate of this UGen
	float mSampleRate;
	// number of outputs connected to this UGen
	int mNumOutputs;
	// counter for the currentTick with respect to the number of Outputs
	int mCurrentTick;
};

};

#endif // UGEN_H