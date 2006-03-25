#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <sys/time.h>
#include <unistd.h>
using namespace std;

#include "config.h"

#include "keywords.h"
#include "Options.h"
#include "Timer.h"
#include "xpUtil.h"

Timer::Timer(const int w, const unsigned long h, const unsigned long i) 
    : wait_(w), hibernate_(h), idlewait_(i)
{
}

Timer::~Timer()
{
}

void
Timer::Update()
{
    gettimeofday(&currentTime_, NULL);
    nextUpdate_ = currentTime_.tv_sec + wait_;
}

// returns false if the program should exit after this sleep
bool
Timer::Sleep()
{
    bool returnVal = true;

    // Sleep until the next update
    gettimeofday(&currentTime_, NULL);
    time_t sleep_time = nextUpdate_ - currentTime_.tv_sec;
    if (sleep_time > 0) 
    {
	Options *options = Options::getInstance();
	if (options->Verbosity() > 0)
	{
	    stringstream msg;
	    msg << "sleeping for " << static_cast<int> (sleep_time) 
		 << " seconds until " << ctime((time_t *) &nextUpdate_);
	    xpMsg(msg.str(), __FILE__, __LINE__);
	}
	sleep(sleep_time);
    }

    return(returnVal);
}
