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
// upc_fwait_async.c
#include <upc.h>
#include <stdlib.h>
#include <stdio.h>
#include "upc_io.h"

int64_t upc_all_fwait_async( upcio_file_t *fh_shared )
{
	int i, j, pos;
	int start_th, my_th;
	int round, roundsize;
	shared unsigned char * buffer_char;
	shared int *global_error_code;
	int count, blocksize_byte;
	struct __struct_thread_upc_file_t *fh;
	int error_code;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if( fh_shared == NULL )
		return -1;

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh = (struct __struct_thread_upc_file_t *)(fh_shared->th[MYTHREAD]);

	/*------------------------------------------------------------------*/
	/* make sure there is asynchrounouse ops pending                    */
	/*------------------------------------------------------------------*/
	if(fh->async_flag == 0)
		return -1;

	/*------------------------------------------------------------------*/
	/* Perform test first                                               */
	/* decide if the pending asycn op has finished or not               */
	/* This is done based on different async op cases                   */
	/*------------------------------------------------------------------*/
	upc_barrier;
	switch( fh->async_op )
	{
	case __REF_UPC_READ_LOCAL_ASYNC:
		UPC_ADIO_Done( &fh->request, &error_code );
		upc_barrier;
		break;
	case __REF_UPC_READ_SHARED_ASYNC:
		if( fh->flags & UPC_INDIVIDUAL_FP )
		{
			UPC_ADIO_Done( &fh->request, &error_code );
			upc_barrier;
			if( fh->blocksize )
			{
				roundsize = fh->blocksize * THREADS;
				buffer_char = (shared unsigned char *)fh->sh_ptr;
				start_th = upc_threadof( buffer_char );

 				my_th = start_th;
				round = 0;
				for(i=0; i<fh->size-(fh->size%fh->blocksize); i+=fh->blocksize)
				{
					upc_memput(buffer_char+round*roundsize+my_th-start_th, &fh->local_ptr[i], fh->blocksize);
					my_th++;
					if(my_th == THREADS)
					{
						my_th = 0;
						round++;
					}
				}
				upc_memput(buffer_char+round*roundsize+my_th-start_th, &fh->local_ptr[i], fh->size-i);

				free( fh->local_ptr );
			}
			else
			{
				upc_memput(fh->sh_ptr, fh->local_ptr, fh->size);
				free( fh->local_ptr );
			}
		}
		else
		{
			if( fh->blocksize )
			{
				UPC_ADIO_Done( &fh->request, &error_code );
				free( fh->disparray );
				free( fh->dispsize );
			}
			else
			{
				global_error_code = (shared int *)upc_all_alloc(1, sizeof(int));
				//if( MYTHREAD == upc_threadof(fh->sh_ptr) )
				if( MYTHREAD == 0 )
				{
					UPC_ADIO_Done( &fh->request, &error_code );
					global_error_code[0] = error_code;
					upc_memput(fh->sh_ptr, fh->local_ptr, fh->size);
					free( fh->local_ptr );
				}

				/* Broadcast error_code */
				upc_barrier;
				error_code = global_error_code[0];
				upc_barrier;
				if(MYTHREAD == 0)
					upc_free(global_error_code);
			}
		}
		break;
	case __REF_UPC_WRITE_LOCAL_ASYNC:
		UPC_ADIO_Done( &fh->request, &error_code );
		upc_barrier;
		break;
	case __REF_UPC_WRITE_SHARED_ASYNC:
		if( fh->flags & UPC_INDIVIDUAL_FP )
		{
			UPC_ADIO_Done( &fh->request, &error_code );
			upc_barrier;
			free( fh->local_ptr );
		}
		else
		{
			if( fh->blocksize )
			{
				UPC_ADIO_Done( &fh->request, &error_code );
				upc_barrier;
				free( fh->disparray );
				free( fh->dispsize );
			}
			else
			{
				global_error_code = (shared int *)upc_all_alloc(1, sizeof(int));
				//if( MYTHREAD == upc_threadof(fh->sh_ptr) )
				if( MYTHREAD == 0 )
				{
					UPC_ADIO_Done( &fh->request, &error_code );
					global_error_code[0] = error_code;
					free( fh->local_ptr );
				}

				/* Broadcast error_code */
				upc_barrier;
				error_code = global_error_code[0];
				upc_barrier;
				if(MYTHREAD == 0)
					upc_free(global_error_code);
			}
		}
		break;

	case __REF_UPC_READ_LIST_LOCAL_ASYNC:
		UPC_ADIO_Done( &fh->request, &error_code );
		upc_barrier;
		free( fh->mem_ptrs );
		free( fh->mem_lengths );
		free( fh->file_offsets );
		free( fh->file_lengths );
		break;
	case __REF_UPC_READ_LIST_SHARED_ASYNC:
		UPC_ADIO_Done( &fh->request, &error_code );
		upc_barrier;
		pos = 0;
		for( i=0; i<fh->num_memvec; i++ )
		{
			if( !fh->shared_memvec[i].blocksize )
			{
				upc_memput( fh->shared_memvec[i].baseaddr, 
					    (const unsigned char *)&fh->local_ptr[pos], fh->shared_memvec[i].len );
			}
			else
			{
				blocksize_byte = fh->shared_memvec[i].blocksize;
				count = fh->shared_memvec[i].len;
				roundsize = blocksize_byte * THREADS;
				buffer_char = (shared unsigned char *)(fh->shared_memvec[i].baseaddr);
				start_th = upc_threadof( buffer_char );

				my_th = start_th;
				round = 0;
				for( j=0; j<count-(count%blocksize_byte); j+=blocksize_byte)
				{
					upc_memput(buffer_char+round*roundsize+my_th-start_th, &fh->local_ptr[pos+j], blocksize_byte);
					my_th++;
					if(my_th == THREADS)
					{
						my_th = 0;
						round++;
					}
				}
				upc_memput(buffer_char+round*roundsize+my_th-start_th, &fh->local_ptr[pos+j], count-j);
			}
			pos += fh->shared_memvec[i].len;
		}
		free(fh->local_ptr);
		free( fh->file_offsets );
		free( fh->file_lengths );
		break;
	case __REF_UPC_WRITE_LIST_LOCAL_ASYNC:
		UPC_ADIO_Done( &fh->request, &error_code );
		upc_barrier;
		free( fh->mem_ptrs );
		free( fh->mem_lengths );
		free( fh->file_offsets );
		free( fh->file_lengths );
		break;
	case __REF_UPC_WRITE_LIST_SHARED_ASYNC:
		UPC_ADIO_Done( &fh->request, &error_code );
		upc_barrier;
		free(fh->local_ptr);
		free( fh->mem_lengths );
		free( fh->file_offsets );
		free( fh->file_lengths );
		break;
	default:
		break;
	}

	upc_barrier;
	/*------------------------------------------------------------------*/
	/* clean the pending async op flag                                  */
	/*------------------------------------------------------------------*/
	fh->async_op = __REF_UPC_WAIT_ASYNC;
	fh->async_flag = 0;
	if( error_code != UPC_ADIO_SUCCESS )
		return -1;

	return fh->size;
}

