#include "bandPass.h"
#include <math.h>

float bandPass(std::vector<float> rawValues, std::vector<float> filteredValues, float freqMin, float freqMax)
{
    float frequencyCenter, bandWidth;

    frequencyCenter = (freqMin + freqMax) / 4;
    bandWidth = (freqMax - freqMin) / 2;

    float r = 1.0 - 3.0 * bandWidth;
    float k = (1.0 - 2.0 * r *cos(2.0 * M_PIf32 * frequencyCenter) + r * r)/(2.0 - 2.0 * cos(2.0  * M_PIf32 * frequencyCenter));

    float a0 = 1.0 - k;
    float a1 = 2.0 * (k - r) * cos(2.0 * M_PIf32 * frequencyCenter);
    float a2 = r * r - k;
    float b1 = 2.0 * r * cos(2.0 * M_PIf32 * frequencyCenter);
    float b2 = - r * r;

    // printf("%f %f %f %f %f %f %f\n", r, k, a0, a1, a2, b1, b2);

    float returnValue = a0 * rawValues[0] + a1 * rawValues[1] + a2 * rawValues[2] + b1 * filteredValues[1] + b2 * filteredValues[2];

    rawValues[2] = rawValues[1];
    rawValues[1] = rawValues[0];

    filteredValues[2] = filteredValues[1];
    filteredValues[1] = returnValue;

    return returnValue;
}