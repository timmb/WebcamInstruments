//
//  ScaleMapper.cpp
//  WebcamInstruments
//
//  Created by Tim Murray-Browne on 23/11/2012.
//
//

#include "ScaleMapper.h"
#include "ofMain.h"


int LoveTheC::getPitch(int row, int col, int numRows, int numCols)
{
	// row chooses octave. 24 is a low C
	// (see http://www.midimountain.com/midi/midi_note_numbers.html )
	
	// rows are counted from the top but it makes more sense to have lower notes at the bottom, so we'll flip the row
	row = numRows - row;
	
	
	int note = 24 + 12 * row;
	// let's let the middle column be middle C and then go in fifths as we move away from this
	int distanceFromMiddle = col - numCols / 2;
	note = note + 7 * distanceFromMiddle;
	
	// Finally, check to make sure the note is still a valid MIDI note number
	note = max(0, min(127, note));
	
	return note;
	
}


/// This is similar to LoveTheC but we alternate between fifths and fourths instead of just fifths
int LoveTheC2::getPitch(int row, int col, int numRows, int numCols)
{
	// row chooses octave. 24 is a low C
	// (see http://www.midimountain.com/midi/midi_note_numbers.html )
	
	// rows are counted from the top but it makes more sense to have lower notes at the bottom, so we'll flip the row
	row = numRows - row;
	
	
	int note = 24 + 12 * row;
	// let's let the middle column be middle C and then go in fifths as we move away from this
	int distanceFromMiddle = col - numCols / 2;
	if (row % 2 ==0) // if row is an even number
	{
		note = note + 7 * distanceFromMiddle;
	}
	else // if row is an odd number
	{
		note = note + 5 * distanceFromMiddle;
	}
	
	// Finally, check to make sure the note is still a valid MIDI note number
	note = max(0, min(127, note));
	
	return note;
	
}


int Tonnetz::getPitch(int row, int col, int numRows, int numCols)
{
	// Tonnetz - there are variations on this structure and this is possibly
	// not the 'authentic tonnetz' - check Wikipedia for more info.
	// Alternate between major and minor thirds as you move sideways
	// Move up a fifth when you move up
	// As above we flip the row
	row = numRows - row;
	
	// Middle note is middle C, which is 48
	int note = 48;
	
	// Now we find how far we are from the middle square
	int rowDifference = row - numRows / 2;
	int colDifference = col - numCols / 2;
	
	// Up and down is easy, we just add on fifths (7 semitones)
	note = note + rowDifference * 7;
	
	// Moving to the right we add on a fifth each time we pass two columns, and then an extra major third
	// Remember that when we divide integers the remainder is ignored
	if (colDifference > 0)
	{
		note = note + (colDifference/2) * 7;
		// if colDifference is odd then we had a remainder which we need to now add on a major third for
		if (colDifference%2 == 1)
		{
			note = note + 4;
		}
	}
	else
	{
		// If we're moving to the left it's a similar story, except this time if we have an odd number then
		// we add a minor third rather than a major.
		note = note + (colDifference/2) * 7;
		// Small caveat - using the % gives us the remainder, but -5/2 has remainder -1 not +1.
		// So we don't need to worry about the sign of the remainder, we'll just check if it's non-zero
		// to test if it's odd
		if (colDifference%2 != 0)
		{
			note = note - 3;
		}
	}
	
	// double check range again
	note = max(0, min(127, note));
	
	return note;
	
	
}