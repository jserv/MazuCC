/* Test pointer */

int expect(int a, int b)
{
    if (!(a == b)) {
        printf("Failed\n");
        printf("  %d expected, but got %d\n", a, b);
        exit(1);
    }
}

int t1()
{
    int a = 61;
    int *b = &a;
    expect(61, *b);
}

int t2()
{
    char *c = "ab";
    expect(97, *c);
}

int t3()
{
    char *c = "ab" + 1;
    expect(98, *c);
}

int t4()
{
    char s[] = "xyz";
    char *c = s + 2;
    expect(122, *c);
}

int t5()
{
    char s[] = "xyz";
    *s = 65;
    expect(65, *s);
}

int t6()
{
    void *x = 1;
    x = x + 3;
    expect(4, x);
}

int main()
{
    t1();
    t2();
    t3();
    t4();
    t5();
    t6();

    return 0;
}
