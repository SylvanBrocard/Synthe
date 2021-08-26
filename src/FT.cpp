#include <complex>
#include <math.h>
#include <FT.h>
#include <constante.h>

void FT(std::vector<float>& buffer, int buffer_size, std::vector<float>& bands, int n_bands, std::vector<std::complex<float>>& cplx_spectrum)
{
    std::complex<float> integral;
    const std::complex<float> I(0.0,1.0);
    // float freq_resolution = buffer_size * sampling_rate;
    float freq_max = sampling_rate / 2.0;

    for(int i = 0; i < n_bands; i++){
        integral = {0.0, 0.0};
        float omega = i * 2* M_PIf32 * freq_max / n_bands;
        for(int j = 0; j < buffer_size; j++){
            float t = j / sampling_rate;
            integral += buffer[j] * std::exp(I * (float)(omega * t));
        }
        bands[i] = std::abs(integral) / sqrt(2 * M_PIf32);
        cplx_spectrum[i] = integral / sqrt(2 * M_PIf32);

    }
}