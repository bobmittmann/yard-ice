/*
 *
 * File:	mime.h
 * Module:
 * Project:	
 * Author:	Robinson Mittmann (bob@boreste.com, bobmittmann@gmail.com)
 * Target:
 * Comment: MIME file types
 * Copyright(c) 2004-2009 BORESTE (www.boreste.com). All Rights Reserved.
 *
 */

#ifndef __MIME_H__
#define __MIME_H__

enum __mime_type {
	/* application/octet-stream */
	APPLICATION_OCTET_STREAM,
	/* application/pdf */
	APPLICATION_PDF,
	/* application/postscript */
	APPLICATION_POSTSCRIPT,
	/* application/rtf */
	APPLICATION_RTF,
	/* application/sgml */
	APPLICATION_SGML,
	/* application/x-dvi */
	APPLICATION_X_DVI,
	/* application/x-gzip */
	APPLICATION_X_GZIP,
	/* application/x-javascript */
	APPLICATION_X_JAVASCRIPT,
	/* application/x-sh */
	APPLICATION_X_SH,
	/* application/x-shockwave-flash */
	APPLICATION_X_SHOCKWAVE_FLASH,
	/* application/x-tar */
	APPLICATION_X_TAR,
	/* application/xml */
	APPLICATION_XML,
	/* application/zip */
	APPLICATION_ZIP,
	/* audio/midi */
	AUDIO_MIDI,
	/* audio/mpeg */
	AUDIO_MPEG,
	/* audio/x-aiff */
	AUDIO_X_AIFF,
	/* audio/x-realaudio */
	AUDIO_X_REALAUDIO,
	/* audio/x-wav */
	AUDIO_X_WAV,
	/* image/gif */
	IMAGE_GIF,
	/* image/jpeg */
	IMAGE_JPEG,
	/* image/png */
	IMAGE_PNG,
	/* image/tiff */
	IMAGE_TIFF,
	/* multipart/digest */
	MULTIPART_DIGEST,
	/* multipart/encrypted */
	MULTIPART_ENCRYPTED,
	/* multipart/form-data */
	MULTIPART_FORM_DATA,
	/* text/css */
	TEXT_CSS,
	/* text/directory */
	TEXT_DIRECTORY,
	/* text/html */
	TEXT_HTML,
	/* text/plain */
	TEXT_PLAIN,
	/* text/rtf */
	TEXT_RTF,
	/* text/sgml */
	TEXT_SGML,
	/* text/tab-separated-values */
	TEXT_TAB_SEPARATED_VALUES,
	/* text/uri-list */
	TEXT_URI_LIST,
	/* text/xml */
	TEXT_XML,
	/* video/mpeg */
	VIDEO_MPEG,
	/* video/quicktime */
	VIDEO_QUICKTIME,
	/* video/x-msvideo */
	VIDEO_X_MSVIDEO,
};

typedef enum __mime_type mimetype_t;

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __MIME_H__ */

