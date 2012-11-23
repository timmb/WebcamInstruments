//
//  SimpleThresholdTrigger.cpp
//  WebcamInstruments
//
//  Created by Tim Murray-Browne on 20/11/2012.
//
//

#include "SimpleThresholdTrigger.h"

SimpleThresholdTrigger::SimpleThresholdTrigger(ofxMidiOut* midiOutput, int pitch, cv::Rect const& location)
{
	mMidiOutput = midiOutput;
	mMidiPitch = pitch;
	mThreshold = 0.2;
	mPreviousMovement = 0.;
	mNoteIsPlaying = false;
	mLocation = location;
}

SimpleThresholdTrigger::~SimpleThresholdTrigger()
{
	flush();
}

void SimpleThresholdTrigger::flush()
{
	if (mNoteIsPlaying)
	{
		// send note off by sending not with zero velocity.
		mMidiOutput->sendNoteOn(0, mMidiPitch, 0);
	}
}


void SimpleThresholdTrigger::update(cv::Mat const& differenceImage)
{
	// First we set a 'region-of-interest' within the input image
	cv::Mat region = differenceImage(mLocation);
	
	// take an average over the data. Note that CV is using pixel values that are integers
	// between 0 and 255 but we convert this to a float between 0. and 1.
	// as it makes the maths simpler.
	cv::Scalar meanPerChannel = cv::mean(region); // This gives us the average in (R,G,B) individually
	float overallMean = (meanPerChannel[0] + meanPerChannel[1] + meanPerChannel[2])/3.;
	float movement = overallMean / 255; // convert range from 0-255 to 0.-1.
	// A little bit of fiddling to exaggerate the value of movement
	movement = 1. - pow((1. - movement),3);
	float amountAboveThreshold = movement - mThreshold;
	float amountAboveThresholdPreviously = mPreviousMovement - mThreshold;
	// We normalise these values so that positive values lie between 0. and 1.
	float rangeAvailableAboveThreshold = 1. - mThreshold;
	movement = movement / rangeAvailableAboveThreshold;
	amountAboveThreshold = amountAboveThreshold / rangeAvailableAboveThreshold;
	// If the average has just gone above the threshold then we
	// trigger a note. Once we go below the threshold then we
	// stop the note.
	if (amountAboveThreshold > 0 && amountAboveThresholdPreviously < 0)
	{
		// To determine the velocity of the note we consider the range
		// of possible values that could be above the threshold
		float velocityRange = 1. - mThreshold;
		// then work out how far through that range our note is
		float velocity = amountAboveThreshold / velocityRange;
		// our values range between 0 and 1, but MIDI uses 0-127 integers
		int velocityInt = (int) (velocity * 127 + .5);
		// Just in case we've gone outside of MIDI's range:
		velocityInt = max(0, velocityInt);
		velocityInt = min(127, velocityInt);
		mMidiOutput->sendNoteOn(0, mMidiPitch, velocityInt);
		mNoteIsPlaying = true;
	}
	else if (amountAboveThreshold < 0 && mNoteIsPlaying)
	{
		// Stop a note by sending velocity as 0
		mMidiOutput->sendNoteOn(0, mMidiPitch, 0);
		mNoteIsPlaying = false;
	}
	// Remember the current movement for the next frame
	mPreviousMovement = movement;
}

void SimpleThresholdTrigger::draw()
{
	ofVec2f center = ofVec2f(mLocation.x + mLocation.width / 2.,
							 mLocation.y + mLocation.height / 2.);
	// Draw the threshold as a light grey cricle
	ofSetColor(180, 180, 180, 64);
	float locationRadius = min(mLocation.width, mLocation.height) / 2.;
	ofCircle(center, mThreshold * locationRadius);
	// Then draw the movement amount as a colour circle growing out of it
	int red = (int)(mPreviousMovement * 224.);
	int green = (int)(mPreviousMovement * 100. + 30);
	int blue = (int)(mPreviousMovement * 243.);
	ofSetColor(red, green, blue, 255);
	ofCircle(center, mPreviousMovement * locationRadius);
}
