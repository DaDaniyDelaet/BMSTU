import math
import cmath

N = 128


def f(x):
    return math.exp(x - math.floor(x))


x = []
for j in range(N):
    x.append(j / N)


y = []
for j in range(N):
    y.append((j + 0.5) / N)


f_x = []

for x_j in x:
    f_x.append(f(x_j))


A = []

for q in range(N):

    s = 0 + 0j

    for j in range(N):

        s += f_x[j] * cmath.exp(
            -2j * math.pi * q * x[j]
        )

    A.append(s / N)


def trig_interpolation(t):

    s = 0 + 0j

    for q in range(N):

        s += A[q] * cmath.exp(
            2j * math.pi * q * t
        )

    return s.real


p_y = []

for y_j in y:
    p_y.append(
        trig_interpolation(y_j)
    )


f_y = []

for y_j in y:
    f_y.append(
        f(y_j)
    )


errors = []

for i in range(N):

    errors.append(
        abs(
            f_y[i] - p_y[i]
        )
    )


print("Значения функции в узлах сетки")

print("-" * 55)

print(
f"{'j':>5} | {'x_j':>12} | {'f(x_j)':>15}"
)

print("-" * 55)


for j in range(10):

    print(
        f"{j:5d} | "
        f"{x[j]:12.6f} | "
        f"{f_x[j]:15.6f}"
    )

print("-" * 55)


print("\nСравнение значений функции и интерполяции")

print("-" * 90)

print(
f"{'j':>5} | {'y_j':>12} | {'f(y_j)':>15} | {'P_N(y_j)':>15} | {'Погрешность':>15}"
)

print("-" * 90)


for j in range(10):

    print(
        f"{j:5d} | "
        f"{y[j]:12.6f} | "
        f"{f_y[j]:15.6f} | "
        f"{p_y[j]:15.6f} | "
        f"{errors[j]:15.6f}"
    )


print("-" * 90)

print("\nИтоговые характеристики погрешности")

print("-" * 55)

print(
f"Максимальная погрешность: {max(errors):.6f}"
)

print(
f"Средняя погрешность: {sum(errors)/N:.6f}"
)

print("-" * 55)