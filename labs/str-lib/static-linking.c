#include <stdio.h>

int mystrlen(char *str);
char *mystradd(char *origin, char *addition);
int mystrfind(char *origin, char *substr);
int stringComparison(char *first, char *second);

int main(int argc, char **argv) {
    
    if(argc < 2)
    {
        printf("There is not operation defined \n");
        return 0;
    }
    else if(stringComparison(argv[1], "-add") && argc < 4)
    {
        printf("Failed addition \n");
        return 0;
    }
    else if (stringComparison(argv[1], "-find") && argc < 4)
    {
        printf("Failed find \n");
        return 0;
    }

    if(stringComparison(argv[1], "-add")){
        printf("Initial Length  : %d \n", mystrlen(argv[2]));
        char * newString = mystradd(argv[2], argv[3]);
        printf("New String  : %s \n",newString);
        printf("New Length  : %d \n", mystrlen(newString));
    }else if(stringComparison(argv[1], "-find"))
    {
        printf("[%s] string was found at [%d] position \n", argv[3], mystrfind(argv[2],argv[3]));
    }
    
    return 0;
}






