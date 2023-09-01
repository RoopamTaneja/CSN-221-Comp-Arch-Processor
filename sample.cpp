bool f()
{
    int a = 15;
    int b = 2;
    while (b < a)
    {
        if (a % b == 0)
            return false;

        b++;
    }
    return true;
}