#pragma once
#include <windows.h>
#include <iostream>
#include "GL\glew.h"

#define QUERY_COUNT 1
#define QUERY_BUFFERS 2

class GPUTimer{
protected:
private:
	GLuint64 startTime, stopTime;
	unsigned int queryID[2];

	//unsigned int queryID[QUERY_BUFFERS][QUERY_COUNT];
	//unsigned int queryBackBuffer = 0, queryFrontBuffer = 1;
//	GLuint64 time1, time2;
	bool ticksReached, outputEnabled;
	int done = 0;
	GLuint query;
	//GLint64 timer1;

	//LARGE_INTEGER frequency;        // ticks per second
	//LARGE_INTEGER timeStampStart, timeStampEnd;           // ticks
	int maxTicks, numberOfticks;

	double elapsedTime, sumOfElapsedTime;
public:
	GPUTimer();
	void swapQueryBuffers();
	void startTimer();
	void stopTimer();
	void accumulateTime();
	void disableOutput();
	void enableOutput();
	void resetElapsedTime();
	double getElapsedTime();
	double getAverageElapsedTime();
	bool isTimeReady();
	void setMaxTicks(int ticks);
};