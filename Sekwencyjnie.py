import time

def sprawdz_punkty_na_funkcji(plik_punktow, funkcja_str, tolerancja=0.001):
    funkcja = eval(f"lambda x: {funkcja_str}")
    pasujace = []
    niepasujace = []

    start_time = time.time()

    with open(plik_punktow, 'r') as f:
        for linia in f:
            x_str, y_str = linia.strip().split()
            x = float(x_str)
            y = float(y_str)
            try:
                y_obliczone = funkcja(x)
                if abs(y - y_obliczone) <= tolerancja:
                    pasujace.append((x, y))
                else:
                    niepasujace.append((x, y))
            except Exception as e:
                print(f"Błąd przy x={x}: {e}")
                niepasujace.append((x, y))

    end_time = time.time()
    czas_wykonania = end_time - start_time

    print(f"\n⏱️ Czas wykonania: {czas_wykonania:.6f} sekundy")

    return pasujace, niepasujace

# Przykład użycia
funkcja = "x**5 - x**2 + 2*x + 1"
pasuje, niepasuje = sprawdz_punkty_na_funkcji('punkty.txt', funkcja)

print("✅ Pasujące punkty:")
for p in pasuje:
    print(p)

print("\n❌ Niepasujące punkty:")
for p in niepasuje:
    print(p)
