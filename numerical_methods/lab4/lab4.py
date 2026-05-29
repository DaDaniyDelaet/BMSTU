import math


EPS = 0.01


def f1(x, y):
    return math.cos(x - 1) + y - 0.8


def f2(x, y):
    return x - math.cos(y) - 2


def jacobian(x, y):
    return [
        [-math.sin(x - 1), 1],
        [1, math.sin(y)]
    ]


def solve_linear_system(a11, a12, b1, a21, a22, b2):
    det = a11 * a22 - a12 * a21
    det_x = b1 * a22 - a12 * b2
    det_y = a11 * b2 - b1 * a21

    dx = det_x / det
    dy = det_y / det

    return dx, dy


def norm(dx, dy):
    return max(abs(dx), abs(dy))


def newton_method(x, y):
    k = 0

    print("k\tx\t\ty\t\tf1(x,y)\t\tf2(x,y)\t\tdx\t\tdy")
    print("-" * 90)

    while True:
        value_f1 = f1(x, y)
        value_f2 = f2(x, y)

        j = jacobian(x, y)

        dx, dy = solve_linear_system(
            j[0][0], j[0][1], -value_f1,
            j[1][0], j[1][1], -value_f2
        )

        print(
            f"{k}\t"
            f"{x:.10f}\t"
            f"{y:.10f}\t"
            f"{value_f1:.10f}\t"
            f"{value_f2:.10f}\t"
            f"{dx:.10f}\t"
            f"{dy:.10f}"
        )

        x = x + dx
        y = y + dy

        k += 1

        if norm(dx, dy) < EPS:
            break

    return x, y, k


x0 = 2.645
y0 = 0.874

x_result, y_result, iterations = newton_method(x0, y0)

print()
print("Результат:")
print(f"x = {x_result:.10f}")
print(f"y = {y_result:.10f}")
print(f"Количество итераций = {iterations}")

print()
print("Проверка:")
print(f"f1(x, y) = {f1(x_result, y_result):.10f}")
print(f"f2(x, y) = {f2(x_result, y_result):.10f}")
