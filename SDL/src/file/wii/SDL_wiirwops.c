/*
SDL - Simple DirectMedia Layer
Copyright (C) 1997-2006 Sam Lantinga

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

Sam Lantinga
slouken@libsdl.org
*/
#include "SDL_config.h"

/* This file provides a general interface for SDL to read and write
data sources.  It can easily be extended to files, memory, etc.
*/

#include "SDL_endian.h"
#include "SDL_rwops.h"

#ifdef __WII__

#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>

/* Constants */
#define WII_MAX_PATH 260

static SDL_bool initialised = SDL_FALSE;

extern bool fatInitDefault(void);
extern bool fatInit (unsigned int cacheSize, bool setAsDefaultDevice);

/* Translate a path to DOS 8.3 format */
static SDL_bool TranslatePath(const char* const in, char* const out)
{
	char		tokens[WII_MAX_PATH + 1];
	const char	delimiters[] = "/\\";
	char*		token;

	// Clear the output.
	*out = '\0';

	// Keep a copy of the input for tokenising.
	strncpy(tokens, in, WII_MAX_PATH);
	tokens[WII_MAX_PATH] = '\0';

	// Split the input into tokens.
	token = strtok(tokens, delimiters);
	while (token)
	{
		if (SDL_strcmp(token, ".") == 0)
		{
			// Skip single dots.
		}
		else if (SDL_strcmp(token, "..") == 0)
		{
			// Can't handle double dots yet.
			SDL_SetError("File name contains a \"..\" - translation into DOS format isn't implemented yet");
			return SDL_FALSE;
		}
		else
		{
			size_t	token_base_length;
			char*	dot;
			char*	space;

			// Find the dot in the token.
			dot = SDL_strchr(token, '.');
			if (dot)
			{
				// Is the extension longer than 3 characters?
				char* const extension = dot + 1;
				if (SDL_strlen(extension) > 3)
				{
					// Truncate the extension to 3 characters.
					extension[3] = '\0';
				}
			}

			// Delete any spaces.
			space = SDL_strchr(token, ' ');
			while (space)
			{
				// Move the string after the space back a character.
				strcpy(space, space + 1);

				// Find the next space.
				space = SDL_strchr(token, ' ');
			}

			// Is the base of the filename longer than 8 chars?
			token_base_length = strcspn(token, ".");
			if (token_base_length > 8)
			{
				// Add a ~1.
				token[6] = '~';
				token[7] = '1';

				// Move the extension back.
				strcpy(&token[8], &token[token_base_length]);
			}

			// Convert the token to upper case.
			SDL_strupr(token);

			// Is the buffer too small to fit a slash and the token?
			if ((SDL_strlen(out) + 1 + SDL_strlen(token)) > WII_MAX_PATH)
			{
				SDL_SetError("File name truncated due to buffer length");
				return SDL_FALSE;
			}

			// Append the slash and the name.
			strcat(out, "/");
			strcat(out, token);
		}

		// Next token.
		token = strtok(NULL, delimiters);
	}

	// Check for empty strings.
	if (SDL_strlen(out) == 0)
	{
		SDL_SetError("Empty file name after translation");
		return SDL_FALSE;
	}
	
	// Done!
	return SDL_TRUE;
}

static int SDLCALL wii_seek(SDL_RWops *context, int offset, int whence)
{
	int action = SEEK_SET;
	switch (whence)
	{
	case RW_SEEK_CUR:
		action = SEEK_CUR;
		break;
	case RW_SEEK_END:
		action = SEEK_END;
		break;
	case RW_SEEK_SET:
		break;
	default:
		SDL_Error(SDL_EFSEEK);
		return(-1);
	}

	if ( fseek(context->hidden.wii.fp, offset, action) == 0 ) {
		return ftell(context->hidden.wii.fp);
	} else {
		SDL_Error(SDL_EFSEEK);
		return(-1);
	}
}

static int SDLCALL wii_read(SDL_RWops *context, void *ptr, int size, int num)
{
	int bytes_read;
	bytes_read = fread(ptr, size, num, context->hidden.wii.fp); 
	if ( bytes_read == -1) {
		SDL_Error(SDL_EFREAD);
		return(-1);
	}
	return(bytes_read);
}

static int SDLCALL wii_write(SDL_RWops *context, const void *ptr, int size, int num)
{
	int bytes_written;

	bytes_written = fwrite(ptr, size, num, context->hidden.wii.fp);
	if ( bytes_written != (size * num) ) {
		SDL_Error(SDL_EFWRITE);
		return(-1);
	}
	return(num);
}

static int SDLCALL wii_close(SDL_RWops *context)
{
	if ( context )
	{
		fclose(context->hidden.wii.fp);
		SDL_FreeRW(context);
	}
	return(0);
}

SDL_RWops *SDL_RWFromFile(const char *file, const char *mode)
{
	char translated_path[WII_MAX_PATH + 1];
	int stat_result;
	struct stat stat_info;
	FILE* fp;

	if (!file || !*file || !mode || !*mode)
	{
		SDL_SetError("SDL_RWFromFile(): No file or no mode specified");
		return NULL;
	}

	/* Translate the path */
	if (!TranslatePath(file, translated_path))
	{
		// The error string should have been set inside TranslatePath().
		return NULL;
	}

#if 1
	printf("translated '%s' to '%s'\n", file, translated_path);
#endif

	/* Initialise the SD library */
	if (!initialised)
	{
		fatInit(32, 1);
		//fatInitDefault();
		initialised = SDL_TRUE;
	}

	/* Opening for reading? */
	memset(&stat_info, 0, sizeof(stat_info));
	if (mode[0] == 'r')
	{
		/* Find the file */
		stat_result = stat(translated_path, &stat_info);
		if (stat_result != 0)
		{
			SDL_SetError("Couldn't find %s to get its length", translated_path);
			return NULL;
		}
	}

	/* Open the file */
	fp = fopen(translated_path, mode);
	if ( fp == NULL )
	{
		SDL_SetError("Couldn't open %s", translated_path);
		return NULL;
	}
	else
	{
		SDL_RWops *rwops = SDL_AllocRW();
		if ( rwops != NULL )
		{
			rwops->seek = wii_seek;
			rwops->read = wii_read;
			rwops->write = wii_write;
			rwops->close = wii_close;
			rwops->hidden.wii.fp = fp;
			rwops->hidden.wii.size = stat_info.st_size;
		}
		return(rwops);
	}
}

#endif /* __WII__ */
