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
// upc_fwrite_shared.c
#include <upc.h>
#include <stdlib.h>
#include "upc_io.h"

void upc_all_fwrite_shared_async( upcio_file_t *fh_shared,
                                  shared void *buffer,
                                  uint32_t blocksize,
                                  upc_off_t size,
                                  uint32_t nmemb,
				  int64_t *ret,
                                  upc_flag_t sync_mode )
{
	Plfs_fd *fd;
	UPC_ADIO_Request request;
	unsigned char *local_buf;
	upc_off_t count, blocksize_byte, roundsize;
	uint32_t round, i;
	upc_off_t start_th, my_th;
	int error_code;
	shared unsigned char * buffer_char;
	struct __struct_thread_upc_file_t *fh;
	upc_off_t *dispsize;
	upc_off_t *disparray;
	upc_off_t disp;
	upc_off_t extra;
	uint32_t extra_block;
	upc_off_t mpi_size;
	upc_off_t nblocks;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if( fh_shared == NULL )
		return;

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh = (struct __struct_thread_upc_file_t *)(fh_shared->th[MYTHREAD]);
	fd = (Plfs_fd *)fh->adio_fd;

	/*------------------------------------------------------------------*/
	/* make sure the file is not opened with read only                  */
	/*------------------------------------------------------------------*/
	if( fh->flags & UPC_RDONLY )
		return;

	/*------------------------------------------------------------------*/
	/* make sure there is no asynchrounouse ops pending                 */
	/*------------------------------------------------------------------*/
	if( fh->async_flag == 1 )
		return;

	/*------------------------------------------------------------------*/
	/* set the asynchrounouse ops flag                                  */
	/*------------------------------------------------------------------*/
	fh->async_flag = 1;

	/*------------------------------------------------------------------*/
	/* upc sync mode                                                    */
	/*------------------------------------------------------------------*/
	if( sync_mode & UPC_IN_NOSYNC )
		;
	else if( sync_mode & UPC_IN_MYSYNC )
		upc_barrier;
	else
		upc_barrier;

	count = size*nmemb;
	blocksize_byte = blocksize*size;

	if( fh->flags & UPC_INDIVIDUAL_FP )
	{
		if( blocksize )
		{
			roundsize = blocksize_byte * THREADS;
			buffer_char = (shared unsigned char *)buffer;
			start_th = upc_threadof( buffer_char );

			local_buf=(unsigned char *)malloc(sizeof(unsigned char)*count);
			my_th = start_th;
			round = 0;
			for(i=0; i<count-(count%blocksize_byte); i+=blocksize_byte)
			{
				upc_memget(&local_buf[i], buffer_char+round*roundsize+my_th-start_th, blocksize_byte);
				my_th++;
				if(my_th == THREADS)
				{
					my_th = 0;
					round++;
				}
			}
			upc_memget(&local_buf[i], buffer_char+round*roundsize+my_th-start_th, count-i);
			UPC_ADIO_IwriteContig( fd, local_buf, count, fh->private_pointer, &request, 
					       ret, &error_code );
		}
		else
		{
			local_buf = (unsigned char *)malloc(sizeof(unsigned char)*count);
			upc_memget(local_buf, buffer, count);
			UPC_ADIO_IwriteContig( fd, local_buf, count, fh->private_pointer, &request, 
					       ret, &error_code );
		}
		/*------------------------------------------------------------------*/
		/* increment the file pointer                                       */
		/*------------------------------------------------------------------*/
		fh->private_pointer += count;
	}
	else
	{
		if( blocksize )
		{
			buffer_char = (shared unsigned char *)buffer;
			start_th = upc_threadof( buffer_char );
			roundsize = blocksize_byte * THREADS;
			round = (uint32_t)(count / roundsize);
			nblocks = (uint32_t)(count / blocksize_byte);
			extra_block = nblocks%THREADS;
			extra = count % blocksize_byte;
			mpi_size = round * blocksize_byte;
			round++;
			if( MYTHREAD < start_th )       /* wrap around */
			{
				my_th = MYTHREAD + THREADS - start_th;
				local_buf = (unsigned char *)(buffer_char + roundsize - start_th +MYTHREAD );
			}
			else
			{
				my_th = MYTHREAD - start_th;
				local_buf = (unsigned char *)(buffer_char + my_th);
			}

			if( my_th < nblocks%THREADS )
				mpi_size += blocksize_byte;
			if( my_th == extra_block )
				mpi_size += extra;

			disp = fh->shared_pointer + blocksize_byte * my_th;
			disparray = (upc_off_t *)malloc(round*sizeof(upc_off_t));
			dispsize = (upc_off_t *)malloc(round*sizeof(upc_off_t));
			for( i=0; i<round; i++ )
			{
				disparray[i] = disp + i * roundsize;
				dispsize[i] = blocksize_byte;
			}

			UPC_ADIO_IwriteStrided( fd, 1, &local_buf, &mpi_size, round, disparray, dispsize, &request, 
						ret, &error_code );
			/*------------------------------------------------------------------*/
			/* update the metadata                                              */
			/*------------------------------------------------------------------*/
			fh->disparray = disparray;
			fh->dispsize = dispsize;
		}
		else
		{
			//if( MYTHREAD == upc_threadof(buffer) )
			if( MYTHREAD == 0 )
			{
				local_buf = (unsigned char *)malloc(sizeof(unsigned char)*count);
				upc_memget(local_buf, buffer, count);
				UPC_ADIO_IwriteContig( fd, local_buf, count, fh->shared_pointer, &request, 
						       ret, &error_code );
			}
		}

		/*------------------------------------------------------------------*/
		/* increment the file pointer                                       */
		/*------------------------------------------------------------------*/
		fh->shared_pointer += count;
	}

	/*------------------------------------------------------------------*/
	/* update the metadata                                              */
	/*------------------------------------------------------------------*/
	fh->request = request;
	fh->async_op = __REF_UPC_WRITE_SHARED_ASYNC;
	fh->local_ptr = local_buf;
	fh->blocksize = blocksize_byte;
	fh->size = count;

	/*------------------------------------------------------------------*/
	/* upc sync mode                                                    */
	/*------------------------------------------------------------------*/
	if( sync_mode & UPC_IN_NOSYNC )
		;
	else if( sync_mode & UPC_IN_MYSYNC )
		upc_barrier;
	else
		upc_barrier;

	return;
}
