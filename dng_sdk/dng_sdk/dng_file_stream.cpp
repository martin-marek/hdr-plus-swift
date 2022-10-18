/*****************************************************************************/
// Copyright 2006-2019 Adobe Systems Incorporated
// All Rights Reserved.
//
// NOTICE:	Adobe permits you to use, modify, and distribute this file in
// accordance with the terms of the Adobe license agreement accompanying it.
/*****************************************************************************/

#include "dng_file_stream.h"

#include "dng_exceptions.h"
#include "dng_flags.h"

#if qAndroid
#include <unistd.h>
#endif

/*****************************************************************************/

dng_file_stream::dng_file_stream (const char *filename,
								  bool output,
								  uint32 bufferSize)

	:	dng_stream ((dng_abort_sniffer *) NULL,
					bufferSize,
					0)
	
	,	fFile (NULL)
	
	{

	fFile = fopen (filename, output ? "wb" : "rb");

	if (!fFile)
		{
		
		#if qDNGValidate

		ReportError ("Unable to open file",
					 filename);
					 
		ThrowSilentError ();
		
		#else
		
		ThrowOpenFile ();
		
		#endif

		}
	
	}

/*****************************************************************************/

dng_file_stream::dng_file_stream (FILE *file,
								  uint32 bufferSize)

	:	dng_stream ((dng_abort_sniffer *) NULL,
					bufferSize,
					0)
	
	,	fFile (file)
	
	{

	if (!fFile)
		{
		
		ThrowOpenFile ("Unable to open FILE *");

		}
	
	}

/*****************************************************************************/

#if qAndroid

/*****************************************************************************/

dng_file_stream::dng_file_stream (int fd,
								  bool output,
								  uint32 bufferSize)

	:	dng_stream ((dng_abort_sniffer *) NULL,
					bufferSize,
					0)

	,	fFile (NULL)

	{

	// Note: Use dup here as caller is responsible for separately managing fd.

	fFile = fdopen (dup (fd), output ? "wb" : "rb");

	if (!fFile)
		{

		#if qDNGValidate

		ReportError ("Unable to open file",
					 filename);

		ThrowSilentError ();

		#else

		ThrowOpenFile ();

		#endif

		}

	}

/*****************************************************************************/

#endif	// qAndroid

/*****************************************************************************/

#if qWinOS

/*****************************************************************************/

dng_file_stream::dng_file_stream (const wchar_t *filename,
								  bool output,
								  uint32 bufferSize)

	:	dng_stream ((dng_abort_sniffer *) NULL,
					bufferSize,
					0)
	
	,	fFile (NULL)
	
	{

	fFile = _wfopen (filename, output ? L"wb" : L"rb");

	if (!fFile)
		{
		
		#if qDNGValidate

		char filenameCString[256];

		size_t returnCount;

		wcstombs_s (&returnCount, 
					filenameCString, 
					256, 
					filename, 
					_TRUNCATE);

		ReportError ("Unable to open file",
					 filenameCString);
					 
		ThrowSilentError ();
		
		#else
		
		ThrowOpenFile ();
		
		#endif	// qDNGValidate

		}
	
	}

/*****************************************************************************/

#endif	// qWinOS
		
/*****************************************************************************/

dng_file_stream::~dng_file_stream ()
	{
	
	if (fFile)
		{
		fclose (fFile);
		fFile = NULL;
		}
	
	}
		
/*****************************************************************************/

uint64 dng_file_stream::DoGetLength ()
	{
	
	if (fseek (fFile, 0, SEEK_END) != 0)
		{
		
		ThrowReadFile ();

		}
	
	return (uint64) ftell (fFile);
	
	}
		
/*****************************************************************************/

void dng_file_stream::DoRead (void *data,
							  uint32 count,
							  uint64 offset)
	{
	
	if (fseek (fFile, (long) offset, SEEK_SET) != 0)
		{
		
		ThrowReadFile ();

		}
	
	uint32 bytesRead = (uint32) fread (data, 1, count, fFile);
	
	if (bytesRead != count)
		{
		
		ThrowReadFile ();

		}
	
	}
		
/*****************************************************************************/

void dng_file_stream::DoWrite (const void *data,
							   uint32 count,
							   uint64 offset)
	{
	
	if (fseek (fFile, (uint32) offset, SEEK_SET) != 0)
		{
		
		ThrowWriteFile ();

		}
	
	uint32 bytesWritten = (uint32) fwrite (data, 1, count, fFile);
	
	if (bytesWritten != count)
		{
		
		ThrowWriteFile ();

		}
	
	}
		
/*****************************************************************************/
