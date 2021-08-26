#include <complex>
#include <math.h>

void FT(float *buffer, int buffer_size, float* bands, int n_bands)
{
    std::complex<float> integral;
    const std::complex<float> I(0.0,1.0);

    for(int i = 0; i < n_bands; i++){
        integral = {0.0, 0.0};
        for(int j = 0; j < buffer_size; j++){
            integral += buffer[j] * std::exp(I * (float)(j * i));
        }
        bands[i] = std::abs(integral) / sqrt(2 * M_PIf32);
    }
}