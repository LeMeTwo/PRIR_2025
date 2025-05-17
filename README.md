# Programowanie Równoległe i Rozproszone, PK, Cyberbezpieczństwo 2025.

Projekt na studia II stopnia z programowania równloegłego i rozproszonego.

Skrypt `Generatro_punktów` generuje pewną ilość punktów w zadanej funkcji.
Obsługiwane funkcje to np.:
x**2 + 3*x + 2

> math.sin(x)

> math.log(x + 1)

> abs(x)

> round(x) 

Celem projektu jest porównanie różnych dwóch różnych metod programowania rozproszonego. Jako punkt wyjściowy służy skrypt `Sekwencyjnie.py`, który oblicza jakie punkty leżą na prostej w sposób sekwencyjny.

## Generowanie punktów na prostej:

Ustawiamy `procent_na_funkcji` oraz samą funkcję, np. `"x**5 - x**2 + 2*x + 1"`

## Sprawdzanie w sposób sekwencyjny
Należy ustawić funkcję jaką będziemy sprawdzać w zmiennej `funkcja`. 

# OpenMPI
`mpiexec -n 4 python openmpi.py`
-n przekazuje ile wątków chcemy użyć


# Implementacja w C++
## MPI (np. z OpenMPI)
mpic++ mpi.cpp -o mpi
mpirun -n 4 ./mpi

## OpenMP
g++ openmp.cpp -o openmp -fopenmp
./openmp