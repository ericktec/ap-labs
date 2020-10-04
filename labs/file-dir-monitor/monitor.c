#include <stdio.h>
#include <stdlib.h>
#include "logger.h"
#include <string.h>
#include <limits.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <ftw.h>
#include "uthash.h"

#define BUF_LEN (10 * (sizeof(struct inotify_event) + NAME_MAX + 1))

int inotifyFd;
char *rootPath;
char *oldPath;
int rootId;


void handler(struct inotify_event *event);
int createWatcher(char *path);
int addFolder(const char *fpath, const struct stat *sb, int typeflag);
void updateFolder(char *oldPath, char* newPath);


struct Watchers
{
    int id;
    char path[NAME_MAX + 1];
    UT_hash_handle hh;
};

struct FTW {
    int base;
    int level;
};

struct Watchers *watchers = NULL;

int main(int argc, char *argv[])
{
    // Place your magic here
    if (argc != 2)
    {
        errorf("Wrong number of parameters\n");
        exit(EXIT_FAILURE);
    }

    int wd;
    inotifyFd = inotify_init();
    char buf[BUF_LEN] __attribute__((aligned(8)));
    ssize_t numRead;
    char *p;
    struct inotify_event *event;

    if (inotifyFd < 0)
    {
        errorf("Something went wrong initializing notify\n");
        exit(EXIT_FAILURE);
    }

    infof("Starting File/Directory Monitor on %s \n -----------------------------------------------------\n", argv[1]);
    rootPath = argv[1];
    oldPath = rootPath;
    wd = inotify_add_watch(inotifyFd, argv[1], IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
    if (wd == -1)
    {
        errorf("Error in the path given \n");
        exit(EXIT_FAILURE);
    }
    struct Watchers *s;

    s = malloc(sizeof(struct Watchers));
    s->id = wd;
    rootId = wd;
    strcpy(s->path, rootPath);
    HASH_ADD_INT(watchers, id, s);

    struct FTW *flag = malloc(sizeof(struct FTW));
    flag->base = 0;
    flag->level = 1;

    if (nftw(argv[1], addFolder, 20, NULL,  flag) == -1)
    {
        errorf("Something went wrong while creating the watcher.\n");
    }

    
    for (;;)
    { /* Read events forever */
        numRead = read(inotifyFd, buf, BUF_LEN);
        if (numRead == 0)
            return (panicf("Error getting information from the buffer \n"));

        if (numRead == -1)
        {
            errorf("Something went wrong while reading the events\n");
        }

        for (p = buf; p < buf + numRead;)
        {
            event = (struct inotify_event *)p;
            handler(event);

            p += sizeof(struct inotify_event) + event->len;
        }
    }

    inotify_rm_watch(inotifyFd, wd);
    close(inotifyFd);

    exit(EXIT_SUCCESS);
}

void handler(struct inotify_event *event)
{
    
    if (event->mask & IN_CREATE)
    {
        struct Watchers *s;
        HASH_FIND_INT(watchers, &event->wd, s);
        if (event->mask & IN_ISDIR)
        {
            char *path = s->path;
            strcat(path, "/");
            strcat(path, event->name);
            int wd = createWatcher(path);
            struct Watchers *n;

            n = malloc(sizeof(struct Watchers));
            n->id = wd;
            strcpy(n->path, path);
            HASH_ADD_INT(watchers, id, n);
            infof("- [Directory - Create] - %s\n", path);
        }
        else
        {
            
            if (rootId == s->id)
            {
                infof("- [File - Create] - %s\n", event->name);
            }
            else
            {
                infof("- [File - Create] - %s/ %s\n", s->path, event->name);
            }
        }
    }
    if (event->mask & IN_DELETE)
    {
        if (event->mask & IN_ISDIR)
        {
            struct Watchers *s;
            HASH_FIND_INT(watchers, &event->wd, s);
            infof("- [Directory - Removal] - %s\n", event->name);
            if (rootId != s->id)
            {
                inotify_rm_watch(inotifyFd, event->wd);
            }
        }
        else
        {
            infof("- [File - Removal] - %s\n", event->name);
        }
    }
    if (event->mask & IN_MOVED_FROM)
    {
        struct Watchers *s;
        HASH_FIND_INT(watchers, &event->wd, s);
        
        if (event->mask & IN_ISDIR)
        {
            infof("- [Directory Rename] %s/ %s", s->path,event->name);
            strcpy(oldPath, s->path);
            strcat(oldPath, "/");
            strcat(oldPath, event->name);
        }
        else
        {
            infof("- [File Rename] %s/ %s", s->path,event->name);
        }
    }
    if (event->mask & IN_MOVED_TO)
    {
        struct Watchers *s;
        HASH_FIND_INT(watchers, &event->wd, s);
        if (event->mask & IN_ISDIR)
        {
            char *newPath = s->path;
            strcat(newPath, "/");
            strcat(newPath, event->name);
            warnf("Old path %s, newPath %s current id %d\n", oldPath, newPath, event->wd);
            updateFolder(oldPath, newPath);
            strcpy(oldPath, "");
        }

        infof("-> %s/ %s\n", s->path, event->name);
    }
}

int createWatcher(char *path)
{
    int wd = inotify_add_watch(inotifyFd, path, IN_CREATE | IN_DELETE | IN_MOVED_FROM | IN_MOVED_TO);
    return wd;
}

int addFolder(const char *fpath, const struct stat *sb, int typeflag)
{
    if (typeflag == FTW_D)
    {
        int wd = createWatcher(fpath);
        if(wd == rootId){
            return 0;
        }
        struct Watchers *s;
        s = malloc(sizeof(struct Watchers));
        s->id = wd;
        strcpy(s->path, fpath);
        HASH_ADD_INT(watchers, id, s);
    }
    return 0;
}


void updateFolder(char *oldPath, char* newPath) {
    struct Watchers *s;

    for(s=watchers; s != NULL; s = s->hh.next) {
        if(strcmp(oldPath, s->path) == 0)
        {
            strcpy(s->path, newPath);
            warnf("sucess");
        }
    }
}