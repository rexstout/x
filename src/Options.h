#ifndef OPTIONS_H
#define OPTIONS_H

#include <ctime>
#include <string>
#include <vector>

#include "body.h"

class PlanetProperties;

class Options
{
 public:
    static Options* getInstance();

    Options();
    ~Options();

    void parseArgs(int argc, char **argv);

    const std::vector<std::string> & ArcFiles() const { return(arcFiles_); };

    const std::string & Background() const { return(background_); };
    double BaseMagnitude() const { return(baseMag_); };

    const std::string & ConfigFile() const { return(configFile_); };
    const unsigned char * Color() const { return(color_); };

    int DisplayMode() const      { return(displayMode_); };

    const std::string & DateFormat() const { return(dateFormat_); };
    bool DrawLabel() const { return(drawLabel_); };
    bool DrawUTCLabel() const { return(drawUTCLabel_); };
    const std::string & DynamicOrigin() const { return(dynamicOrigin_); };
    
    double FieldOfView() const           { return(fov_); };
    void FieldOfView(const double f)     { fov_ = f; };

    const std::string & Font() const { return(font_); };
    int FontSize() const { return(fontSize_); };

    bool Fork() const { return(fork_); };
    int FOVMode() const          { return(fovMode_); };

    void setCenterX(const double x)    { center_x = x; };
    void setCenterY(const double y)    { center_y = y; };
    bool CenterSelected() const     { return(centerSelected_); };
    double getCenterX() const          { return(center_x); };
    double getCenterY() const          { return(center_y); };

    const std::string & getOutputBase() const      { return(base_); };
    const std::string & getOutputExtension() const { return(extension_); };

    unsigned long Hibernate() const { return(hibernate_); };
    unsigned long IdleWait() const { return(idleWait_); };

    bool InterpolateOriginFile() const { return(interpolateOriginFile_); };

    const std::string & JPLFile() const { return(jplFile_); };

    int JPEGQuality() const { return(quality_); };
    double getJulianDay() const     { return(julianDay_); };

    int LabelMask() const { return(labelMask_); };
    int LabelX() const { return(labelX_); };
    int LabelY() const { return(labelY_); };
    const std::string & LabelString() const { return(labelString_); };

    double Latitude() const      { return(latitude_); };
    void Latitude(const double b) { latitude_ = b; };

    double LogMagnitudeStep() const { return(logMagStep_); };

    void LocalTime(const double l) { localTime_ = l; };

    double Longitude() const     { return(longitude_); };
    void Longitude(const double l) { longitude_ = l; };

    bool LightTime() const       { return(lightTime_); };

    bool MakeCloudMaps() const { return(makeCloudMaps_); };

    const std::vector<std::string> & MarkerFiles() const { return(markerFiles_); };
    const std::string & MarkerBounds() const { return(markerBounds_); };

    int North() const            { return(north_); };
    int NumTimes() const         { return(numTimes_); };
    void NumTimes(const int n) { numTimes_ = n; };

    bool OppositeSide() const { return(oppositeSide_); };
    int OriginMode() const;
    body getOrigin() const          { return(origin_); };
    const std::string & OriginFile() const { return(originFile_); };

    bool Pango() const { return(pango_); };
    const std::string & PostCommand() const { return(post_command_); };
    const std::string & PrevCommand() const { return(prev_command_); };
    bool PrintEphemeris() const { return(printEphemeris_); };
    int Projection() const       { return(projection_); };
    void Projection(const int p)     { projection_ = p; };
    int ProjectionMode() const       { return(projectionMode_); };

    double Radius() const        { return(radius_); };
    void Radius(const double r)  { radius_ = r; };
    double getRange() const         { return(range_); };
    void setRange(const double r) { range_ = r; };

    double getRotate() const        { return(rotate); };
    void setRotate(const double r) { rotate = rotate0 + r; };

    const std::vector<std::string> & getSearchDir() const { return(searchdir); };
    bool SaveDesktopFile() const { return(saveDesktopFile_); };
    const std::vector<int> & SpiceEphemeris() const { return(spiceEphemeris_); };
    const std::vector<std::string> & SpiceFiles() const { return(spiceFiles_); };
    double StarFreq() const        { return(starFreq_); };
    const std::string & getStarMap() const { return(star_map); };

    double SunLat() const      { return(sunLat_); };
    void SunLat(const double b) { sunLat_ = b; };
    double SunLon() const     { return(sunLon_); };
    void SunLon(const double l) { sunLon_ = l; };

    int TargetMode() const { return(targetMode_); };

    double getTimeWarp() const      { return(timewarp); };
    time_t getTVSec() const         { return(tv_sec); };

    bool TransPNG() const { return(transpng_); };
    bool Transparency() const { return(transparency_); };

    std::string TmpDir() const { return(tmpDir_); };

    bool UniversalTime() const { return(universalTime_); };

    int Verbosity() const { return(verbosity_); };
    bool VirtualRoot() const        { return(virtual_root); };
    int getWait() const             { return(wait); };

    int getWindowX() const          { return(windowX_); };
    int getWindowY() const          { return(windowY_); };

    const std::string & WindowTitle() const { return(windowTitle_); };

    unsigned long XID() const { return(xid_); };

    bool GeometrySelected() const   { return(geometrySelected_); };
    int GeometryMask() const     { return(geometryMask_); };
    int getWidth() const            { return((int) width); };
    int getHeight() const           { return((int) height); };


    body getPrimary() const { return(primary_); };

    void setTarget(PlanetProperties *planetProperties[]);
    void setOrigin(PlanetProperties *planetProperties[]);
    void getOrigin(double &X, double &Y, double &Z);
    body getTarget() const          { return(target_); };
    void getTarget(double &X, double &Y, double &Z);
    void setTarget(const double X, const double Y, const double Z);
    bool UseCurrentTime() const { return(useCurrentTime_); };
    void incrementTime(const time_t tv_sec);
    void setTime(const double jd);

 private:

    static Options *instance_;

    std::vector<std::string> arcFiles_;

    std::string background_;
    std::string base_;
    double baseMag_;    // a star of this magnitude has a pixel
                        // brightness of 1
    bool centerSelected_;
    unsigned char color_[3];
    std::string configFile_;

    std::string dateFormat_;
    int displayMode_;
    bool drawLabel_;
    bool drawUTCLabel_;
    std::string dynamicOrigin_;

    std::string extension_;

    std::string font_;
    int fontSize_;
    bool fork_;
    double fov_;          // field of view
    int fovMode_;

    int geometryMask_;
    bool geometrySelected_;

    unsigned long hibernate_;

    unsigned long idleWait_;
    bool interpolateOriginFile_;

    std::string jplFile_;

    int labelMask_;
    int labelX_;
    int labelY_;
    std::string labelString_;
    double latitude_;
    bool lightTime_;
    double localTime_;
    double logMagStep_;   // log (base 10) of brightness increase for
                          // each step in magnitude (default 0.4)
    double longitude_;

    bool makeCloudMaps_;
    std::string markerBounds_;
    std::vector<std::string> markerFiles_;

    int north_;                    // BODY, GALACTIC, ORBIT, or TERRESTRIAL
    int numTimes_;

    bool oppositeSide_;
    body origin_;
    std::string originFile_;
    int originID_;                // for NAIF or NORAD bodies
    int originMode_;                     // BODY, LBR, RANDOM, MAJOR,
                                         // SYSTEM, ABOVE, BELOW
    double oX_, oY_, oZ_;   // heliocentric rectangular coordinates of
                            // the observer

    bool pango_;
    std::string post_command_;    // command to run after xplanet renders
    std::string prev_command_;    // command to run before xplanet renders
    body primary_;
    bool printEphemeris_;
    int projection_;         // type of map projection
    int projectionMode_;         // type of map projection

    int quality_;        // For JPEG images

    double radius_;      // radius of the body, as a fraction of the
                         // height of the display
    bool random_;
    bool rangeSpecified_; // if the -range option is used
    double range_;        // distance from the body, in units of its radius
    double rotate;       // rotate0 plus any increments
    double rotate0;      // rotation angle specified on command line

    bool saveDesktopFile_;
    std::vector<int> spiceEphemeris_;
    std::vector<std::string> spiceFiles_;
    double starFreq_;
    std::string star_map;
    double sunLat_, sunLon_;    // these aren't options and shouldn't
                                // be here, but it's a convenient way
                                // to put them in the label
    body target_;
    int targetMode_;     // BODY, RANDOM, MAJOR
    int targetID_;                // for NAIF or NORAD bodies
    double timewarp;     // multiplication factor for the passage of time
    std::string tmpDir_;
    bool transparency_;
    bool transpng_;

    double tX_, tY_, tZ_;   // heliocentric rectangular coordinates of
                            // the target

    bool universalTime_;
    bool useCurrentTime_;

    int verbosity_;
    bool virtual_root;

    int wait;           // time (in seconds) to wait between updates
    unsigned int width, height;
    int windowX_, windowY_;
    std::string windowTitle_;

    unsigned long xid_;

    double center_x, center_y;
    double julianDay_;


    std::vector<std::string> searchdir; // check these directories for files

    time_t tv_sec;      // UNIX time (seconds from 00:00:00 UTC on
                        // January 1, 1970)

    void showHelp();
};

#endif
