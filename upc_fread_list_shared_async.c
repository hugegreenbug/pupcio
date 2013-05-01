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
// upc_fread_list_shared_async.c
#include <upc.h>
#include <stdlib.h>
#include "upc_io.h"

void upc_all_fread_list_shared_async( upcio_file_t *fh_shared,
                                      uint32_t memvec_entries,
                                      struct upc_io_shared_memvec const *memvec,
                                      uint32_t filevec_entries,
                                      struct upc_io_filevec const *filevec,
				      int64_t *ret,
                                      upc_flag_t sync_mode )
{
	int error_code;
	uint32_t i, j, pos;
	upc_off_t size;
	unsigned char *buf;
	UPC_ADIO_Request request;
	struct __struct_thread_upc_file_t *fh;
	Plfs_fd *fd;
	upc_off_t * file_offsets;
	upc_off_t *file_lengths;

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
	/* make sure the file is not opened with write only                 */
	/*------------------------------------------------------------------*/
	if( fh->flags & UPC_WRONLY )
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

	/* Read the strides from the file */
	size = 0;
	file_offsets = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);
	file_lengths = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);
	if( !file_offsets || !file_lengths )
		return;

	for( i=0; i<filevec_entries; i++ )
	{
		size += filevec[i].len;
		file_offsets[i] = filevec[i].offset;
		file_lengths[i] = filevec[i].len;
	}

	buf = (unsigned char *)malloc(size*sizeof(unsigned char));
	if( !buf )
		return;

	UPC_ADIO_IreadStrided( fd, 1, buf, &size, filevec_entries, file_offsets, 
			       file_lengths, &request, ret, &error_code );

	/*------------------------------------------------------------------*/
	/* Setup metadata                                                   */
	/*------------------------------------------------------------------*/
	fh->request = request;
	fh->size = size;
	fh->async_op = __REF_UPC_READ_LIST_SHARED_ASYNC;
	fh->local_ptr = buf;
	fh->num_memvec = memvec_entries;
	fh->shared_memvec = memvec;
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



