#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "logger.h"
#include <signal.h>

#define WHITESPACE 64
#define EQUALS 65
#define INVALID 66

static const unsigned char d[] = {
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 64, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 62, 66, 66, 66, 63, 52, 53,
    54, 55, 56, 57, 58, 59, 60, 61, 66, 66, 66, 65, 66, 66, 66, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,
    10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 66, 66, 66, 66, 66, 66, 26, 27, 28,
    29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66,
    66, 66, 66, 66, 66, 66};

char *readcontent(char *filename);
int base64encode(const void *data_buf, size_t dataLength, char *result, size_t resultSize);
int base64decode(char *in, size_t inLen, unsigned char *out, size_t *outLen);
void sigHandler(int sign);

long double progress = 0;
long double filesize = 1;

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        return errorf("Wrong input, please introduce the flag --encode || --decode and the file's name\n");
    }
    signal(SIGINT, sigHandler);
    signal(SIGUSR1, sigHandler);

    if (strcmp("--decode", argv[1]) == 0)
    {
        char *content = readcontent(argv[2]);
        if (content == NULL)
        {
            return errorf("Error reading the file\n");
        }
        int size = 8 * strlen(content);
        char *decoded = malloc(size);
        base64decode(content, strlen(content), decoded, &size);
        char *newFileName = argv[2];
        newFileName[strlen(newFileName)-4] = '\0';
        strcat(newFileName, "-decoded.txt");
        writeContent(newFileName, decoded);
    }
    else if (strcmp("--encode", argv[1])==0)
    {
        char *content = readcontent(argv[2]);
        if (content == NULL)
        {
            return errorf("Error reading the file\n");
        }
        int size = 8 * strlen(content);
        char *encode = malloc(size);
        base64encode(content, strlen(content), encode, size);
        char *newFileName = argv[2];
        newFileName[strlen(newFileName)-4] = '\0';
        strcat(newFileName, "-encoded.txt");
        writeContent(newFileName, encode);
    
    }else
    {
        return errorf("Wrong flag \n");
    }
    
    return 0;
}

char *readcontent(char *filename)
{
    char *fcontent = NULL;
    int fsize = 0;
    FILE *fp;

    fp = fopen(filename, "r");
    if (fp)
    {
        fseek(fp, 0, SEEK_END);
        fsize = ftell(fp);
        filesize = fsize;
        rewind(fp);

        fcontent = (char *)malloc(sizeof(char) * fsize);
        fread(fcontent, 1, fsize, fp);

        fclose(fp);
    }
    return fcontent;
}

int writeContent(char *filename, char *content)
{
    FILE *fp;
    int i;

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        return errorf("Error creating or writing the file\n");
    }

    fprintf(fp, "%s", content);

    fclose(fp);
    return 1;
}

int base64encode(const void *data_buf, size_t dataLength, char *result, size_t resultSize)
{
    const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    const uint8_t *data = (const uint8_t *)data_buf;
    size_t resultIndex = 0;
    size_t x;
    uint32_t n = 0;
    int padCount = dataLength % 3;
    uint8_t n0, n1, n2, n3;

    for (x = 0; x < dataLength; x += 3)
    {
        
        progress=x;

        n = ((uint32_t)data[x]) << 16; 

        if ((x + 1) < dataLength)
            n += ((uint32_t)data[x + 1]) << 8; 
        if ((x + 2) < dataLength)
            n += data[x + 2];


        n0 = (uint8_t)(n >> 18) & 63;
        n1 = (uint8_t)(n >> 12) & 63;
        n2 = (uint8_t)(n >> 6) & 63;
        n3 = (uint8_t)n & 63;


        if (resultIndex >= resultSize)
            return 1; 
        result[resultIndex++] = base64chars[n0];
        if (resultIndex >= resultSize)
            return 1; 
        result[resultIndex++] = base64chars[n1];

 
        if ((x + 1) < dataLength)
        {
            if (resultIndex >= resultSize)
                return 1; 
            result[resultIndex++] = base64chars[n2];
        }


        if ((x + 2) < dataLength)
        {
            if (resultIndex >= resultSize)
                return 1; 
            result[resultIndex++] = base64chars[n3];
        }
        // sleep(0.5);  // Used just to have time to send the signals using another terminal
    }


    if (padCount > 0)
    {
        for (; padCount < 3; padCount++)
        {
            if (resultIndex >= resultSize)
                return 1; 
            result[resultIndex++] = '=';
        }
    }
    if (resultIndex >= resultSize)
        return 1; 
    result[resultIndex] = 0;
    return 0; 
}

int base64decode(char *in, size_t inLen, unsigned char *out, size_t *outLen)
{
    char *end = in + inLen;
    char iter = 0;
    uint32_t buf = 0;
    size_t len = 0;
    while (in < end)
    {
        progress +=1;
        unsigned char c = d[*in++];

        switch (c)
        {
        case WHITESPACE:
            continue;
        case INVALID:
            return 1; 
        case EQUALS:  
            in = end;
            continue;
        default:
            buf = buf << 6 | c;
            iter++; 

            if (iter == 4)
            {
                if ((len += 3) > *outLen)
                    return 1; 
                *(out++) = (buf >> 16) & 255;
                *(out++) = (buf >> 8) & 255;
                *(out++) = buf & 255;
                buf = 0;
                iter = 0;
            }
        }
        // sleep(0.5);  // Used just to have time to send the signals using another terminal
    }

    if (iter == 3)
    {
        if ((len += 2) > *outLen)
            return 1; 
        *(out++) = (buf >> 10) & 255;
        *(out++) = (buf >> 2) & 255;
    }
    else if (iter == 2)
    {
        if (++len > *outLen)
            return 1; 
        *(out++) = (buf >> 4) & 255;
    }

    *outLen = len; 
    return 0;
}

void sigHandler(int sign)
{
    infof("Progress %Lf % \n",(progress/filesize)*100);
}