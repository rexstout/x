#ifndef READ_ORIGIN_FILE_H
#define READ_ORIGIN_FILE_H

#include <vector>
#include <string>

struct LBRPoint
{
    double time;
    double latitude;
    double longitude;
    double radius;
};

extern void
readOriginFile(std::string filename, std::vector<LBRPoint> &originMap);

#endif
