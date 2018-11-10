
#include <FirstOrderFilter.h>


void FirstOrderFilter::debugPrintParams()
{

        Serial.print(yk); Serial.print(" ");
        Serial.print(ykm1); Serial.print(" ");
        Serial.print(ukm1); Serial.print(" ");
        Serial.print(wnTs); Serial.print(" ");
        Serial.print(a0); Serial.print(" ");
        Serial.print(a1); Serial.print(" ");
        Serial.println(" ");
}

void FirstOrderFilter::initFilter(float cutOffFreq, float samplingTime, float initialInput)
{

        ukm1 = 0.0; // reads u_input [k - 1]
        wnTs = 2*PI*cutOffFreq*samplingTime; // wn*Ts;
        a0 = 1/(wnTs + 2);
        a1 = (wnTs - 2);
        ykm1 = wnTs*initialInput/(wnTs+2); // reads y_output [k - 1]

}

float FirstOrderFilter::computeOutput(float uk)
{       // first order filter
        // of the form in continous domain: wn / (s + wn)
        // discretization was performed using bilinear transform
        // s = (2/Ts) * ( z- 1) / (z + 1)
        yk = a0 * ( wnTs*(uk + ukm1) - a1*ykm1 );
        ykm1 = yk;
        ukm1 = uk;
        return yk;
}
