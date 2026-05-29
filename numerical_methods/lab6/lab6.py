import math


EPS = 0.001


def f(x1, x2):
    return math.sqrt(x1 ** 2 + x2 ** 2 + 1) + x1 / 2 + x2 / 2


def grad_f(x1, x2):
    d = math.sqrt(x1 ** 2 + x2 ** 2 + 1)

    df_dx1 = x1 / d + 1 / 2
    df_dx2 = x2 / d + 1 / 2

    return df_dx1, df_dx2


def second_derivatives(x1, x2):
    d = math.sqrt(x1 ** 2 + x2 ** 2 + 1)
    d3 = d ** 3

    f_xx = (x2 ** 2 + 1) / d3
    f_yy = (x1 ** 2 + 1) / d3
    f_xy = -x1 * x2 / d3

    return f_xx, f_xy, f_yy


def gradient_norm(g1, g2):
    return max(abs(g1), abs(g2))


def find_step(x1, x2, g1, g2):
    f_xx, f_xy, f_yy = second_derivatives(x1, x2)

    phi_first = -(g1 ** 2 + g2 ** 2)
    phi_second = f_xx * g1 ** 2 + 2 * f_xy * g1 * g2 + f_yy * g2 ** 2

    return -phi_first / phi_second


def steepest_descent(x1, x2):
    k = 0

    print("k\tx1\t\tx2\t\tf(x)\t\t||grad f||")
    print("-" * 65)

    while True:
        g1, g2 = grad_f(x1, x2)
        norm = gradient_norm(g1, g2)

        print(f"{k}\t{x1:.6f}\t{x2:.6f}\t{f(x1, x2):.6f}\t{norm:.6f}")

        if norm < EPS:
            break

        t = find_step(x1, x2, g1, g2)

        x1 = x1 - t * g1
        x2 = x2 - t * g2

        k += 1

    return x1, x2, f(x1, x2)


x1_start = 0.0
x2_start = 0.0

x1_min, x2_min, f_min = steepest_descent(x1_start, x2_start)

print()
print("Численное решение:")
print(f"x1 = {x1_min:.6f}")
print(f"x2 = {x2_min:.6f}")
print(f"f(x) = {f_min:.6f}")

print()
print("Аналитическое решение:")
x_exact = -1 / math.sqrt(2)
f_exact = 1 / math.sqrt(2)

print(f"x1 = {x_exact:.6f}")
print(f"x2 = {x_exact:.6f}")
print(f"f(x) = {f_exact:.6f}")

print()
print("Погрешность:")
print(f"|dx1| = {abs(x1_min - x_exact):.6f}")
print(f"|dx2| = {abs(x2_min - x_exact):.6f}")
print(f"|df| = {abs(f_min - f_exact):.6f}")

