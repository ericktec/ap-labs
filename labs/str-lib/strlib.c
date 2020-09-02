#include <stdio.h>
#include <stdlib.h>

int mystrlen(char *str)
{
    int tam = 0;
    for (int i = 0; str[i] != '\0'; i++)
    {
        tam++;
    }
    return tam;
}

char *mystradd(char *origin, char *addition)
{
    int i = 0;
    int j = 0;
    char * temp =(char *)malloc(mystrlen(origin)+mystrlen(addition));
    while (origin[i] != '\0')
    {
        temp[i] = origin[i];
        i++;
    }
    while (addition[j] != '\0')
    {
        temp[i] = addition[j];
        i++;
        j++;
    }
    temp[i] = '\0';
    return temp;
}

int mystrfind(char *origin, char *substr)
{
    int subCounter = 0;
    int length = mystrlen(substr);
    int answer = -1;
    for(int i = 0; origin[i]!='\0'; i++)
    {
        if(origin[i] == substr[subCounter])
        {
            if(subCounter == 0){
                answer = i;
            }
            subCounter++;
        }else
        {
            subCounter = 0;
            answer = -1;
        }

        if(subCounter == length){
            return answer;
        }
    }
    return answer;
}


int stringComparison(char *first, char *second)
{
    if(mystrlen(first) == mystrlen(second)){
        for (int i = 0; first[i] != '\0'; i++)
        {
            if(first[i] != second[i]){
                return 0;
            }
        }
        
    }else
    {
        return 0;
    }
    return 1;
    
}


