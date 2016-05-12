#include "Timer.h"

Timer::Timer(){
	QueryPerformanceFrequency(&frequency);
	enableOutput();
	resetTimer();
}

void Timer::startTimer()
{
	elapsedTime = 0.0;
	QueryPerformanceCounter(&timeStampStart);
}

void Timer::stopTimer()
{
	// stop timer
	QueryPerformanceCounter(&timeStampEnd);
	elapsedTime = (timeStampEnd.QuadPart - timeStampStart.QuadPart) * 1000.0 / frequency.QuadPart;
}

void Timer::accumulateTime()
{
	if (!ticksReached){
		sumOfElapsedTime += elapsedTime;
		numberOfticks++;
		ticksReached = (numberOfticks == maxTicks);
	}
}

double Timer::getElapsedTime()
{
	std::cout << "Elapsed Time:" << elapsedTime << "ms \n";
	return elapsedTime;
}

double Timer::getAverageElapsedTime()
{
	if (ticksReached){
		std::cout << "Averaged Elapsed Time:" << elapsedTime << "ms" << " over " << maxTicks << "Samples \n";
		numberOfticks = 0;
		ticksReached = false;
		return sumOfElapsedTime / maxTicks;
	}
	else{
		return -1.0;
	}
}

bool Timer::isTimeReady()
{
	return ticksReached;
}

void Timer::resetTimer()
{
	// start timer
	elapsedTime = 0.0;
	sumOfElapsedTime = 0.0;
	ticksReached = false;
	numberOfticks = 0;
}

void Timer::setMaxTicks(int ticks)
{
	maxTicks = ticks;
}

void Timer::disableOutput()
{
	outputEnabled = false;
}

void Timer::enableOutput()
{
	outputEnabled = true;
}
