SUBDIRS = themes decoder_plugins
AM_CPPFLAGS = -DSYSTEM_THEMES_DIR=\"$(pkgdatadir)/themes\" \
	      -DPLUGIN_DIR=\"$(plugindir)/$(DECODER_PLUGIN_DIR)\"

bin_PROGRAMS = mocp
mocp_SOURCES = log.c \
	       log.h \
	       protocol.h \
	       protocol.c \
	       server.c \
	       server.h \
	       main.c \
	       common.c \
	       common.h \
	       playlist.c \
	       playlist.h \
	       fifo_buf.c \
	       fifo_buf.h \
	       out_buf.c \
	       out_buf.h \
	       audio.c \
	       audio.h \
	       decoder.c \
	       decoder.h \
       	       interface.c \
	       interface.h \
	       interface_elements.c \
	       interface_elements.h \
	       menu.c \
	       menu.h \
	       files.c \
	       files.h \
	       options.c \
	       options.h \
	       player.c \
	       player.h \
	       playlist_file.c \
	       playlist_file.h \
	       themes.c \
	       themes.h \
	       keys.c \
	       keys.h \
	       io.c \
	       io.h \
	       compat.c \
	       compat.h \
	       audio_conversion.c \
	       audio_conversion.h \
	       rbtree.c \
	       rbtree.h \
	       tags_cache.c \
	       tags_cache.h \
	       utf8.c \
	       utf8.h \
	       audio_helper.h \
	       audio_helper.c \
	       softmixer.c \
	       softmixer.h \
	       lyrics.h \
	       lyrics.c \
	       equalizer.h \
	       equalizer.c
EXTRA_mocp_SOURCES = gnugetopt.h \
		     getopt.c \
		     getopt1.c \
		     null_out.c \
		     null_out.h \
		     oss.c \
		     oss.h \
		     alsa.c \
		     alsa.h \
		     io_curl.c \
		     io_curl.h \
		     jack.c \
		     jack.h
man_MANS = mocp.1
mocp_LDADD = @EXTRA_OBJS@ -lltdl
mocp_DEPENDENCIES = @EXTRA_OBJS@
mocp_LDFLAGS = @EXTRA_LIBS@ $(RCC_LIBS)
EXTRA_DIST = config.example README_equalizer mocp.1 THANKS keymap.example Doxyfile \
	     doxy_pages/decoder_api.doxy doxy_pages/main_page.doxy \
	     doxy_pages/sound_output_driver_api.doxy moc.spec.in \
	     moc.spec

docdir = $(datadir)/doc/$(PACKAGE)
doc_DATA = config.example THANKS README README_equalizer keymap.example
