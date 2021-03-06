/*
 * MOC - music on console
 * Copyright (C) 2004 Damian Pietras <daper@daper.net>
 *
 * libTiMidity-plugin Copyright (C) 2007 Hendrik Iben <hiben@tzi.de>
 * The hard work is done by the libTiMidity-Library written by
 * Konstantin Korikov (http://libtimidity.sourceforge.net).
 * I have merely hacked together a wrapper...
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define DEBUG

#include <ctype.h> // for toupper
#include <string.h>
#include "io.h"
#include "decoder.h"
#include "log.h"
#include "files.h"
#include "common.h"
#include "options.h"
#include <timidity.h>

MidSongOptions midioptions;

struct timidity_data
{
  MidSong *midisong;
  int length;
  struct decoder_error error;
};

static struct timidity_data *make_timidity_data(const char *file) {
  struct timidity_data *data;

  data = (struct timidity_data *)xmalloc (sizeof(struct timidity_data));

  data->midisong = NULL;
  decoder_error_init (&data->error);

  MidIStream *midistream = mid_istream_open_file(file);
  
  if(midistream==NULL) {
    decoder_error(&data->error, ERROR_FATAL, 0, "Can't open midifile: %s", file);
    return data;
  }
  
  data->midisong = mid_song_load(midistream, &midioptions);
  mid_istream_close(midistream);

  if(data->midisong==NULL) {
    decoder_error(&data->error, ERROR_FATAL, 0, "Can't load midifile: %s", file);
    return data;
  }

  return data;
}

static void *timidity_open (const char *file)
{
  struct timidity_data *data = make_timidity_data(file);

  if(data->midisong) {
    data->length = mid_song_get_total_time(data->midisong); 
  }


  if(data->midisong) {
    debug ("Opened file %s", file);
  
    mid_song_set_volume(data->midisong, options_get_int("TiMidity_Volume"));
    mid_song_start(data->midisong);
  }

  return data;
}

static void timidity_close (void *void_data)
{
  struct timidity_data *data = (struct timidity_data *)void_data;

  if (data->midisong) {
    mid_song_free(data->midisong);
  }
}

static void timidity_info (const char *file_name, struct file_tags *info,
		const int tags_sel)
{
  struct timidity_data *data = make_timidity_data(file_name);

  if(data->midisong==NULL)
    return;

  if(tags_sel & TAGS_TIME) {
    info->time = mid_song_get_total_time(data->midisong) / 1000;
    info->filled |= TAGS_TIME;
  }

  timidity_close(data);  
}

static int timidity_seek (void *void_data, int sec)
{
  struct timidity_data *data = (struct timidity_data *)void_data;

  int ms = sec*1000;

  ms = (ms>data->length)?data->length:((ms<0)?0:ms);

  mid_song_seek(data->midisong, ms);

  return ms/1000;
}

static int timidity_decode (void *void_data, char *buf, int buf_len,
		struct sound_params *sound_params)
{
  struct timidity_data *data = (struct timidity_data *)void_data;
  
  sound_params->channels = midioptions.channels;
  sound_params->rate = midioptions.rate;
  sound_params->fmt = (midioptions.format==MID_AUDIO_S16LSB)?(SFMT_S16 | SFMT_LE):SFMT_S8;

  return mid_song_read_wave(data->midisong, buf, buf_len);
}

static int timidity_get_bitrate (void *void_data ATTR_UNUSED)
{
  return -1;
}

static int timidity_get_duration (void *void_data)
{
  struct timidity_data *data = (struct timidity_data *)void_data;
  return data->length/1000;
}



static void timidity_get_name (const char *file, char buf[4])
{
  char *ext = ext_pos (file);

  strncpy(buf, ext, 3);

  unsigned int i;

  for(i=0;i<strlen(ext);i++)
    buf[i]=toupper(buf[i]);
}

static int timidity_our_format_ext(const char *ext)
{
  return !strcasecmp(ext, "MID");
}

static void timidity_get_error (void *prv_data, struct decoder_error *error)
{
  struct timidity_data *data = (struct timidity_data *)prv_data;
  
  decoder_error_copy (error, &data->error);
}

static void timidity_destroy()
{
  mid_exit();
}

static struct decoder timidity_decoder =
{
  DECODER_API_VERSION,
  NULL,
  timidity_destroy,
  timidity_open,
  NULL,
  NULL,
  timidity_close,
  timidity_decode,
  timidity_seek,
  timidity_info,
  timidity_get_bitrate,
  timidity_get_duration,
  timidity_get_error,
  timidity_our_format_ext,
  NULL,
  timidity_get_name,
  NULL,
  NULL,
  NULL
};

struct decoder *plugin_init ()
{
  int initresult = mid_init(options_get_str("TiMidity_Config"));

  // is there a better way to signal failed init ?
  // the decoder-init-function may no return errors AFAIK...
  if(initresult < 0)
  {
    fatal("TiMidity-Plugin: Error processing TiMidity-Configuration!");
    return NULL;
  }

  midioptions.rate = options_get_int("TiMidity_Rate");
  midioptions.format = (options_get_int("TiMidity_Bits")==16)?MID_AUDIO_S16LSB:MID_AUDIO_S8;
  midioptions.channels = options_get_int("TiMidity_Channels");
  midioptions.buffer_size = midioptions.rate;

  return &timidity_decoder;
}

