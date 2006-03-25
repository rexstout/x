#ifndef TIMER_H
#define TIMER_H

#include <ctime>
#include <sys/time.h>

class Timer
{
 public:
    Timer(const int w, const unsigned long h, const unsigned long i);
    virtual ~Timer();

    void Update();
    virtual bool Sleep();

 protected:
    const int wait_;
    const unsigned long hibernate_;
    const unsigned long idlewait_;

    struct timeval currentTime_;
    time_t nextUpdate_;
};

#endif
