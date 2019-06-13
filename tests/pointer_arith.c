/* Test pointer arithmetic*/

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
    char *s = "abcdefghi";
    char *x = s;
    char *t = x + 1;
    expect(98, *t);
}

int t2()
{
    char *s = "abcdefghi";
    int *x = s;
    char *t = x + 1;
    expect(101, *t);
}

int t3()
{
    char *s = "abcdefghi";
    long *x = s;
    char *t = x + 1;
    expect(105, *t);
}

int t4()
{
    char *s = "abcdefghi";
    void *x = s;
    char *t = x + 1;
    expect(98, *t);
}

int main()
{
    t1();
    t2();
    t3();
    t4();
}
