#include <stdio.h>
#include <time.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
/*#include <malloc.h>*/
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#include "score.h"

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

int scorefile(char *name,
              int   score,
              int   level,
              void  (*dispproc)(struct score_rec *allrec,
                                int               cnt,
                                int               rank,
                                char              upped,
                                char              first))
{
    int              i, j, retry, savmask;
    char             fname[MAXFNAM], *path, found;
    struct score_rec myscore, initrec, fstrec, *allrec;
    size_t           readcnt;
    int              sfp;
    
    bzero(fname, MAXFNAM);
    if ((path = getenv(SCOREPATH)) != NULL) {
        strcpy(fname, path);
        strcat(fname, "/");
    } else {
        strcpy(fname, "/tmp/mtetris_scores/");
    }
    
    struct stat st = {0};
    if (stat(fname, &st) == -1) {
        int result = mkdir(fname, 0777);
        if (result == -1) {
            printf("Scorefile directory \"%s\" creation failed: %d (%d: %s)\n", fname, result, errno, strerror(errno));
            return FALSE;
        }
    }
    
    strcat(fname, SCOREFILE);
    myscore.score = score;
    myscore.level = level;
    time(&myscore.time);
    bzero(myscore.user, 256);
    strcpy(myscore.user, name);
    retry = 0;
    savmask = umask((mode_t)011);
    while ((sfp = open(fname, O_RDWR | O_CREAT, 0666)) == -1)    {
        printf("File open failed: %d (%d: %s)\n", sfp, errno, strerror(errno));
        sleep(1);
        if (retry++ > 10)
            return FALSE;
    }
#ifdef ultrix
    flock(sfp,  LOCK_EX);
#endif
    umask((mode_t)savmask);
    
    if ((readcnt = read(sfp, &fstrec, sizeof(struct score_rec))) == 0) {
        /* New File was created */
        initrec.level = 0;
        initrec.score = 1;
        time( &initrec.time );
        bzero(initrec.user, 256);
        strcpy(initrec.user, "mtetris");
        write(sfp, &initrec, sizeof(struct score_rec));
        write(sfp, &myscore, sizeof(struct score_rec));
#ifdef ultrix
        flock(sfp, LOCK_UN);
#endif
        close(sfp);
        (dispproc)(&myscore, 1, 0, TRUE, TRUE);
        return TRUE;
    } else {
        if (readcnt == -1)
            return FALSE;
    }
    
    allrec = (struct score_rec *)malloc((fstrec.score + 1) *
                                        sizeof(struct score_rec));
    if ((readcnt =
         read(sfp, allrec,
              (int)(sizeof(struct score_rec) * fstrec.score))) == -1) {
             return FALSE;
         }
    
    for (found = FALSE,  i = 0; i < fstrec.score; i++) {
        if (allrec[i].score <= myscore.score) {
            break;
        }
        if (strcmp(allrec[i].user,  myscore.user) == 0) {
            found  = TRUE;
            break;
        }
    }
    
    if (found) {
#ifdef ultrix
        flock(sfp,  LOCK_UN);
#endif
        close(sfp);
        (dispproc)(allrec, fstrec.score, i, FALSE, FALSE);
    } else {
        for (j = fstrec.score-1; j >= i; j--) {
            if (strcmp(allrec[j].user,  myscore.user) == 0) {
                found  = TRUE;
                break;
            }
            allrec[j+1] = allrec[j];
        }
        if (found) {
            memcpy(&allrec[j+1], &allrec[j+2], 
                   (fstrec.score - j - 1 ) * sizeof (struct score_rec));
            for (j-- ; j >= i; j--) {
                allrec[j+1] = allrec[j];
            }
        }
        
        allrec[i] = myscore;
        if (!found) {
            fstrec.score++;
        }
        
        lseek(sfp, 0, 0);
        write(sfp, &fstrec, (int)sizeof(struct score_rec));
        write(sfp, allrec, (int)(sizeof(struct score_rec) * fstrec.score));
#ifdef ultrix
        flock(sfp, LOCK_UN);
#endif
        close(sfp);
        (dispproc)(allrec, fstrec.score, i, TRUE, (found) ? FALSE : TRUE);
    }
    free(allrec);
    return TRUE;
}
