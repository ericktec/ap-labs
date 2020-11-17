#include <stdio.h>
#include "logger.h"
#include <pthread.h>
#include <string.h>
#include <stdlib.h>

//Important modify this variables depending on dimension of matrix NxN
#define NUMBERS_PER_COLUMN 2000 
#define NUMBERS_PER_ROW 2000

long *readMatrix(char *filename);
long *getColumn(int col, long *matrix);
long *getRow(int row, long *matrix);
int getLock();
int releaseLock(int lock);
long dotProduct(long *vec1, long *vec2);
long *multiply(long *matA, long *matB);
int saveResultMatrix(long *result);
void printMatrix(long* matirx);


//Variables
pthread_t threads[NUMBERS_PER_ROW];
long NUM_BUFFERS;
long **buffers;
pthread_mutex_t *mutexes;
long *result;
char *fileName;
long *matrixA;
long *matrixB;


struct indexes
{
    int column;
    int row;
};

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        return errorf("Wrong parameters");
    }
    else
    {
        if (strcmp(argv[1], "-n") == 0 && strcmp(argv[3], "-out") == 0)
        {
            NUM_BUFFERS = atoi(argv[2]);
            fileName = argv[4];
        }
        else if (strcmp(argv[1], "-out") == 0 && strcmp(argv[3], "-n") == 0)
        {
            NUM_BUFFERS = atoi(argv[4]);
            fileName = argv[2];
        }
        else
        {
            return errorf("wrong flags");
        }
    }
    mutexes = malloc(NUM_BUFFERS * sizeof(pthread_mutex_t));
    for (int i = 0; i < NUM_BUFFERS; i++)
    {
        pthread_mutex_init(&mutexes[i], NULL);
    }
    buffers = malloc(NUM_BUFFERS * sizeof(long *));
    
    long *matrix1;
    long *matrix2;
    matrix1 = readMatrix("matA.dat");
    matrix2 = readMatrix("matB.dat");
    long *answer = multiply(matrix1, matrix2);
    pthread_exit(NULL);
    return 0;
}

long *readMatrix(char *fileName)
{
    FILE *fp;
    fp = fopen(fileName, "r");
    char line[20];
    long lines = 0;
    while (!feof(fp))
    {
        fgets(line, 8, fp);
        lines++;
    }
    fseeko(fp, 0, SEEK_SET);
    long *matrix = malloc(lines * sizeof(long));
    long counter = 0;
    while (!feof(fp))
    {
        fgets(line, 8, fp);
        matrix[counter] = atol(line);
        counter++;
    }
    fclose(fp);


    return matrix;
}

long *getColumn(int col, long *matrix)
{
    long *column = malloc(NUMBERS_PER_COLUMN * sizeof(long));
    // infof("column\n");
    for (int i = 0; i < NUMBERS_PER_COLUMN; i++)
    {
        column[i] = matrix[i * NUMBERS_PER_COLUMN + col];
        // infof("%ld ", column[i]);
    }
    // infof("\n");
    return column;
}

long *getRow(int row, long *matrix)
{
    // infof("row \n");
    long *rowTemp = malloc(NUMBERS_PER_ROW * sizeof(long));
    for (int i = 0; i < NUMBERS_PER_ROW; i++)
    {
        rowTemp[i] = matrix[i + NUMBERS_PER_ROW * row];
        // infof("%ld ", rowTemp[i]);
    }
    // infof("\n");
    return rowTemp;
}

void printMatrix(long *matrix)
{
    infof("\n Matriz \n");
    for (int i = 0; i < NUMBERS_PER_COLUMN * NUMBERS_PER_ROW; i++)
    {
        if (i % NUMBERS_PER_COLUMN == 0 && i != 0)
        {
            infof("\n");
        }
        infof("%ld ", matrix[i]);
    }
    infof("\n");
}

int getLock()
{
    for (int i = 0; i < NUM_BUFFERS; i++)
    {
        if (pthread_mutex_trylock(&mutexes[i] )== 0)
        {
            return i;
        }
    }
    return -1;
}

int releaseLock(int lock)
{
    if(pthread_mutex_unlock(&mutexes[lock])==0)
    {
        return 0;
    }
    return -1;
}

long dotProduct(long *vec1, long *vec2)
{
    long answer = 0;
    for (int i = 0; i < NUMBERS_PER_ROW; i++)
    {
        answer += vec1[i] * vec2[i];
    }
    return answer;
}



long threadMultiplication(struct indexes *roxXcolumn)
{
    int index1 = -1;
    int index2 = -1;
    long attempts = 0;
    while (index1 == -1 || index2 == -1)
    {
        attempts++;
        if(index1 == -1){
            index1=getLock();
        }
        if(index2 == -1){
            index2 = getLock();
        }
        if(attempts==3000)
        {
            if(index1 != -1 && index2 == -1){
                releaseLock(index1);
            }
            if(index2 != -1 && index1 == -1){
                releaseLock(index2);
            }
            attempts = 0;
        }
    }

    buffers[index1] = getRow(roxXcolumn->row, matrixA);
    buffers[index2] = getColumn(roxXcolumn->column, matrixB);
    long answer = dotProduct(buffers[index1], buffers[index2]);
    free(roxXcolumn);
    if(releaseLock(index1) == -1)
    {
        errorf("something went wrong with the lock 1\n");
        exit(-1);
    }
    if(releaseLock(index2) == -1)
    {
        errorf("something went wrong with the lock 2\n");
        exit(-1);
    }
    return answer;
    // pthread_exit(answer);
    
}

long *multiply(long *matA, long *matB)
{
    long *answer = malloc((NUMBERS_PER_COLUMN*NUMBERS_PER_ROW)*sizeof(long));
    matrixA = matA;
    matrixB = matB;
    for (int row = 0; row < NUMBERS_PER_ROW; row++)
    {
        for (int column = 0; column < NUMBERS_PER_COLUMN; column++)
        {
            struct indexes *rowXcolumn = malloc(sizeof(struct indexes));
            rowXcolumn->row = row;
            rowXcolumn->column = column;
            int rc = pthread_create(&threads[column], NULL, threadMultiplication, (void *)rowXcolumn);
            if (rc)
            {
                errorf("ERROR; return code from pthread_create() is %d\n", rc);
                exit(-1);
            }
            
        }
        //To get position of the array apply this formula lenght of rows * current row + position
        for(int position = 0; position<NUMBERS_PER_ROW; position++)
        {
            void* dotProduct;
            pthread_join(threads[position], &dotProduct);
            answer[NUMBERS_PER_ROW*row+position] = (long)dotProduct;
        }
    }
    saveResultMatrix(answer);
    return answer;
}


int saveResultMatrix(long *matrix)
{
    FILE *fp;
    fp = fopen(fileName, "w");
    for(int i = 0; i<NUMBERS_PER_ROW*NUMBERS_PER_COLUMN; i++)
    {
        fprintf(fp, "%ld\n", matrix[i]);
    }
}

