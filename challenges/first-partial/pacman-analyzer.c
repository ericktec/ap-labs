#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>


#define REPORT_FILE "packages_report.txt"


struct Package
{
    char *name;
    char *installDate;
    char *lastUpdate;
    int totalUpdates;
    char *removeDate;
    struct Package *next;
};

void analizeLog(char *logFile, char *report);
void printPackages(struct Package *node, int fd2);
void addReport(struct Package **head_ref, char *name, char *date, char *type);



int main(int argc, char **argv)
{

    if (argc < 4)
    {
        printf("Usage:./pacman-analizer.o \n");
        return 1;
    }
    analizeLog(argv[2], argv[4]);

    return 0;
}

void analizeLog(char *logFile, char *report)
{
    printf("Generating Report from: [%s] log file\n", logFile);

    // Implement your solution here.

    //Setup
    struct Package *head = NULL;
    char buffer[1];
    int fd;

    //Counters
    int contador = -1;
    int bytesReaded = 0;
    int installed = 0;
    int updated = 0;
    int removed = 0;
    int lineas = 0;
    //Strings
    char *date = malloc(sizeof(char) * 20);
    char *type = malloc(sizeof(char) * 20);
    char *name = malloc(sizeof(char) * 60);

    //Boolean
    fd = open(logFile, O_RDONLY);

    if (fd == -1)
    {
        printf("File does not exist\n");
        return;
    }

    while (bytesReaded = read(fd, buffer, 1) > 0)
    {
        if (*buffer == '[' && contador == -1)
        {
            contador++;
        }
        else if (*buffer != ']' && contador == 0)
        {
            strcat(date, &buffer[0]);
        }
        else if (*buffer == ']' && contador == 0)
        {
            contador++;
        }
        else if (*buffer == '[' && contador == 1)
        {
            contador++;
        }
        else if (*buffer == ']' && contador == 2)
        {
            contador++;
        }
        else if (*buffer == ' ' && contador == 3)
        {
            contador++;
        }
        else if (*buffer != ' ' && contador == 4)
        {
            strcat(type, &buffer[0]);
            // printf("%s\n", type);
        }
        else if (*buffer == ' ' && contador == 4)
        {
            contador++;
        }
        else if (*buffer != ' ' && contador == 5)
        {
            strcat(name, &buffer[0]);
        }
        else if (*buffer == ' ')
        {
            contador++;
        }

        if (*buffer == '\n')
        {
            lineas++;
            if (strcmp(type, "installed") == 0)
            {
                addReport(&head, name, date, type);
                installed++;
            }
            else if (strcmp(type, "upgraded") == 0)
            {
                addReport(&head, name, date, type);
                updated++;
            }
            else if (strcmp(type, "removed") == 0)
            {
                addReport(&head, name, date, type);
                removed++;
            }

            type[0] = '\0';
            date = malloc(sizeof(char) * 20);
            name = malloc(sizeof(char) * 50);

            contador = -1;
        }
    }

    close(fd);

    int current = installed - removed;

    printf("Pacman Packages Report\n");
    printf("----------------------\n");
    printf("- Installed packages : %d\n", installed);
    printf("- Removed packages   : %d\n", removed);
    printf("- Upgraded packages  : %d\n", updated);
    printf("- Current installed  : %d\n", current);
    printf("- Numero de lienas : %d\n", lineas);

    int fd2;

    if ((fd2 = open(report, O_WRONLY | O_CREAT, 0644)) == -1)
    {
        printf("Something went wrong while creating the new file\n");
    }
    else
    {
        char *temp = malloc(sizeof(char) * 5);
        write(fd2, "Pacman Packages Report\n", 23);
        write(fd2, "----------------------\n", 23);
        write(fd2, "- Installed packages : ", 23);
        sprintf(temp, "%d\n", installed);
        write(fd2, temp, strlen(temp));
        write(fd2, "- Removed packages   : ", 23);
        sprintf(temp, "%d\n", removed);
        write(fd2, temp, strlen(temp));
        write(fd2, "- Upgraded packages  : ", 23);
        sprintf(temp, "%d\n", updated);
        write(fd2, temp, strlen(temp));
        write(fd2, "- Current installed  : ", 23);
        sprintf(temp, "%d\n", current);
        write(fd2, temp, strlen(temp));
        printPackages(head, fd2);
        free(temp);
        close(fd2);
    }
    printf("Report is generated at: [%s]\n", report);
}

void addReport(struct Package **head_ref, char *name, char *date, char *type)
{

    struct Package *newPackage = (struct Package *)malloc(sizeof(struct Package));

    struct Package *last = *head_ref;

    newPackage->next = NULL;

    if (*head_ref == NULL)
    {
        if (strcmp(type, "installed") == 0)
        {
            newPackage->name = name;
            newPackage->installDate = date;
            newPackage->lastUpdate = "-";
            newPackage->removeDate = "-";
            newPackage->totalUpdates = 0;
        }
        else if (strcmp(type, "upgraded") == 0)
        {
            newPackage->name = name;
            newPackage->installDate = date;
            newPackage->lastUpdate = date;
            newPackage->removeDate = "-";
            newPackage->totalUpdates += 1;
        }
        else if (strcmp(type, "removed") == 0)
        {
            newPackage->name = name;
            newPackage->installDate = date;
            newPackage->lastUpdate = "-";
            newPackage->removeDate = date;
            newPackage->totalUpdates = 0;
        }
        *head_ref = newPackage;
        return;
    }

    while (last->next != NULL)
    {
        if (strcmp(last->name, name) == 0)
        {
            if (strcmp(type, "installed") == 0)
            {
                last->installDate = date;
            }
            else if (strcmp(type, "upgraded") == 0)
            {

                last->lastUpdate = date;
                last->totalUpdates += 1;
            }
            else if (strcmp(type, "removed") == 0)
            {
                last->removeDate = date;
            }
            return;
        }
        last = last->next;
    }

    if (strcmp(last->name, name) == 0)
    {
        if (strcmp(type, "installed") == 0)
        {
            last->installDate = date;
        }
        else if (strcmp(type, "upgraded") == 0)
        {

            last->lastUpdate = date;
            last->totalUpdates += 1;
        }
        else if (strcmp(type, "removed") == 0)
        {
            last->removeDate = date;
        }
        return;
    }
    else
    {
        if (strcmp(type, "installed") == 0)
        {
            newPackage->name = name;
            newPackage->installDate = date;
            newPackage->lastUpdate = "-";
            newPackage->removeDate = "-";
            newPackage->totalUpdates = 0;
        }
        else if (strcmp(type, "upgraded") == 0)
        {
            newPackage->name = name;
            newPackage->installDate = date;
            newPackage->lastUpdate = date;
            newPackage->removeDate = "-";
            newPackage->totalUpdates += 1;
        }
        else if (strcmp(type, "removed") == 0)
        {
            newPackage->name = name;
            newPackage->installDate = date;
            newPackage->lastUpdate = "-";
            newPackage->removeDate = date;
            newPackage->totalUpdates = 0;
        }
        last->next = newPackage;
        return;
    }
}

void printPackages(struct Package *node, int fd2)
{
    
    while (node != NULL)
    {
        char *temp = malloc(sizeof(char) * 5);
        sprintf(temp, "%d", node->totalUpdates);
        printf("List of packages\n");
        printf("- Package Name        : %s\n", node->name);
        printf("----------------\n");
        printf("- Install date      : %s\n", node->installDate);
        printf("- Last update date  : %s\n", node->lastUpdate);
        printf("- How many updates  : %d\n", node->totalUpdates);
        printf("- Removal date      : %s\n", node->removeDate);


        write(fd2, "List of packages\n", 17);
        write(fd2, "- Package Name        : ", 24);
        write(fd2, node->name, strlen(node->name));
        write(fd2, "\n", 1);
        write(fd2, "----------------\n", 17);
        write(fd2, "- Install date      : ", 22);
        write(fd2, node->installDate, strlen(node->installDate));
        write(fd2, "\n", 1);
        write(fd2, "- Last update date  : %s\n", 22);
        write(fd2, node->lastUpdate, strlen(node->lastUpdate));
        write(fd2, "\n", 1);
        write(fd2, "- How many updates  : ", 22);
        write(fd2, temp, strlen(temp));
        write(fd2, "\n", 1);
        write(fd2, "- Removal date      : ", 22);
        write(fd2, node->removeDate, strlen(node->removeDate));
        write(fd2, "\n", 1);

        node = node->next;
    }
    
}
