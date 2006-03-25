#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

#include "Options.h"
#include "xpDefines.h"
#include "xpUtil.h"

static bool
fileExists(string &filename)
{
    bool returnVal = false;

    stringstream msg;
    msg << "Looking for " << filename << " ... ";
    ifstream f(filename.c_str());
    if (f.is_open())
    {
	msg << "found\n";
	f.close();
	returnVal = true;
    }
    else
    {
	msg << "not found\n";
	returnVal = false;
    }

    Options *options = Options::getInstance();
    if (options->Verbosity() > 2)
	xpMsg(msg.str(), __FILE__, __LINE__);

    return(returnVal);
}

bool
findFile(string &filename, const string &subdir)
{
    // Check if the file exists in the current directory before going
    // to searchdir
    if (fileExists(filename)) return(true);

    string newname;

    Options *options = Options::getInstance();
    vector<string> searchdir = options->getSearchDir();

    for (int i = searchdir.size() - 1; i >= 0; i--)
    {
	// Check in searchdir itself
	newname = searchdir[i];
	newname += separator;
	newname += filename;

	if (fileExists(newname))
	{
	    filename = newname;
	    return(true);
	}

	// Now look in searchdir + subdir
	newname = searchdir[i];
	newname += separator;
	if (!subdir.empty())
	{
	    newname += subdir;
	    newname += separator;
	}
	newname += filename;

	if (fileExists(newname))
	{
	    filename = newname;
	    return(true);
	}
    }

    string errMsg("Can't find ");
    errMsg += filename;
    errMsg += " in\n";
    for (int i = searchdir.size() - 1; i >= 0; i--)
    {
	errMsg += searchdir[i];
	errMsg += separator;
	errMsg += subdir;
	errMsg += "\n";
    }
    xpWarn(errMsg, __FILE__, __LINE__);
    return(false);
}
