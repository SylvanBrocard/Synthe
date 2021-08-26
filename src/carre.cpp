#include <math.h>
#include <carre.h>

float carre(float phase)
{
    float signal_carre = sin(phase);

    for(int i = 1; i < 100; i++){
        int j = 1+(2*i);
        signal_carre += sin(phase*j)/j;
    }

    return signal_carre;
}