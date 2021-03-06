/*
 *  CodeTimer.h
 *  MinimTouch
 *
 *  Created by Damien Di Fede on 4/1/10.
 *  Copyright 2010 __MyCompanyName__. All rights reserved.
 *
 */

#include "sys/time.h"
#include "stdio.h"

class CodeTimer
{
public:
	CodeTimer(const char * tag, long int printThreshold = 0) 
	: mTag(tag)
	, mPrintThreshold(printThreshold)
	{
		gettimeofday(&mStart, NULL);
	}
	
	~CodeTimer()
	{
		gettimeofday(&mEnd, NULL);
		timeval dur;
		timersub(&mEnd, &mStart, &dur);
		if ( dur.tv_sec * 1000000 + dur.tv_usec > mPrintThreshold )
		{
			printf("%s took %lu seconds and %lu microseconds.\n", mTag, (long unsigned int)dur.tv_sec, (long unsigned int)dur.tv_usec);
		}
	}
	
private:
	
	const char * mTag;
	long int     mPrintThreshold;
	timeval		 mStart;
	timeval		 mEnd;
	
};