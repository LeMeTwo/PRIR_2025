// openmp_check_points.cpp
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <sstream>

struct Point {
    double x, y;
};

std::vector<Point> load_points(const std::string& filename) {
    std::ifstream file(filename);
    std::vector<Point> points;
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double x, y;
        if (iss >> x >> y) {
            points.push_back({x, y});
        }
    }
    return points;
}

bool check_point(double x, double y) {
    double fx = pow(x, 5) - pow(x, 2) + 2*x + 1;
    return std::abs(y - fx) < 1e-6;
}

int main() {
    auto points = load_points("punkty.txt");
    int n = points.size();

    std::vector<Point> matches;

    double start = omp_get_wtime();

    // Użycie wektora pomocniczego do zaznaczenia które punkty pasują
    std::vector<bool> is_match(n);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        is_match[i] = check_point(points[i].x, points[i].y);
    }

    // Zbieramy wyniki do wektora matches (niezrównoleglone)
    for (int i = 0; i < n; i++) {
        if (is_match[i]) {
            matches.push_back(points[i]);
        }
    }

    double end = omp_get_wtime();

    std::cout << "Liczba punktow spelniajacych warunek: " << matches.size() << std::endl;
    std::cout << "Czas wykonania: " << (end - start) << " s\n";

    return 0;
}
