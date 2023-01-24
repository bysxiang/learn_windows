#include <stdio.h>

void foo();

int main()
{
    foo();
    foo();

    getchar();
    
    return 0;
}

void foo()
{
    static int cx = 5;
    cx++;
    printf("ni hao, cx: %d\n", cx);
}