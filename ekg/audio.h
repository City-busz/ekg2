/* $Id$ */

/*
 *  (C) Copyright 2003 Wojtek Kaniewski <wojtekka@irc.pl>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License Version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __EKG_AUDIO_H
#define __EKG_AUDIO_H

#include "dynstuff.h"
#include "plugins.h"

typedef struct {
	char *buf;
	int buflen;
} stream_buffer_t;

typedef struct {
	char *name;	/* nazwa urzadzenia */
	
	void *managment_handler;	/* initing / checking if audio_io_t is correct / deiniting */
	watcher_handler_func_t *read_handler;
	watcher_handler_func_t *write_handler;

	void *private;
} audio_t;

typedef struct {
	audio_t *a;
	int fd;
	stream_buffer_t *buffer;
	void *private;
} audio_io_t;

typedef struct {
	char *name;	/* nazwa codeca */

	void *managment_handler;	/* ^^ */
	void *code_handler;
	void *decode_handler;
	void *private;
} codec_t;

typedef struct {
	codec_t *c;		/* codec_t * */
	int way;		/* CODEC_CODE CODEC_DECODE */
	
	void *private;
} audio_codec_t;

typedef struct {
	char *stream_name;
	audio_io_t	*input;
	audio_codec_t	*codec;
	audio_io_t	*output;

	void *private;
} stream_t;

list_t audio_codecs;
list_t audio_inputs;
list_t streams;

/* managment handler: (int INIT_DEINIT, int READ_WRITE, char **initializing_data, void **private)  */

/* 
int audio_register(audio_t *audio);
void audio_unregister(audio_t *audio);
*/ 
int codec_register(codec_t *codec);
void codec_unregister(codec_t *codec);

#endif /* __EKG_AUDIO_H */


/*
 * Local Variables:
 * mode: c
 * c-file-style: "k&r"
 * c-basic-offset: 8
 * indent-tabs-mode: t
 * End:
 */
