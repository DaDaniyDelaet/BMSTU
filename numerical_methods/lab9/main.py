import math


EPS = 0.001
P = 2

a = math.pi / 6
b = math.pi / 2

c = 0.0
d = 2.0


def phi1(x):
    return 0.0


def phi2(x):
    return 1.0 / math.sin(x)


def f(x, y):
    return y / math.tan(x)


def cell_method(n, m):
    hx = (b - a) / n
    hy = (d - c) / m

    total = 0.0

    for i in range(n):
        x_mid = a + (i + 0.5) * hx

        for j in range(m):
            y_mid = c + (j + 0.5) * hy

            if phi1(x_mid) <= y_mid <= phi2(x_mid):
                total += f(x_mid, y_mid)

    return hx * hy * total


h0 = math.sqrt(EPS)

n = math.ceil((b - a) / h0)
m = math.ceil((d - c) / h0)

s_old = cell_method(n, m)

while True:
    n_new = 2 * n
    m_new = 2 * m

    s_new = cell_method(n_new, m_new)

    runge = abs(s_new - s_old) / (2 ** P - 1)

    if runge <= EPS:
        break

    n = n_new
    m = m_new
    s_old = s_new

print(f"I ≈ {s_new}")
print(f"n = {n_new}")
print(f"m = {m_new}")
print(f"Runge = {runge}")