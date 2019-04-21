/* Test scope */

int expect(int a, int b)
{
    if (!(a == b)) {
        printf("Failed\n");
        printf("  %d expected, but got %d\n", a, b);
        exit(1);
    }
}

int main()
{
    int a = 31;
    {
        int a = 64;
    }
    expect(31, a);
    {
        int a = 64;
        expect(64, a);
    }

    return 0;
}
