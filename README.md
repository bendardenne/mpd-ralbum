# mpd-ralbum

mpd-ralbum is a simple program to add random directories in MPD automatically. It is particularly convenient for
an Artist/Album folder stucture, to add random albums.

It will pick a random directory in MPD's *music_directory* and add all tracks inside that directory and subdirectories to
the playlist.

It supports two modes : monitor mode and normal mode.

## Monitor mode

In monitor mode, the program will fork to the background and repeatedly check the status of the MPD playlist.
If the last track of the playlist is being played, a random directory will be appended at the end of the playlist.

## Normal mode

In normal mode, the program will instantly append a directory at the end of the playlist.

## Other options

For album-based listening, the --delete option will keep a clean playing queue : all tracks before the one currently played
and that have a different "Album" tag will be removed from the queue.

The --recursion option determines how deep the program should go to pick the directory. 
The default (2) is suitable for an "Artist/Album"-style folder hierarchy.

See 

> mpd-ralbum -h

For a full list of available options.

## Dependencies

mpd-ralbum requires [libmpdclient](http://www.musicpd.org/libs/libmpdclient/).
