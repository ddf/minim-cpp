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
	CodeTimer(const char * tag) : mTag(tag)
	{
		gettimeofday(&mStart, NULL);
	}
	
	~CodeTimer()
	{
		gettimeofday(&mEnd, NULL);
		printf("%s took %d seconds and %d microseconds.\n", mTag, mEnd.tv_sec - mStart.tv_sec, mEnd.tv_usec - mStart.tv_usec);
	}
	
private:
	
	const char * mTag;
	timeval		 mStart;
	timeval		 mEnd;
	
};