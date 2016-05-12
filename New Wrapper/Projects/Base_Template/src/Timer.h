#pragma once
#include <windows.h>
#include <iostream>

class Timer{
protected:
private:
	LARGE_INTEGER frequency;        // ticks per second
	LARGE_INTEGER timeStampStart, timeStampEnd;           // ticks
	int maxTicks, numberOfticks;
	bool ticksReached, outputEnabled;
	double elapsedTime, sumOfElapsedTime;
public:
	Timer();
	void startTimer();
	void stopTimer();
	void accumulateTime();
	void disableOutput();
	void enableOutput();
	void resetTimer();
	double getElapsedTime();
	double getAverageElapsedTime();
	bool isTimeReady();
	void setMaxTicks(int ticks);
};