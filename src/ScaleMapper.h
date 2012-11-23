//
//  ScaleMapper.h
//  WebcamInstruments
//
//  Created by Tim Murray-Browne on 23/11/2012.
//
//

#ifndef __WebcamInstruments__ScaleMapper__
#define __WebcamInstruments__ScaleMapper__

#include <iostream>

class ScaleMapper
{
public:
	virtual int getPitch(int row, int col, int numRows, int numCols) = 0;
	
};


class LoveTheC : public ScaleMapper
{
public:
	int getPitch(int row, int col, int numRows, int numCols);
};


class LoveTheC2 : public ScaleMapper
{
public:
	int getPitch(int row, int col, int numRows, int numCols);
};


class Tonnetz : public ScaleMapper
{
public:
	int getPitch(int row, int col, int numRows, int numCols);
};

#endif /* defined(__WebcamInstruments__ScaleMapper__) */
