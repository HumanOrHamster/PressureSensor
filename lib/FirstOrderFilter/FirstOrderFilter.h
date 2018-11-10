#ifndef FirstOrderFilter_h
#define FirstOrderFilter_h

// has math lib
#include <Arduino.h>

class FirstOrderFilter
{
float yk;   // reads y_output [k]
float ykm1;   // reads y_output [k - 1]
float ukm1;   // reads u_input [k - 1]
float wnTs;   // wn*Ts;
float a0;   // a0 = 1/(wnTs + 2)
float a1;   // a1 = (wnTs - 2)
public:
void initFilter(float cutOffFreq, float samplingTime, float initialInput);
float computeOutput(float u_in);
void debugPrintParams();

};


#endif
