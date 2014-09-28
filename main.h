#ifndef __MAIN_H
#define __MAIN_H

#ifndef PATH_MAX
#define PATH_MAX 1024
#endif

typedef struct cleanup_t {
	struct mpd_connection* mpd;
} cleanup_t;

void signal_handle(int sig);

void usage(char* me, FILE* dest);

#endif
