#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "ofxMidi.h"

#include "SimpleThresholdTrigger.h"
#include "ScaleMapper.h"

class testApp : public ofBaseApp{
	
public:
	
	// These are the default functions defined by openFrameworks
	void setup();
	void update();
	void draw();
	
	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	/////////////////////////////////////////////////////
	//////// OPEN CV & VIDEO RELATED ////////////////////
	/////////////////////////////////////////////////////
	
	/// Object grab video from the webcam
	ofVideoGrabber mGrabber;
	
	// Open CV calls its images matrices. The main object for an OpenCV image
	// is cv::Mat, where 'Mat' is short for Matrix.
	/// We remember the previous frame here.
	cv::Mat mFrame;
	/// We keep a mix of previous frames to be considered the background here
	cv::Mat mBackground;
	/// The difference between that frame and the previous frame is put here
	cv::Mat mDifferenceImage;
	/// Rather than just assuming the previous frame to be our background, or a
	/// single frame captured a while ago, we slightly blend the background with
	/// the current frame. This is essentially a low-pass filter on the background.
	/// This value is between 0. and 1.
	ofxParameter<float> mBackgroundUpdateRate;
	
	/// This object subtracts the background from incoming frames for us and performs
	/// a threshold to give us a binary image of foreground and background
//	ofxCv::RunningBackground mBackgroundRemover;
	/// We put the output of the above into here
//	cv::Mat mThresholdedFrame;
	/// Call this function to train the background remover on a new background.
	void learnNewBackground(bool&);
	
	// Options to determine what we draw to the screen
	ofxParameter<bool> mDrawRawImage;
	ofxParameter<bool> mDrawBackground;
	ofxParameter<bool> mDrawDifferenceImage;

	
	/////////////////////////////////////////////////////
	//////// TRIGGERING NOTES ///////////////////////////
	/////////////////////////////////////////////////////
	
	/// Objects to keep a track of the location-based triggers
	/// This is basically a two dimensional array.
	/// These are indexed as [row][column]
	vector<vector<SimpleThresholdTrigger*> > mTriggers;
	void setupTriggers();
	/// How many triggers we want to overlay over the frame
	ofxParameter<int> mTriggerRows;
	ofxParameter<int> mTriggerCols;
	ofxParameter<float> mThresholdValue;
	/// openFrameworks will call this function when mThresholdValue changes
	void thresholdValueHasChangedCallback(float& newValue);
	/// Prevent the triggers from being updated
	ofxParameter<bool> mMuteTriggers;
	
	
	/////////////////////////////////////////////////////
	//////// MISC ///////////////////////////////////////
	/////////////////////////////////////////////////////
	
	/// The user interface object.
	ofxPanel mGui;
	/// Pointers to various objects that we have added to the gui and need
	/// to remember to delete.
	vector<ofxBaseGui*> mGuiElements;
	/// Helper function for adding unchanging labels to the gui
	void addLabelToGui(string const& labelText);

	/// An object to send MIDI data
	ofxMidiOut mMidiOut;
	/// A label on the gui to provide information about the MIDI status.
	ofxLabel mMidiStatus;
	
};
