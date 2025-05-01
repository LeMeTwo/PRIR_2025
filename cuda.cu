#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <cuda_runtime.h>

__global__ void sprawdz_punkty(const double* xs, const double* ys, int* wynik, int n, double tolerancja) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n) {
        double x = xs[i];
        double y = ys[i];
        double y_calc = pow(x, 5) - pow(x, 2) + 2 * x + 1;
        wynik[i] = (fabs(y - y_calc) <= tolerancja) ? 1 : 0;
    }
}

int main() {
    std::ifstream file("punkty.txt");
    if (!file) {
        std::cerr << "Nie mogę otworzyć pliku punkty.txt\n";
        return 1;
    }

    std::vector<double> xs, ys;
    double x, y;
    while (file >> x >> y) {
        xs.push_back(x);
        ys.push_back(y);
    }

    int n = xs.size();
    double tolerancja = 0.001;

    double *d_xs, *d_ys;
    int *d_wynik, *wynik = new int[n];

    cudaMalloc(&d_xs, n * sizeof(double));
    cudaMalloc(&d_ys, n * sizeof(double));
    cudaMalloc(&d_wynik, n * sizeof(int));

    cudaMemcpy(d_xs, xs.data(), n * sizeof(double), cudaMemcpyHostToDevice);
    cudaMemcpy(d_ys, ys.data(), n * sizeof(double), cudaMemcpyHostToDevice);

    int threadsPerBlock = 256;
    int blocksPerGrid = (n + threadsPerBlock - 1) / threadsPerBlock;

    // Pomiar czasu
    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    cudaEventRecord(start);

    sprawdz_punkty<<<blocksPerGrid, threadsPerBlock>>>(d_xs, d_ys, d_wynik, n, tolerancja);
    cudaDeviceSynchronize();

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    float milliseconds = 0;
    cudaEventElapsedTime(&milliseconds, start, stop);

    cudaMemcpy(wynik, d_wynik, n * sizeof(int), cudaMemcpyDeviceToHost);

    int pasuje = 0, niepasuje = 0;
    for (int i = 0; i < n; ++i) {
        if (wynik[i]) pasuje++;
        else niepasuje++;
    }

    std::cout << "Pasuje: " << pasuje << "\nNie pasuje: " << niepasuje << "\n";
    std::cout << "Czas działania GPU: " << milliseconds << " ms\n";

    // Czyszczenie pamięci
    delete[] wynik;
    cudaFree(d_xs);
    cudaFree(d_ys);
    cudaFree(d_wynik);
    cudaEventDestroy(start);
    cudaEventDestroy(stop);

    return 0;
}
