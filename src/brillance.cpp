#include <brillance.h>


float representation(float phase, float brillance)
{
    float alpha, beta;
    if(brillance<1.0){
        alpha=0;
        beta=brillance;
    }
    else{
        alpha = brillance - 1.0;
        beta = 1.0;
    }
    float signal = sin(phase);

    
    for(int i = 1; i < 100; i++){
        int j = 2*i;
        
        signal += alpha * (- sin(phase*j)/j) + beta * (sin(phase*(j+1))/(j+1));

    }

    return signal;
}