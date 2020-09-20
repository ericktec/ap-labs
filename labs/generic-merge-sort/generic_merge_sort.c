#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXLINES 5000
void mergeSort(char *lineptr[], int left, int right, int numeric);
void merge(char *lineptr[], int left, int mid, int right, int (*comp)(void*, void*));
int numcmp(char*, char*);

int main(int argc, char *argv[])
{
    FILE * fp;
    int numeric = 0;
    char *input[500];
    if(argc == 2)
    {
        fp = fopen(argv[1], "r");
    }
    else if (argc == 3 && strcmp(argv[1], "-n") == 0)
    {
        fp = fopen(argv[2], "r");
        numeric = 1;
    }
    else
    {
        printf("Error, wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }
    
    
    char * line = NULL;
    size_t len = 0;
    ssize_t read;
    
    if (fp == NULL)
    {
        printf("Error file does not exist");
        exit(EXIT_FAILURE);
    }
    int sizeOfInput = 0;
    while ((read = getline(&line, &len, fp)) != -1) 
    {
        input[sizeOfInput] = malloc(sizeof(char)*len);
        sprintf(input[sizeOfInput], "%s",line);
        input[sizeOfInput][strlen(line)-1]=0;
        sizeOfInput++;
    }

    fclose(fp);
    if (line)
        free(line);

    char* test[] = {"12","11", "13", "5", "6", "7"};
    
    mergeSort(input, 0, sizeOfInput-1, numeric);


    for(int i = 0; i<sizeOfInput; i++)
    {
        printf("%s\n", input[i]);
    }
    
    
    exit(EXIT_SUCCESS);
}



int numcmp(char *s1, char *s2)
{
    double v1, v2;
    v1 = atof(s1);
    v2 = atof(s2);
    if (v1 < v2)
    {
        return -1;
    }
    else if(v1 > v2)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void merge(char* arr[], int left, int mid, int right, int (*comp)(void*, void*)) 
{ 
    int i, j, k; 
    int leftSize = mid - left + 1; 
    int rightSize = right - mid; 

    /* create temp arrays */
    char* L[leftSize]; 
    char* R[rightSize]; 

    for (i = 0; i < leftSize; i++) 
        L[i] = arr[left + i]; 
    for (j = 0; j < rightSize; j++) 
        R[j] = arr[mid + 1 + j]; 

    i = 0; 
    j = 0; 
    k = left; 
    while (i < leftSize && j < rightSize) { 
        if ((*comp)(L[i], R[j])<=0) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 

    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < leftSize) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 

    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < rightSize) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 

void mergeSort(char *arr[], int left, int right, int numeric)
{
    if(left < right)
    {
        int mid = left + (right - left) / 2; 
        mergeSort(arr, left, mid, numeric); 
        mergeSort(arr, mid + 1, right, numeric);
        merge(arr, left, mid, right,(int (*)(void*, void*)) (numeric ? numcmp : strcmp)) ;
    }
}