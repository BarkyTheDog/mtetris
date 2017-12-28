#include <stdio.h>
#include <time.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
/*#include <malloc.h>*/
#include <stdlib.h>
#include <unistd.h>

#include "score.h"

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

int scorefile(char *name,
              int   score,
              int   level,
              void  (*dispproc)())
{
    int              i, j, retry, savmask;
    char             fname[MAXFNAM], *path, found;
    struct score_rec myscore, initrec, fstrec, *allrec;
    size_t           readcnt;
    int              sfp;
    
    if ((path = getenv(SCOREPATH)) != NULL) {
        strcpy(fname, path);
        strcat(fname, "/");
    } else {
        strcpy(fname, "/tmp/mtetris_scores/");
    }
    
    strcat(fname,  SCOREFILE);
    myscore.score = score;
    myscore.level = level;
    time(&myscore.time);
    strcpy(myscore.user, name);
    retry = 0;
    savmask = umask((mode_t)011);
    while ((sfp = open(fname, O_RDWR | O_CREAT ,  0777)) == -1)    {
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
