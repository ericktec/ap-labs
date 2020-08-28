#include <stdio.h>

void reverse(int lenght, char arr[])
{

    int i, tmp;

    for (i = 0; i < lenght / 2; i++)
    {
        tmp = arr[i];
        arr[i] = arr[lenght - i - 1];
        arr[lenght - i - 1] = tmp;
    }
    for (int i = 0; i < lenght; ++i)
        putchar(arr[i]);
    
    putchar('\n');
}

int main(void)
{
    char str[10090];
    int ch, length = 0;

    while ((ch = getchar()) != EOF && length < 1000)
    {
        if (ch == '\n')
        {
            reverse(length, str);
            length = 0;
        }
        else
        {
            str[length] = ch;
            ++length;
        }
    }

    return 0;
}