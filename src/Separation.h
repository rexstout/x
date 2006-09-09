#ifndef SEPARATION_H
#define SEPARATION_H

class View;

class Separation
{
 public:
    Separation(const double oX, const double oY, const double oZ,
	       const double tX, const double tY, const double tZ, 
	       const double sX, const double sY, const double sZ);

    ~Separation();

    void getOrigin(double &oX, double &oY, double &oZ);
    double getSeparation() const { return(separation_); };

    void setAngle(const double angle);
    double getAngle() const { return(angle_); };

 private:
    double tX_, tY_, tZ_;
    double sX_, sY_, sZ_;
    double oX_, oY_, oZ_;

    double separation_;   // target 1 - observer - target 2
    double angle_;        // observer - target 1 - target 2

    View *view_;

};

#endif
