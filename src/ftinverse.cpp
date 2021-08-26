#include <complex>
#include <math.h>
#include <ftinverse.h>
#include <constante.h>

void ftinverse(std::vector<std::complex<float>>& bands, int bands_size, std::vector<float>& signal, int n_signal)
{
    std::complex<float> integral;
    const std::complex<float> I(0.0,1.0);
    // float freq_resolution = bands_size * sampling_rate;
    float freq_max = sampling_rate / 2.0;

    for(int j = 0; j < n_signal; j++){

        integral = {0.0, 0.0};
        float t = j / sampling_rate;
        for(int i = 0; i < bands_size; i++){
            float omega = i * 2* M_PIf32 * freq_max / n_signal;
            
            integral += bands[j] * std::exp(-I * (float)(omega * t));
        }

        signal[j] = integral.real() / sqrt(2 * M_PIf32);
    }
}