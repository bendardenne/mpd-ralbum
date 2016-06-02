#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <getopt.h>
#include <mpd/client.h>

#include "mpd-ralbum.h"
#include "main.h"

static cleanup_t clean;
static struct option long_opt[] =
{
	{"help", no_argument, 0, 'h'},
	{"server", required_argument, 0, 's' },
	{"port", required_argument, 0, 'p'},
	{"delete", no_argument, 0, 'd'},
	{"monitor", no_argument, 0, 'm'},
	{"recursion", required_argument, 0, 'r'},
	{"quiet", no_argument, 0, 'q'},
	{0,0,0,0}
};

int main(int argc, char* argv[])
{
	unsigned count, port = 0, i, recursion = 2;
	char path[PATH_MAX] = "", *host = NULL;
	struct timeval* seed1;
	pid_t seed2;
	char opt;
	bool monitor = false, delete = false, quiet = false;
	struct mpd_connection* handle;
	enum mpd_idle idle;
	extern char* optarg;

	/* Catch signals */
	signal(SIGHUP, signal_handle);
	signal(SIGINT, signal_handle);
	signal(SIGTERM, signal_handle);
	signal(SIGABRT, signal_handle);

	/* Command line options parsing */
	while((opt = getopt_long(argc, argv, "hmdqs:p:r:", long_opt, NULL)) != -1)
	{
		switch(opt)
		{
			case 'h' :
				usage(argv[0], stdout);
				exit(EXIT_SUCCESS);
			case 'm' :
				monitor = true;
				break;
			case 'd' :
				delete = true;
				break;
			case 's' :
				host = optarg;
				break;
			case 'p' :
				sscanf(optarg, "%d", &port);
				break;
			case 'q' :
				quiet = true;
				break;
			case 'r' :
				if(sscanf(optarg, "%u", &recursion) != 1 || recursion < 1)
				{
					fprintf(stderr, "Invalid argument: %s\n", optarg);
					exit(EXIT_FAILURE);
				}
				break;
			default :
				usage(argv[0], stderr);
				exit(EXIT_FAILURE);
				break;
		}
	}

	/* Daemonize */
	if(monitor)
	{
		pid_t pid = fork();
		if(pid > 0) {
            printf("Started daemon with PID %d\n", pid);
			return EXIT_SUCCESS;
        } else if(pid == -1)
			return EXIT_FAILURE;
	}

	/* MPD Init */
	handle = mpd_connection_new(host, port, 0);
	if(handle == NULL)
	{
		perror("malloc");
		return(EXIT_FAILURE);
	}
	clean.mpd = handle;
	mpd_check_error(handle);


	/* srand() seed -- Time in ms XOR'd with PID to avoid same number picked in same second */
	seed1 = malloc(sizeof(struct timeval));
	gettimeofday(seed1, NULL);
	seed2 = getpid();
	srand(seed1->tv_usec ^ seed2);
	free(seed1);

	do
	{
		if(monitor)
		{
			/* Wait for state change */
			idle = mpd_run_idle(handle);

			if(!(idle & MPD_IDLE_PLAYER && last_track(handle)))
				continue;
		}

		/* Previous albums deletion */
		if(delete)
			delete_old_songs(handle);

		/* Build a random path */
		for(i = 0; i < recursion; i++)
		{
			count = count_files(handle, path, true);
			if(count == 0)
			{
				fprintf(stderr, "Error: No more directories to continue recursion. "
									 "Proceeding with path \"%s\"\n", path);
				break;
			}
			pick_directory(handle, path, rand() % count);
		}

		/* Add the path */
		if(!quiet)
			printf("Adding: %s\n", path);

		mpd_run_add(handle, path);

		path[0] = '\0';
	} while(monitor);

	mpd_connection_free(handle);

	return EXIT_SUCCESS;
}

void signal_handle(int sig)
{
	switch(sig)
	{
		case SIGABRT:
		case SIGTERM:
		case SIGINT:
			mpd_connection_free(clean.mpd);
			exit(EXIT_FAILURE);

		case SIGHUP:
		default:
			break;
	}
}

void usage(char* me, FILE* dest)
{
	fprintf(dest, "USAGE: %s [OPTIONS]\n", me);
	fprintf(dest, "\nOptions:\n"
	"	 -h, --help\t\t\tPrint this help.\n"
	"	 -s, --server HOST\t\tServer hostname.\n"
	"	 -p, --port PORT\t\tConnection port.\n"
	"	 -d, --delete\t\tDelete previous tracks which are not part of the currently playing album.\n"
	"	 \t\t\t\t(does nothing if no music is playing.)\n"
	"	 -m, --monitor\t\tMonitor MPD's activity and add an album when the playlist reaches the last track.\n"
	"	 -r, --recursion LEVEL\tLevel of recursion. (default: 2 ; minimum: 1) \n"
	"	 -q, --quiet\t\t\tSuppress output. \n");
}

