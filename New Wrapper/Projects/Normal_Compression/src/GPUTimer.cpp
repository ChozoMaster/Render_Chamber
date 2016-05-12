#include "GPUTimer.h"


GPUTimer::GPUTimer(){
	glGenQueries(2, queryID);
	elapsedTime = 0.0;
	ticksReached = false;
	sumOfElapsedTime = 0.0;
	numberOfticks = 0;

	//glGenQueries(1, &query);
	//glGenQueries(QUERY_COUNT, queryID[queryBackBuffer]);
	//glGenQueries(QUERY_COUNT, queryID[queryFrontBuffer]);

	//// dummy query to prevent OpenGL errors from popping out
	//glQueryCounter(queryID[queryFrontBuffer][0], GL_TIMESTAMP);
}

// aux function to keep the code simpler
void GPUTimer::swapQueryBuffers() {

	/*if (queryBackBuffer) {
		queryBackBuffer = 0;
		queryFrontBuffer = 1;
	}
	else {
		queryBackBuffer = 1;
		queryFrontBuffer = 0;
	}*/
}

void GPUTimer::startTimer()
{
	//elapsedTime = 0.0;
	//glBeginQuery(GL_TIME_ELAPSED, queryID[queryBackBuffer][0]);
	//glBeginQuery(GL_TIME_ELAPSED, query);
	glQueryCounter(queryID[0], GL_TIMESTAMP);
}

void GPUTimer::stopTimer()
{
	glQueryCounter(queryID[1], GL_TIMESTAMP);
	while (!done) {
		glGetQueryObjectiv(queryID[1], GL_QUERY_RESULT_AVAILABLE, &done);
		//glGetQueryObjectiv(query,
		//	GL_QUERY_RESULT_AVAILABLE,
		//	&done);
	}
	glGetQueryObjectui64v(queryID[0], GL_QUERY_RESULT, &startTime);
	glGetQueryObjectui64v(queryID[1], GL_QUERY_RESULT, &stopTime);
	//glGetQueryObjectui64v(query, GL_QUERY_RESULT, &time1);
	// stop timer
	//glEndQuery(GL_TIME_ELAPSED);
	//glGetQueryObjectui64v(queryID[queryFrontBuffer][0], GL_QUERY_RESULT, &time1);
	//swapQueryBuffers();
	elapsedTime += ((double)stopTime - startTime) / 1000000.0;
	//std::cout << "Elapsed Time:" << elapsedTime << "ms \n";

}

void GPUTimer::accumulateTime()
{
	if (!ticksReached){
		sumOfElapsedTime += elapsedTime;
		numberOfticks++;
		ticksReached = (numberOfticks == maxTicks);
		resetElapsedTime();
	}
}

double GPUTimer::getElapsedTime()
{
	std::cout << "Elapsed Time:" << elapsedTime << "ms \n";
	return elapsedTime;
}

double GPUTimer::getAverageElapsedTime()
{
	if (ticksReached){
		std::cout << "Averaged Elapsed Time:" << sumOfElapsedTime / maxTicks << "ms" << " over " << maxTicks << "Samples \n";
		numberOfticks = 0;
		ticksReached = false;
		resetElapsedTime();
		double result = sumOfElapsedTime / maxTicks;
		sumOfElapsedTime = 0;
		return result;
	}
	else{
		return -1.0;
	}
}

bool GPUTimer::isTimeReady()
{
	return ticksReached;
}

void GPUTimer::resetElapsedTime()
{
	//// start timer
	elapsedTime = 0.0;
	//sumOfElapsedTime = 0.0;
	//ticksReached = false;
	//numberOfticks = 0;
}

void GPUTimer::setMaxTicks(int ticks)
{
	maxTicks = ticks;
}

void GPUTimer::disableOutput()
{
	outputEnabled = false;
}

void GPUTimer::enableOutput()
{
	outputEnabled = true;
}
