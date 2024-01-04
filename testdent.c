/*
    Program to show the difference between how readdir() and stat() show symlinks.

    Adapted from https://stackoverflow.com/a/39430337 and https://www.unix.com/man-page/linux/3/stat/
*/
#define _GNU_SOURCE // includes _BSD_SOURCE for DT_UNKNOWN etc.
#include <dirent.h>
#include <stdint.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFSIZE 256

char *check_d_type(int d_type)
{
    if (d_type == DT_DIR)
    {
        return "Directory";
    }
    else if (d_type == DT_LNK)
    {
        return "Link";
    }
    else if (d_type == DT_REG)
    {
        return "Regular file";
    }
    else if (d_type == DT_UNKNOWN)
    {
        return "DT_UNKNOWN";
    }
    else
        return "Other";
}

char *check_stmode(int stmode)
{
    if (S_ISLNK(stmode))
    {
        return "Link";
    }
    else if (S_ISDIR(stmode))
    {
        return "Directory";
    }
    else if (S_ISREG(stmode))
    {
        return "Regular file";
    }
    else
        return "Other";
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        printf("Must supply path to open\n");
        return 1;
    }
    DIR *dirhandle = opendir(argv[1]); // POSIX doesn't require this to be a plain file descriptor.  Linux uses open(".", O_DIRECTORY); to implement this
    //^Todo: error check
    struct dirent *de;
    while (de = readdir(dirhandle))
    { // NULL means end of directory
        if (strcmp(de->d_name, ".") == 0 || strcmp(de->d_name, "..") == 0)
        {
            // Skip . and ..
            continue;
        }
        printf("%s\n", de->d_name);
#ifdef _DIRENT_HAVE_D_TYPE
        printf("D_TYPE=%s\n", check_d_type(de->d_type));
#endif
        // the only method if d_type isn't available,
        // otherwise this is a fallback for FSes where the kernel leaves it DT_UNKNOWN.
        char pathbuf[BUFSIZE];
        snprintf(pathbuf, BUFSIZE, "%s/%s", argv[1], de->d_name);

        struct stat lstbuf;
        int lstatrc = lstat(pathbuf, &lstbuf);
        if (lstatrc != 0)
        {
            printf("Lstat error: %d\n", lstatrc);
        }
        printf("LSTAT type=%s\n", check_stmode(lstbuf.st_mode));

        // stat follows symlinks, lstat doesn't.
        struct stat stbuf;
        int statrc = stat(pathbuf, &stbuf);
        if (statrc != 0)
        {
            printf("Stat error: %d\n", statrc);
        }
        printf("STAT type=%s\n\n", check_stmode(stbuf.st_mode));
    }
    return 0;
}