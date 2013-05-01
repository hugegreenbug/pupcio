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
// upc_fwrite_list_shared_async.c
#include <upc.h>
#include <stdlib.h>
#include "upc_io.h"

void upc_all_fwrite_list_shared_async( upcio_file_t *fh_shared,
                                       uint32_t memvec_entries,
                                       struct upc_io_shared_memvec const *memvec,
                                       uint32_t filevec_entries,
                                       struct upc_io_filevec const *filevec,
				       int64_t *ret,
                                       upc_flag_t sync_mode )
{
	int error_code;
	uint32_t i, j, pos;
	unsigned char *buf;
	UPC_ADIO_Request request;
	upc_off_t round, roundsize;
	uint32_t start_th;
	uint32_t my_th;
	upc_off_t count, blocksize_byte;
	shared unsigned char * buffer_char;
	struct __struct_thread_upc_file_t *fh;
	Plfs_fd *fd;
	upc_off_t * file_offsets;
	upc_off_t * file_lengths;
	upc_off_t size;

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

	/* Read the strides from source  */
	file_offsets = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);
	file_lengths = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);

	if( !file_offsets || !file_lengths )
		return;

	size = 0;
	for( i=0; i<filevec_entries; i++ )
	{
		file_offsets[i] = filevec[i].offset;
		file_lengths[i] = filevec[i].len;
		size += memvec[i].len;
	}

	buf = (unsigned char *)malloc(size*sizeof(unsigned char));
	if( !buf )
		return;

	/* an alternative way to implement this function will be realloc() */
	pos = 0;
	for( i=0; i<memvec_entries; i++ )
	{		
		if( !memvec[i].blocksize )
		{
			upc_memget( &buf[pos], memvec[i].baseaddr, memvec[i].len );
		}
		else
		{
			blocksize_byte = memvec[i].blocksize;
			count = memvec[i].len;
			roundsize = blocksize_byte * THREADS;
			buffer_char = (shared unsigned char *)(memvec[i].baseaddr);
			start_th = upc_threadof( buffer_char );

			my_th = start_th;
			round = 0;
			for( j=0; j<count-(count%blocksize_byte); j+=blocksize_byte)
			{
				upc_memget(&buf[pos+j], buffer_char+round*roundsize+my_th-start_th, blocksize_byte);
				my_th++;
				if(my_th == THREADS)
				{
					my_th = 0;
					round++;
				}
			}
			upc_memget(&buf[pos+j], buffer_char+round*roundsize+my_th-start_th, count-j);
		}
		pos += memvec[i].len;
	}

	UPC_ADIO_IwriteStrided( fd, 1, buf, &size, filevec_entries, 
				file_offsets, file_lengths, &request, ret, 
				&error_code );

	/*------------------------------------------------------------------*/
	/* Setup metadata                                                   */
	/*------------------------------------------------------------------*/
	fh->async_op = __REF_UPC_WRITE_LIST_SHARED_ASYNC;
	fh->size = size;
	fh->local_ptr = buf;
	fh->request = request;
	fh->file_offsets = file_offsets;
	fh->file_lengths = file_lengths;

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



