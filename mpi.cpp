// mpi_check_points.cpp
#include <mpi.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
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

int main(int argc, char* argv[]) {
    MPI_Init(&argc, &argv);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<Point> all_points;
    std::vector<Point> local_matches;

    double start_time = MPI_Wtime();

    if (rank == 0) {
        all_points = load_points("punkty.txt");
    }

    // Broadcast liczby wszystkich punktów
    int total_points = all_points.size();
    MPI_Bcast(&total_points, 1, MPI_INT, 0, MPI_COMM_WORLD);

    int chunk_size = total_points / size;
    int remainder = total_points % size;

    int local_size = (rank < remainder) ? (chunk_size + 1) : chunk_size;
    int offset = (rank < remainder) ? rank * local_size : rank * local_size + remainder;

    // Serializacja punktów do tablicy double (x,y)
    std::vector<double> sendbuf;
    if (rank == 0) {
        sendbuf.resize(total_points * 2);
        for (int i = 0; i < total_points; i++) {
            sendbuf[2*i] = all_points[i].x;
            sendbuf[2*i + 1] = all_points[i].y;
        }
    }

    std::vector<double> recvbuf(local_size * 2);

    std::vector<int> counts(size);
    std::vector<int> displs(size);

    for (int i = 0; i < size; i++) {
        counts[i] = ((i < remainder) ? (chunk_size + 1) : chunk_size) * 2;
    }
    displs[0] = 0;
    for (int i = 1; i < size; i++) {
        displs[i] = displs[i-1] + counts[i-1];
    }

    MPI_Scatterv(sendbuf.data(), counts.data(), displs.data(), MPI_DOUBLE,
                 recvbuf.data(), local_size * 2, MPI_DOUBLE,
                 0, MPI_COMM_WORLD);

    // Lokalna obróbka
    for (int i = 0; i < local_size; i++) {
        double x = recvbuf[2*i];
        double y = recvbuf[2*i + 1];
        if (check_point(x, y)) {
            local_matches.push_back({x, y});
        }
    }

    // Zbieranie wyników - najpierw ilość punktów od każdego
    int local_match_count = local_matches.size();
    std::vector<int> all_counts(size);

    MPI_Gather(&local_match_count, 1, MPI_INT,
               all_counts.data(), 1, MPI_INT,
               0, MPI_COMM_WORLD);

    std::vector<int> displs_matches(size, 0);
    int total_matches = 0;
    if (rank == 0) {
        // Mnożymy przez 2 bo każdy punkt to 2 double
        for (int i = 0; i < size; i++) {
            all_counts[i] *= 2;
        }

        // Liczymy przesunięcia w elementach double
        displs_matches[0] = 0;
        for (int i = 1; i < size; i++) {
            displs_matches[i] = displs_matches[i-1] + all_counts[i-1];
        }

        // Sumujemy całkowitą liczbę punktów (nie * 2, bo total_matches to punkty, a nie double)
        for (int i = 0; i < size; i++) {
            total_matches += all_counts[i] / 2;  // dzielimy przez 2, by mieć liczbę punktów
        }
    }

    std::vector<double> send_matches(local_match_count * 2);
    for (int i = 0; i < local_match_count; i++) {
        send_matches[2*i] = local_matches[i].x;
        send_matches[2*i + 1] = local_matches[i].y;
    }

    std::vector<double> recv_matches;
    if (rank == 0) {
        recv_matches.resize(total_matches * 2);
    }

    MPI_Gatherv(send_matches.data(), local_match_count * 2, MPI_DOUBLE,
                recv_matches.data(), all_counts.data(), displs_matches.data(), MPI_DOUBLE,
                0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0) {
        std::cout << "Liczba punktow spelniajacych warunek: " << total_matches << std::endl;
        std::cout << "Czas wykonania: " << (end_time - start_time) << " s\n";
    }

    MPI_Finalize();
    return 0;
}
