import math


x_values = [1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0]
y_values = [2.61, 1.62, 1.17, 0.75, 0.30, 0.75, 1.03, 0.87, 0.57]


def solve_system(a11, a12, b1, a21, a22, b2):
    det = a11 * a22 - a12 * a21
    det_a = b1 * a22 - a12 * b2
    det_b = a11 * b2 - b1 * a21

    a = det_a / det
    b = det_b / det

    return a, b


def z(x, a, b):
    return a / x + b


n = len(x_values)

u_values = [1 / x for x in x_values]

sum_u = sum(u_values)
sum_u2 = sum(u ** 2 for u in u_values)
sum_y = sum(y_values)
sum_uy = sum(u_values[i] * y_values[i] for i in range(n))

a, b = solve_system(
    sum_u2, sum_u, sum_uy,
    sum_u, n, sum_y
)

x0 = x_values[0]
xn = x_values[-1]

y0 = y_values[0]
yn = y_values[-1]

x_a = (x0 + xn) / 2
x_g = math.sqrt(x0 * xn)
x_h = 2 / (1 / x0 + 1 / xn)

y_a = (y0 + yn) / 2
y_g = math.sqrt(y0 * yn)
y_h = 2 / (1 / y0 + 1 / yn)

z_values = [z(x, a, b) for x in x_values]
deltas = [abs(z_values[i] - y_values[i]) for i in range(n)]

sum_sq = sum((z_values[i] - y_values[i]) ** 2 for i in range(n))
delta = math.sqrt(sum_sq / n)

print("Исходные данные:")
print("x:", x_values)
print("y:", y_values)

print()
print("Замена u = 1 / x:")
for i in range(n):
    print(f"u{i + 1} = {u_values[i]:.6f}")

print()
print("Суммы для нормальной системы:")
print(f"sum_u  = {sum_u:.6f}")
print(f"sum_u2 = {sum_u2:.6f}")
print(f"sum_y  = {sum_y:.6f}")
print(f"sum_uy = {sum_uy:.6f}")

print()
print("Нормальная система:")
print(f"{sum_u2:.6f} * a + {sum_u:.6f} * b = {sum_uy:.6f}")
print(f"{sum_u:.6f} * a + {n} * b = {sum_y:.6f}")

print()
print("Коэффициенты:")
print(f"a = {a:.6f}")
print(f"b = {b:.6f}")

print()
print("Аппроксимирующая функция:")
print(f"z(x) = {a:.6f} / x + {b:.6f}")

print()
print("Средние значения:")
print(f"x_a = {x_a:.6f}")
print(f"x_g = {x_g:.6f}")
print(f"x_h = {x_h:.6f}")
print(f"y_a = {y_a:.6f}")
print(f"y_g = {y_g:.6f}")
print(f"y_h = {y_h:.6f}")

print()
print("Значения функции в средних точках:")
print(f"z(x_a) = {z(x_a, a, b):.6f}")
print(f"z(x_g) = {z(x_g, a, b):.6f}")
print(f"z(x_h) = {z(x_h, a, b):.6f}")

print()
print("Таблица значений:")
print("i\tx_i\t\ty_i\t\tz(x_i)\t\t|z(x_i) - y_i|")
print("-" * 70)

for i in range(n):
    print(
        f"{i + 1}\t"
        f"{x_values[i]:.6f}\t"
        f"{y_values[i]:.6f}\t"
        f"{z_values[i]:.6f}\t"
        f"{deltas[i]:.6f}"
    )

print()
print("Среднеквадратичное отклонение:")
print(f"sum((z_i - y_i)^2) = {sum_sq:.6f}")
print(f"Delta = {delta:.6f}")
