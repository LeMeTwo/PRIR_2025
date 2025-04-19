import random
import math

def generuj_punkty_mieszane(liczba_punktow, plik_wyj, funkcja_str, procent_na_funkcji=0.5, zakres_x=(0, 10)):
    # eval bezpieczne + math w scope
    local_scope = {"math": math}
    funkcja = eval(f"lambda x: {funkcja_str}", {"__builtins__": None}, local_scope)

    x_min, x_max = zakres_x

    with open(plik_wyj, 'w') as f:
        for _ in range(liczba_punktow):
            x = random.uniform(x_min, x_max)
            if random.random() < procent_na_funkcji:
                try:
                    y = funkcja(x)
                except Exception as e:
                    print(f"Błąd dla x={x}: {e}")
                    continue  # pomijamy ten punkt, jeśli funkcja się wywali
            else:
                y = random.uniform(-100, 100)

            f.write(f"{x} {y}\n")


# Przykład użycia
# Funkcja y = x**2 + 2*x + 1
generuj_punkty_mieszane(
    1500,
    'punkty.txt',
    "x**5 - x**2 + 2*x + 1",
    procent_na_funkcji=0.5,
    zakres_x=(0, 1000)  # ważne dla użycia funkcji np. log albo sin
)