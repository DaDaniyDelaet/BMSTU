import math
import random

a = 0.75
b = 3.0
eps = 0.001

def f(x):
    return math.exp(1 / x) / (x ** 2)

def exact_integral():
    return math.exp(1 / a) - math.exp(1 / b)

def middle_rectangles(n):
    h = (b - a) / n
    s = 0.0

    for i in range(n):
        x_i = a + (i + 0.5) * h
        s += f(x_i)

    return h * s

def trapezoids(n):
    h = (b - a) / n
    s = (f(a) + f(b)) / 2

    for i in range(1, n):
        x_i = a + i * h
        s += f(x_i)

    return h * s

def simpson(n):
    if n % 2 != 0:
        n += 1

    h = (b - a) / n
    s = f(a) + f(b)

    for i in range(1, n):
        x_i = a + i * h

        if i % 2 == 0:
            s += 2 * f(x_i)
        else:
            s += 4 * f(x_i)

    return h * s / 3

def monte_carlo(N):
    s = 0.0

    for i in range(N):
        r = random.random()
        x_i = a + (b - a) * r
        s += f(x_i)

    return (b - a) * s / N

def print_method_header(method_name):
    print(f"\n{method_name}")
    print("-" * 65)
    print(f"{'n':>8} | {'Значение интеграла':>22} | {'Погрешность':>18}")
    print("-" * 65)

def print_method_row(n, value, error):
    print(f"{n:8d} | {value:22.6f} | {error:18.6f}")

def print_method_footer():
    print("-" * 65)

def main():
    I_exact = exact_integral()

    print("Точное значение интеграла:")
    print(f"I = {I_exact:.6f}")

    print_method_header("Метод средних прямоугольников")

    n = 2
    while True:
        value = middle_rectangles(n)
        error = abs(I_exact - value)

        print_method_row(n, value, error)

        if error <= eps:
            final_middle = (n, value, error)
            break

        n *= 2

    print_method_footer()

    print_method_header("Метод трапеций")

    n = 2
    while True:
        value = trapezoids(n)
        error = abs(I_exact - value)

        print_method_row(n, value, error)

        if error <= eps:
            final_trapezoid = (n, value, error)
            break

        n *= 2

    print_method_footer()

    print_method_header("Метод Симпсона")

    n = 2
    while True:
        value = simpson(n)
        error = abs(I_exact - value)

        print_method_row(n, value, error)

        if error <= eps:
            final_simpson = (n, value, error)
            break

        n *= 2

    print_method_footer()

    random.seed(42)

    print("\nМетод Монте-Карло")
    print("-" * 65)
    print(f"{'N':>8} | {'Значение интеграла':>22} | {'Погрешность':>18}")
    print("-" * 65)

    final_monte = None

    for N in [10, 100, 1000, 10000]:
        value = monte_carlo(N)
        error = abs(I_exact - value)

        print(f"{N:8d} | {value:22.6f} | {error:18.6f}")

        final_monte = (N, value, error)

    print("-" * 65)

    print("\nИтоговое сравнение методов")
    print("-" * 85)
    print(f"{'Метод':>30} | {'Параметр':>10} | {'Значение интеграла':>20} | {'Погрешность':>12}")
    print("-" * 85)

    print(f"{'Средние прямоугольники':>30} | {final_middle[0]:10d} | {final_middle[1]:20.6f} | {final_middle[2]:12.6f}")
    print(f"{'Трапеции':>30} | {final_trapezoid[0]:10d} | {final_trapezoid[1]:20.6f} | {final_trapezoid[2]:12.6f}")
    print(f"{'Симпсон':>30} | {final_simpson[0]:10d} | {final_simpson[1]:20.6f} | {final_simpson[2]:12.6f}")
    print(f"{'Монте-Карло':>30} | {final_monte[0]:10d} | {final_monte[1]:20.6f} | {final_monte[2]:12.6f}")

    print("-" * 85)

main()