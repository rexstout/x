#include <cstdlib>
#include <sstream>
using namespace std;

#include <sys/time.h>
#include <unistd.h>

#include "config.h"

#include "keywords.h"
#include "Options.h"
#include "xpUtil.h"

#include "TimerX11.h"

#include "libdisplay/DisplayX11.h"

Display *TimerX11::display_ = NULL;

TimerX11::TimerX11(const int w, const unsigned long h, 
                   const unsigned long i) : Timer(w, h, i)
{
    display_ = XOpenDisplay(NULL);

#ifdef HAVE_XSS
    screenSaverInfo_ = NULL;
    if (display_)
    {
        const int screen_num = DefaultScreen(display_);
        root_ = RootWindow(display_, screen_num);
        int event_base, error_base;
        
        if (XScreenSaverQueryExtension(display_, &event_base, &error_base))
            screenSaverInfo_ = XScreenSaverAllocInfo();
    }
#endif
}

TimerX11::~TimerX11()
{
    XCloseDisplay(display_);

#ifdef HAVE_XSS
    XFree(screenSaverInfo_);
#endif
}

// Sleep for sleep_time seconds.  Also check if this is a window
// that's been closed, in which case the program should quit.
bool
TimerX11::Sleep(time_t sleep_time)
{
    Options *options = Options::getInstance();
    if (options->DisplayMode() == WINDOW)
    {
        Window window = DisplayX11::WindowID();
        Atom wmDeleteWindow = XInternAtom(display_, 
                                          "WM_DELETE_WINDOW", 
                                          False);
        
        XSetWMProtocols(display_, window, &wmDeleteWindow, 1);
        XSelectInput(display_, window, KeyPressMask);
        XEvent event;

        // Check every 1/10th of a second if there's been a request to
        // kill the window
        for (int i = 0; i < sleep_time * 10; i++)
        {
            if (XCheckTypedWindowEvent(display_, window, 
                                       ClientMessage, &event) == True) 
            {
                if ((unsigned int) event.xclient.data.l[0] 
                    == wmDeleteWindow)
                    return(false);
            }
            else if (XCheckTypedWindowEvent(display_, window,
                                            KeyPress, &event) == True)
            {
                KeySym keysym;
                char keybuf;
                XLookupString(&(event.xkey), &keybuf, 1, &keysym, NULL);
                if (keybuf == 'q' || keybuf == 'Q') 
                    return(false);
            }
            else
            {
                usleep(100000);  // sleep for 1/10 second
            }
        }
    }
    else
    {
        sleep(sleep_time);
    }
    return(true);
}


// returns false if the program should exit after this sleep
bool
TimerX11::Sleep()
{
    // Sleep until the next update
    gettimeofday(&currentTime_, NULL);
    time_t sleep_time = nextUpdate_ - currentTime_.tv_sec;
    if (sleep_time > 0) 
    {
        Options *options = Options::getInstance();
        if (options->Verbosity() > 0)
        {
            ostringstream msg;
            msg << "sleeping for " << static_cast<int> (sleep_time) 
                << " seconds until "
                <<  ctime((time_t *) &nextUpdate_);
            xpMsg(msg.str(), __FILE__, __LINE__);
        }
        if (!Sleep(sleep_time)) return(false);
    }

#ifdef HAVE_XSS
    if (screenSaverInfo_ != NULL)
    {
        if (idlewait_ > 0) 
        {
            XScreenSaverQueryInfo(display_, root_, screenSaverInfo_);
            while (screenSaverInfo_->idle < idlewait_) 
            {
                if (!Sleep((idlewait_ - screenSaverInfo_->idle) / 1000))
                    return(false);
                XScreenSaverQueryInfo(display_, root_, screenSaverInfo_);
            }
        }

        if (hibernate_ > 0)
        {
            XScreenSaverQueryInfo(display_, root_, screenSaverInfo_);
            while (screenSaverInfo_->idle > hibernate_) 
            {
                if (!Sleep(1)) return(false);
                XScreenSaverQueryInfo(display_, root_, screenSaverInfo_);
            }
        }
    }
#endif

    return(true);
}
