
#include <stdio.h>

char itoa_opt(int x)
{
    switch(x)
    {
        case 1:return '1';
        case 2:return '2';
        case 3:return '3';
        case 4:return '4';
        case 5:return '5';
        case 6:return '6';
        case 7:return '7';
        case 8:return '8';
        case 9:return '9';
        case 0:return '0';
        default: return'x'; 
    }
}

int main()
{
    printf("Hello World\n");
    char x = itoa_opt(2);
    char arr[3] = {itoa_opt(1),itoa_opt(5)};
    printf("x = %c\n", x);
    printf("arr = ");
    printf(arr);
    return 0;
}