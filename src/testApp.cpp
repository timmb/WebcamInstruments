#include "testApp.h"

const int testApp::MAX_TRIGGERS_IN_LINE;

//--------------------------------------------------------------
void testApp::setup(){
	// Open the video grabber to get images from the webcam
	mGrabber.initGrabber(640, 480, false);
	// Initialize our matrices based around the size we get from mGrabber
	mFrame = cv::Mat(mGrabber.height, mGrabber.width, CV_8UC3);
	mBackground = cv::Mat(mFrame.size(), mFrame.type());
	mDifferenceImage = cv::Mat(mFrame.size(), mFrame.type());
	
	// Set up our parameters
	mDrawRawImage = ofxParameter<bool>("Draw raw image", true);
	mDrawDifferenceImage = ofxParameter<bool>("Draw thresholded image", false);
	mTriggerCols = ofxParameter<int>("Number of trigger columns", 12);
	mTriggerCols.setMax(MAX_TRIGGERS_IN_LINE);
	mTriggerRows = ofxParameter<int>("Number of trigger rows", 6);
	mTriggerRows.setMax(MAX_TRIGGERS_IN_LINE);
	mThresholdValue = ofxParameter<float>("Threshold value", .03);
	mThresholdValue.setMin(0.);
	mThresholdValue.setMax(1.);
	mThresholdValue.addListener(this, &testApp::thresholdValueHasChangedCallback);
	mBackgroundUpdateRate = ofxParameter<float>("Background update rate", 0.1);
	mBackgroundUpdateRate.setMin(0.);
	mBackgroundUpdateRate.setMax(1.);
	
	// Set up the user interface
	ofxButton * learnNewBackgroundButton = new ofxButton;
	learnNewBackgroundButton->setup("* Learn new background");
	learnNewBackgroundButton->addListener(this, &testApp::learnNewBackground);
	mGuiElements.push_back(learnNewBackgroundButton);
	
	mMidiStatus.setup("MIDI:", "");
	
	mGui.setup("", "settings.xml", 0, mGrabber.height);
	mGui.add(mDrawRawImage);
	mGui.add(mDrawBackground);
	mGui.add(mDrawDifferenceImage);
	mGui.add(learnNewBackgroundButton);
	mGui.add(mBackgroundUpdateRate);
	mGui.add(&mMidiStatus);
	addLabelToGui("Simple threshold trigger");
	mGui.add(mTriggerRows);
	mGui.add(mTriggerCols);
	mGui.add(mThresholdValue);
	
	
	// Set up the MIDI connection
	vector<string> portNames = ofxMidiOut::getPortList();
	// search for the IAC loopback driver (a virtual midi port on Macs)
	string port;
	for (int i=0; i<portNames.size(); ++i)
	{
		// for each port name check if it contains the characters IAC
		if (portNames.at(i).find("IAC")!=string::npos)
		{
			// we've found the IAC driver. Save it's name and stop looking
			port = portNames.at(i);
			break;
		}
	}
	if (port != "")
	{
		bool midiPortOpenedSuccessfully = mMidiOut.openPort(port);
		if (midiPortOpenedSuccessfully)
		{
			mMidiStatus = "Successfully opened " + port;
		}
		else
		{
			mMidiStatus = "Failed to open " + port;
		}
	}
	else
	{
		mMidiStatus = "Could not find loopback MIDI port to open";
	}
	
	ofSetFrameRate(60);
}

//--------------------------------------------------------------
void testApp::addLabelToGui(string const& labelText)
{
	ofxLabel* l = new ofxLabel;
	l->setup(labelText, "");
	mGuiElements.push_back(l);
	mGui.add(l);
}

//--------------------------------------------------------------
void testApp::learnNewBackground(bool& buttonIsPressed)
{
	mBackground.setTo(mFrame);
}

//--------------------------------------------------------------
void testApp::setupTriggers()
{
	// Check there aren't a ridiculous number requeted
	mTriggerRows = min<int>(mTriggerRows, MAX_TRIGGERS_IN_LINE);
	mTriggerCols = min<int>(mTriggerCols, MAX_TRIGGERS_IN_LINE);
	
	// Delete any existing triggers
	for (int i=0; i<mTriggers.size(); ++i)
	{
		for (int j=0; j<mTriggers[i].size(); ++j)
		{
			// Delete the SimpleThresholdTrigger object
			delete mTriggers[i][j];
		}
	}
	// Remove the pointers referring to the objects
	mTriggers.clear();
	
	// Make new triggers. Each trigger is associated with a square area in the
	// image. We will define this square as starting at (left, top) and ending
	// at (right, bottom).
	float triggerHeight = float(mBackground.rows) / mTriggerRows;
	float triggerWidth = float(mBackground.cols) / mTriggerCols;
	// A counter to cycle through different pitches
	int pitch = 36;
	for (int i=0; i<mTriggerRows; ++i)
	{
		vector<SimpleThresholdTrigger*> row;
		// These parameters are the same for all the triggers in a single row
		int top  = int(i * triggerHeight);
		int bottom = int((i+1) * triggerHeight);
		int height = bottom - top;
		// Just to double check we don't fall off the end of the frame
		bottom = min(mBackground.rows, bottom);
		
		for (int j=0; j<mTriggerCols; ++j)
		{
			int left = int(j * triggerWidth);
			int right = int((j+1) * triggerWidth);
			right = min(mBackground.cols, right);
			int width = right - left;
			
			// Define the trigger rectangle
			cv::Rect triggerArea = cv::Rect(left, top, width, height);
			// Create the trigger
			SimpleThresholdTrigger* t = new SimpleThresholdTrigger(&mMidiOut, pitch, triggerArea);
			row.push_back(t);
			// increment pitch
			++pitch;
		}
		mTriggers.push_back(row);
	}
}



//--------------------------------------------------------------
void testApp::update(){
	// Check the webcam for any new frames
	mGrabber.update();
	if (mGrabber.isFrameNew())
	{
		// Get the current frame
		cv::Mat frame = ofxCv::toCv(mGrabber);
		// To make the image seem like a mirror we reflect it in the y axis
		// and then save the result into mFrame.
		cv::flip(frame, mFrame, 1);
			
		// Subtract the background
		cv::absdiff(mFrame, mBackground, mDifferenceImage);
		
		// We update our background picture with this new frame
		// http://opencv.willowgarage.com/documentation/cpp/core_operations_on_arrays.html#addWeighted
		cv::addWeighted(mFrame, mBackgroundUpdateRate, mBackground, 1.-mBackgroundUpdateRate, 0., mBackground);
			
		// Check if the user has changed the number of triggers
		// and reset them if they have.
		if (mTriggers.size() != mTriggerRows
			|| (!mTriggers.empty() && mTriggers[0].size() != mTriggerCols))
		{
			setupTriggers();
		}
		for (int i=0; i<mTriggers.size(); ++i)
		{
			for (int j=0; j<mTriggers[i].size(); ++j)
			{
				mTriggers[i][j]->update(mDifferenceImage);
			}
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofClear(64,64,64);
	ofSetColor(ofColor::white);
	// Draw the current frame to the screen
	if (mDrawRawImage)
	{
		ofxCv::drawMat(mFrame, 0, 0);
	}
	if (mDrawBackground)
	{
		ofxCv::drawMat(mBackground, 0, 0);
	}
	if (mDrawDifferenceImage)
	{
		ofxCv::drawMat(mDifferenceImage, 0, 0);
	}
	// Draw triggers over the top of the frame
	for (int i=0; i<mTriggers.size(); ++i)
	{
		for (int j=0; j<mTriggers[i].size(); ++j)
		{
			mTriggers[i][j]->draw();
		}
	}
	mGui.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}


//-------------
void testApp::thresholdValueHasChangedCallback(float &newValue)
{
	// Tell all the triggers about the new threshold value
	for (int i=0; i<mTriggers.size(); ++i)
	{
		for (int j=0; j<mTriggers[i].size(); ++j)
		{
			mTriggers[i][j]->mThreshold = newValue;
		}
	}
}