#ifndef __MPD_RALBUM_H
#define __MPD_RALBUM_H


unsigned count_files(struct mpd_connection* handle, char* path, bool dirs_only);

void pick_directory(struct mpd_connection* handle, char* path, unsigned pos);

void delete_old_songs(struct mpd_connection* handle);

bool last_track(struct mpd_connection *handle);

inline void mpd_check_error(struct mpd_connection* handle);

#endif

