#include <stdio.h>
#include <stdlib.h>

void month_day(int year, int yearday, int *pmonth, int *pday);

int main(int argc, char **argv)
{

    if (!(argv[1] != NULL && argv[2] != NULL))
    {
        printf("Not data");
        return 0;
    }

    char monthsNames[13][10] = {
        "0",
        "January",
        "February",
        "March",
        "April",
        "May",
        "June",
        "July",
        "August",
        "September",
        "October",
        "November",
        "December"};

    int day = 0;
    int month = 0;
    int *pmonth = &month;
    int *pday = &day;

    int year = atoi(argv[1]);
    int yearDay = atoi(argv[2]);

    month_day(year, yearDay, pmonth, pday);
    printf("%s %d, %d\n", monthsNames[*pmonth], *pday, year);
    return 0;
}

int validator(int year, int yearday, int leap)
{
    if (year <= 0 || yearday <= 0)
    {
        return 0;
    }
    else if (leap == 0 && yearday > 365)
    {
        return 0;
    }
    else if (leap == 1 && yearday > 366)
    {
        return 0;
    }
    return 1;
}

void month_day(int year, int yearday, int *pmonth, int *pday)
{
    char daytab[2][13] = {{0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}, {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};
    int i, leap;
    leap = year % 4 == 0 && year % 100 != 0 || year % 400 == 0;

    if (!validator(year, yearday, leap))
    {
        printf("Invalid data\n");
        return;
    }

    for (i = 1; i < 13; i++)
    {
        if (yearday > daytab[leap][i])
        {
            yearday -= daytab[leap][i];
        }
        else
        {
            *pmonth = i;
            *pday = yearday;
            return;
        }
    }
}
