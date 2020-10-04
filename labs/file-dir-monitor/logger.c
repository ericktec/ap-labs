#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "logger.h"
#include <syslog.h>
#include <stdarg.h>


#define RESET 0
#define BRIGHT 1
#define DIM 2
#define UNDERLINE 3
#define BLINK 4
#define REVERSE 7
#define HIDDEN 8

#define BLACK 0
#define RED 1
#define GREEN 2
#define YELLOW 3
#define BLUE 4
#define MAGENTA 5
#define CYAN 6
#define WHITE 7

int sysLogMode = 0;

void textcolor(int attr, int fg, int bg)
{
    char command[13];
    sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
    printf("%s", command);
}

int initLogger(char *logType)
{
    if(strcmp(logType, "syslog") == 0)
    {
        printf("Initializing Logger on: %s\n", logType);
        sysLogMode = 1;
    }
    else if( (strcmp(logType,"stdout")==0) || (strcmp(logType, "")==0) ){
        sysLogMode = 0;
    }
    else
    {
        printf("Invalid option, using default stdout mode\n");
        sysLogMode = 0;
    }
    
    return 0;
}

int infof(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    if(sysLogMode == 1)
    {
        openlog("Logger", LOG_PID | LOG_CONS, LOG_CONS);
        vsyslog(LOG_INFO, format, arguments);
        closelog();
    }
    else
    {
        vprintf(format, arguments);
    }
    va_end(arguments);
    return 0;
}
int warnf(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    if(sysLogMode == 1)
    {
        openlog("Logger", LOG_PID | LOG_CONS, LOG_CONS);
        vsyslog(LOG_WARNING, format, arguments);
        closelog();
    }
    else
    {
        textcolor(BRIGHT, YELLOW, BLACK);
        vprintf(format, arguments);
        textcolor(RESET, WHITE, BLACK);
    }
    va_end(arguments);
    
    return 0;
}
int errorf(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    if(sysLogMode == 1)
    {
        openlog("Logger", LOG_PID | LOG_CONS, LOG_CONS);
        vsyslog(LOG_ERR, format, arguments);
        closelog();
    }
    else
    {
        textcolor(RESET, RED, BLACK);
        vprintf(format, arguments);
        textcolor(RESET, WHITE, BLACK);
    }
    va_end(arguments);
    
    return -1;
}
int panicf(const char *format, ...)
{
    va_list arguments;
    va_start(arguments, format);
    if(sysLogMode == 1)
    {
        openlog("Logger", LOG_PID | LOG_CONS, LOG_CONS);
        vsyslog(LOG_EMERG, format, arguments);
        closelog();
    }
    else
    {
        textcolor(BRIGHT, MAGENTA, BLACK);
        vprintf(format, arguments);
        textcolor(RESET, WHITE, BLACK);
    }
    va_end(arguments);
    return -1;
}


