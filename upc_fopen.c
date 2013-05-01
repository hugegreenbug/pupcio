/************************************************************************/
/*              UPC-IO Implementation Based on UPC-ADIO                 */
/*                                                                      */
/*  For more detail description of the UPC-IO, please refer to the UPC  */
/*  language specification. For licence information, please refer to    */
/*  the README file.                                                    */
/*                                                                      */
/*              HPCL, The George Wasnington University                  */
/*                        Author: Yiyi Yao                              */
/*                       E-mail: yyy@gwu.edu                            */
/************************************************************************/
// upc_fopen.c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <upc.h>
#include "upc_io.h"

upcio_file_t * upc_all_fopen( const char *filename,
			    int flags, mode_t mode )
{
	int error_code;
	shared struct __struct_upc_file_t * fh;
	struct __struct_thread_upc_file_t * fh_local;
	char * local_filename;
	upc_off_t offset;
	Plfs_fd *fd = NULL;

	/*------------------------------------------------------------------*/
	/* assign the global upc file handler                               */
	/* here I have to use the complex struct name because the upc_alloc */
	/* has to return a pointer with "shared" qualifier                  */
	/*------------------------------------------------------------------*/
	fh = (shared struct __struct_upc_file_t *)upc_all_alloc(1, sizeof(struct __struct_upc_file_t));
	if(fh == NULL)
		return NULL;
	/*------------------------------------------------------------------*/
	/* assign the local file handler for each thread                    */
	/* again I have to use the complex struct name because the upc_alloc*/
	/* has to return a pointer with "shared" qualifier                  */
	/*------------------------------------------------------------------*/
	fh->th[MYTHREAD] = (shared struct __struct_thread_upc_file_t *)upc_alloc(sizeof(struct __struct_thread_upc_file_t));
	if(fh->th[MYTHREAD] == NULL)
	{
		if( !MYTHREAD )
			upc_free( fh );

		return NULL;
	}

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh_local = (struct __struct_thread_upc_file_t *)(fh->th[MYTHREAD]);

	/*------------------------------------------------------------------*/
	/* setup the file handler metadata                                  */
	/* hints are now discarded, a high quality implementation may want  */
	/* to keep the hint information for later use                       */
	/*------------------------------------------------------------------*/
	fh_local->fname = (char *)malloc( sizeof(char) * (strlen(filename) + 1) );
	local_filename = (char *)(fh_local->fname);
	strcpy( local_filename, filename );

	/*------------------------------------------------------------------*/
	/* File pointers are defined in UPC-IO only                         */
	/* remove them before passing to the ADIO interface                 */
	/*------------------------------------------------------------------*/
	fh_local->flags = flags;
	flags = flags & ~UPC_INDIVIDUAL_FP;
	flags = flags & ~UPC_COMMON_FP;

	/*------------------------------------------------------------------*/
	/* Setup metadata                                                   */
	/*------------------------------------------------------------------*/
	fh_local->async_flag = 0;
	fh_local->async_op = __REF_UPC_NONE_ASYNC;
        fh_local->file_view = NULL;

	/*------------------------------------------------------------------*/
	/* Thread 0 opens the file first                                    */
	/*------------------------------------------------------------------*/
	if (!MYTHREAD) {
	  UPC_ADIO_Open( &fd, filename,
			 flags, mode, 
			 &error_code );
	}
	
	upc_barrier;

	/*------------------------------------------------------------------*/
	/* calling the UPC_ADIO_Open function                               */
	/*------------------------------------------------------------------*/
	if (MYTHREAD) {
	  UPC_ADIO_Open( &fd, filename,
			 flags, mode, 
			 &error_code );
	}

	if(error_code != UPC_ADIO_SUCCESS)
	{
		/* Error handling here */
		free( local_filename );
		upc_free( fh->th[MYTHREAD] );
		if( !MYTHREAD )
			upc_free( fh );

		return NULL;
	}

	fh_local->adio_fd = fd;

	/*------------------------------------------------------------------*/
	/* Reset the file pointers                                          */
	/*------------------------------------------------------------------*/
	if(flags & UPC_APPEND)
	{
		UPC_ADIO_GetSize(fh_local->adio_fd, filename, 
				 &offset, &error_code);
		if(error_code != UPC_ADIO_SUCCESS)
		{
			/* Error handling here */
			free( local_filename );
			upc_free( fh->th[MYTHREAD] );
			if( !MYTHREAD )
				upc_free( fh );

			return NULL;
		}

		fh_local->private_pointer = offset;
		fh_local->shared_pointer = offset;
	}
	else
	{
		fh_local->private_pointer = 0;
		fh_local->shared_pointer = 0;
	}

	return fh;
}

