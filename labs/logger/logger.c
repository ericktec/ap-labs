
#include <stdio.h>
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


void textcolor(int attr, int fg, int bg)
{
  char command[13];
  sprintf(command, "%c[%d;%d;%dm", 0x1B, attr, fg + 30, bg + 40);
  printf("%s", command);
}

int infof(const char *format, ...)
{
  va_list arguments;
  va_start(arguments, format);
  vprintf(format, arguments);
  va_end(arguments);
  return 0;
}
int warnf(const char *format, ...)
{
  textcolor(BRIGHT, YELLOW, BLACK);
  va_list arguments;
  va_start(arguments, format);
  vprintf(format, arguments);
  va_end(arguments);
  textcolor(RESET, WHITE, BLACK);
  return 0;
}
int errorf(const char *format, ...)
{
  textcolor(RESET, RED, BLACK);
  va_list arguments;
  va_start(arguments, format);
  vprintf(format, arguments);
  va_end(arguments);
  textcolor(RESET, WHITE, BLACK);
  return -1;
}
int panicf(const char *format, ...)
{
  textcolor(BRIGHT , MAGENTA, BLACK);
  va_list arguments;
  va_start(arguments, format);
  vprintf(format, arguments);
  va_end(arguments);
  textcolor(RESET, WHITE, BLACK);
  return -1;
}