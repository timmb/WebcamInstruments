//
//  SimpleThresholdTrigger.h
//  WebcamInstruments
//
//  Created by Tim Murray-Browne on 20/11/2012.
//
//

#ifndef __WebcamInstruments__SimpleThresholdTrigger__
#define __WebcamInstruments__SimpleThresholdTrigger__

#include <iostream>
#include "ofxMidiOut.h"
#include "ofxCv.h"

// Class to represent an area on the screen that operates as a threshold-based trigger


class SimpleThresholdTrigger
{
public:
	/// midiOutput is a pointer to the MIDI sender object defined elsewhere
	/// pitch is a MIDI note number
	/// location is the position within the frame where the trigger is
	/// activated and drawn
	SimpleThresholdTrigger(ofxMidiOut* midiOutput, int pitch, cv::Rect const& location);
	// Destructor: this is run when this object is deleted
	virtual ~SimpleThresholdTrigger();
	
	/// differenceImage should be the current frame with the previous
	/// frame subtracted giving higher values for parts of the frame
	/// that are different
	void update(cv::Mat const& differenceImage);
	/// End any currently playing notes.
	void flush();
	/// Draw some user feedback for this trigger
	void draw();
	
	/// How much brightness in frames we need to detect to
	/// activate the trigger.
	float mThreshold; // value is between 0 and 1.
	
private:
	/// The location of this trigger (within the camera image frame)
	cv::Rect mLocation;
	/// How much movement was detected from the last frame data
	float mPreviousMovement;
	/// A pointer to the object that sends MIDI data
	ofxMidiOut* mMidiOutput;
	/// The pitch to send MIDI data at (this is a MIDI note number)
	int mMidiPitch;
	/// Use this to remember whether we are in the middle of
	/// playing a note.
	bool mNoteIsPlaying;
};


#endif /* defined(__WebcamInstruments__SimpleThresholdTrigger__) */
