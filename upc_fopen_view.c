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

upcio_file_t * upc_all_fopen_view( const char *filename,
				   int flags, mode_t mode, 
				   upc_file_view **file_view, 
				   int populate )
{
	int error_code;
	shared struct __struct_upc_file_t * fh;
	struct __struct_thread_upc_file_t * fh_local;
	char * local_filename;
	upc_off_t offset;
	Plfs_fd *fd = NULL;
	upc_file_view_shared *shared_file_view;
	shared int *count;
	int lcount;

	/*------------------------------------------------------------------*/
	/* assign the global upc file handler                               */
	/* here I have to use the complex struct name because the upc_alloc */
	/* has to return a pointer with "shared" qualifier                  */
	/*------------------------------------------------------------------*/
	fh = (shared struct __struct_upc_file_t *)
		upc_all_alloc(1, sizeof(struct __struct_upc_file_t));
	if(fh == NULL)
		return NULL;
	/*------------------------------------------------------------------*/
	/* assign the local file handler for each thread                    */
	/* again I have to use the complex struct name because the upc_alloc*/
	/* has to return a pointer with "shared" qualifier                  */
	/*------------------------------------------------------------------*/
	fh->th[MYTHREAD] = (shared struct __struct_thread_upc_file_t *)
		upc_alloc(sizeof(struct __struct_thread_upc_file_t));
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
	/*------------------------------------------------------------------*/
	/* Thread 0 opens the file first                                    */
	/*------------------------------------------------------------------*/


	fh_local->free_view_in_plfs = 0;
	if (populate) {
		count = upc_all_alloc(1, sizeof(int));
		upc_barrier;
		if (!MYTHREAD) {
			if (!*file_view) {
				fh_local->free_view_in_plfs = 1;
			}

			UPC_ADIO_ViewOpen( &fd, filename,
					   flags, mode,
					   &error_code, file_view );
			if (file_view == NULL) {
				printf("TH%d: Error populating file view\n", MYTHREAD);
				upc_global_exit(-1);
			}
		  
			upc_memput(count, &(*file_view)->count, sizeof(uint32_t));
		}
	  
		upc_barrier;
		upc_memget(&lcount, count, sizeof(int));
		upc_barrier;
	  
		if (lcount == 0) {
			printf("TH%d: Received zero for the count\n", MYTHREAD);
			upc_global_exit(-1);
		}

		shared_file_view = upc_all_alloc(1, sizeof(upc_file_view));
		shared_file_view->block_lengths = upc_all_alloc(1, lcount * sizeof(uint32_t));
		shared_file_view->block_displacements = upc_all_alloc(1, lcount * sizeof(uint32_t));
		shared_file_view->types = upc_all_alloc(1, lcount * sizeof(uint32_t));
		upc_barrier;

		if (!MYTHREAD) {	 
//			upc_memput(shared_file_view, file_view, sizeof(upc_file_view));
			upc_memput(shared_file_view->block_lengths, (*file_view)->block_lengths, 
				   sizeof(uint32_t) * (*file_view)->count);
			upc_memput(shared_file_view->block_displacements, (*file_view)->block_displacements, 
				   sizeof(uint32_t) * (*file_view)->count);
			upc_memput(shared_file_view->types, (*file_view)->types, sizeof(uint32_t) * (*file_view)->count);
			upc_memput(&shared_file_view->elem_type, &(*file_view)->elem_type, sizeof(uint32_t));		       			
		}
      
		upc_barrier;
		if (MYTHREAD) {
			*file_view = malloc(sizeof(upc_file_view));
			upc_memget(*file_view, shared_file_view, sizeof(upc_file_view));
			(*file_view)->count = lcount;
			(*file_view)->block_lengths = malloc(lcount * sizeof(uint32_t));
			(*file_view)->block_displacements = malloc(lcount * sizeof(uint32_t));
			(*file_view)->types = malloc(lcount * sizeof(uint32_t));
			upc_memget((*file_view)->block_lengths, shared_file_view->block_lengths, sizeof(uint32_t) * lcount);
			upc_memget((*file_view)->block_displacements, shared_file_view->block_displacements, sizeof(uint32_t) * lcount);
			upc_memget((*file_view)->types, shared_file_view->types, sizeof(uint32_t) * lcount);
			UPC_ADIO_ViewOpen( &fd, filename,
					   flags, mode,
					   &error_code, file_view );
		}
		
		upc_barrier;
		if (!MYTHREAD) {
			upc_free(shared_file_view);
		}
	} else {
		UPC_ADIO_ViewOpen( &fd, filename,
				   flags, mode,
				   &error_code, file_view );
	}	 

	fh_local->file_view = *file_view;
	
	//	upc_barrier;
	/*------------------------------------------------------------------*/
	/* calling the UPC_ADIO_Open function                               */
	/*------------------------------------------------------------------*/
	//	if (MYTHREAD) {
	/*	  UPC_ADIO_ViewOpen( &fd, filename,
			     flags, mode,
			     &error_code, &fh_local->file_view );
	}
	
	upc_barrier;*/
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

