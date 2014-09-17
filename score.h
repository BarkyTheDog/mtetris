/*
	This is Scorefile record structure difinition.
*/

#include <stdio.h>

#define  SCOREPATH "MTETRIS_SCOREPATH"
#define  SCOREFILE "scorefile"
#define  MAXFNAM   256

struct score_rec {
    int	    score;
    short   level;
    /*    char    user[L_cuserid];*/
    char user[256];
    time_t  time;
};
