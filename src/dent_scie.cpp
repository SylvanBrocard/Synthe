#include <math.h>
#include <dent_scie.h>


float dentScie(float phase, float brillance)
{
    float signal_dent_scie = sin(phase);

    for(int i = 1; i < 100; i++){
        int j = 2*i;
        
        signal_dent_scie += brillance * (- sin(phase*j)/j + sin(phase*(j+1))/(j+1));

    }

    return signal_dent_scie;
}