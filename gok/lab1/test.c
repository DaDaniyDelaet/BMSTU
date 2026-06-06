int foo(int *a, int n)
{
    int i;
    int s = 0;

    for (i = 0; i < n; i++)
    {
        s = s + a[i];
    }

    if (s > 10)
    {
        s = s * 2;
    }
    else
    {
        s = s - 1;
    }

    return s;
}
