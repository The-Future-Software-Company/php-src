/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) Zend Technologies Ltd. (http://www.zend.com)           |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.00 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.zend.com/license/2_00.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Wez Furlong <wez@thebrainroom.com>                          |
   |          Scott MacVicar <scottmac@php.net>                           |
   |          Nuno Lopes <nlopess@php.net>                                |
   |          Marcus Boerger <helly@php.net>                              |
   +----------------------------------------------------------------------+
*/

#ifndef ZEND_STREAM_H
#define ZEND_STREAM_H

#include "zend_result.h"

#include <sys/types.h>
#include <sys/stat.h>

/* Lightweight stream implementation for the ZE scanners.
 * These functions are private to the engine.
 * */
typedef size_t (*zend_stream_fsizer_t)(void* handle);
typedef ssize_t (*zend_stream_reader_t)(void* handle, char *buf, size_t len);
typedef void   (*zend_stream_closer_t)(void* handle);

#define ZEND_MMAP_AHEAD 32

typedef enum {
	ZEND_HANDLE_FILENAME,
	ZEND_HANDLE_FP,
	ZEND_HANDLE_STREAM
} zend_stream_type;

typedef struct _zend_stream {
	void        *handle;
	int         isatty;
	zend_stream_reader_t   reader;
	zend_stream_fsizer_t   fsizer;
	zend_stream_closer_t   closer;
} zend_stream;

typedef struct _zend_file_handle {
	union {
		FILE          *fp;
		zend_stream   stream;
	} handle;
	zend_string       *filename;
	zend_string       *opened_path;
	zend_uchar        type; /* packed zend_stream_type */
	bool              primary_script;
	bool              in_list; /* added into CG(open_file) */
	char              *buf;
	size_t            len;
} zend_file_handle;

BEGIN_EXTERN_C()
ZEND_API void zend_stream_init_fp(zend_file_handle *handle, FILE *fp, const char *filename);
ZEND_API void zend_stream_init_filename(zend_file_handle *handle, const char *filename);
ZEND_API void zend_stream_init_filename_ex(zend_file_handle *handle, zend_string *filename);
ZEND_API zend_result zend_stream_open(zend_file_handle *handle);
ZEND_API zend_result zend_stream_fixup(zend_file_handle *file_handle, char **buf, size_t *len);
ZEND_API void zend_destroy_file_handle(zend_file_handle *file_handle);

void zend_stream_init(void);
void zend_stream_shutdown(void);
END_EXTERN_C()

#ifdef ZEND_WIN32
# include "win32/ioutil.h"
typedef php_win32_ioutil_stat_t zend_stat_t;
#ifdef _WIN64
#  define zend_fseek _fseeki64
#  define zend_ftell _ftelli64
#  define zend_lseek _lseeki64
# else
#  define zend_fseek fseek
#  define zend_ftell ftell
#  define zend_lseek lseek
# endif
# define zend_fstat php_win32_ioutil_fstat
# define zend_stat php_win32_ioutil_stat
#else
typedef struct stat zend_stat_t;
# define zend_fseek fseek
# define zend_ftell ftell
# define zend_lseek lseek
# define zend_fstat fstat
# define zend_stat stat
#endif

#endif
