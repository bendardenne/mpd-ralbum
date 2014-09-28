#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <limits.h>
#include <mpd/client.h>

#include "mpd-ralbum.h"

unsigned count_files(struct mpd_connection* handle, char* path, bool dirs_only)
{
	unsigned i;
	struct mpd_entity* response;

	mpd_send_list_meta(handle, path);
	mpd_check_error(handle);
	i = 0;
	while((response = mpd_recv_entity(handle)) != NULL)
	{
		if(!dirs_only || mpd_entity_get_type(response) == MPD_ENTITY_TYPE_DIRECTORY)
			i++;
		mpd_entity_free(response);
	}

	mpd_response_finish(handle);
	return i;
}

void pick_directory(struct mpd_connection* handle, char* path, unsigned pos)
{
	unsigned i;
	struct mpd_entity* response;
	const char *tmp;

	mpd_send_list_meta(handle, path);
	mpd_check_error(handle);

	i = 0;
	while((response = mpd_recv_entity(handle)) != NULL && i != pos)
	{
		i++;
		mpd_entity_free(response);
	}

	tmp = mpd_directory_get_path(mpd_entity_get_directory(response));
	strncpy(path, tmp, PATH_MAX);
	mpd_entity_free(response);

	mpd_response_finish(handle);
}

void delete_old_songs(struct mpd_connection* handle)
{
	struct mpd_song *playing, *cur;
	unsigned end;

	playing = mpd_run_current_song(handle);
	cur = mpd_run_get_queue_song_pos(handle, 0);
	mpd_check_error(handle);

	end = 0;
	while(strcmp(mpd_song_get_tag(cur, MPD_TAG_ALBUM, 0), mpd_song_get_tag(playing, MPD_TAG_ALBUM, 0)) != 0)
	{
		end++;
		mpd_song_free(cur);
		cur = mpd_run_get_queue_song_pos(handle, end);
	}

	mpd_send_delete_range(handle, 0, end);
	mpd_check_error(handle);
	mpd_song_free(cur);
	mpd_song_free(playing);
	mpd_response_finish(handle);
}

bool last_track(struct mpd_connection *handle)
{
	struct mpd_song *playing;
	playing = mpd_run_current_song(handle);
	mpd_check_error(handle);
	if(playing == NULL)
		return false;

	unsigned position = mpd_song_get_pos(playing);
	mpd_song_free(playing);

	if((playing = mpd_run_get_queue_song_pos(handle, position + 1)) == NULL)
		return true;
	else
	{
		mpd_song_free(playing);
		return false;
	}

}

inline void mpd_check_error(struct mpd_connection *handle)
{
	if(mpd_connection_get_error(handle) == MPD_ERROR_SUCCESS)
		return;

	fprintf(stderr, "%s\n", mpd_connection_get_error_message(handle));
	raise(SIGABRT);
}

