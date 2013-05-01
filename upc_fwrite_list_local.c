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
// upc_fwrite_list_local.c
#include <upc.h>
#include <stdlib.h>
#include "upc_io.h"

int64_t upc_all_fwrite_list_local( upcio_file_t *fh_shared,
                                     uint32_t memvec_entries,
                                     struct upc_io_local_memvec const *memvec,
                                     uint32_t filevec_entries,
                                     struct upc_io_filevec const *filevec,
                                     upc_flag_t sync_mode )
{
	int error_code;
	uint32_t i;
	upc_off_t size;
	void ** mem_ptrs;
	upc_off_t *file_offsets;
	upc_off_t *mem_lengths;
	upc_off_t *file_lengths;
	struct __struct_thread_upc_file_t *fh;
	Plfs_fd *fd;

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
	fd = (Plfs_fd *)fh->adio_fd;

	/*------------------------------------------------------------------*/
	/* make sure the file is not opened with read only                  */
	/*------------------------------------------------------------------*/
	if( fh->flags & UPC_RDONLY )
		return -1;

	/*------------------------------------------------------------------*/
	/* make sure there is no asynchrounouse ops pending                 */
	/*------------------------------------------------------------------*/
	if( fh->async_flag == 1 )
		return -1;

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
	mem_ptrs = (void **)malloc(sizeof(void *)*memvec_entries);
	mem_lengths = (upc_off_t *)malloc(sizeof(upc_off_t)*memvec_entries);
	file_offsets = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);
	file_lengths = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);
	if( !mem_ptrs || !mem_lengths || !file_offsets || !file_lengths )
		return -1;

	size = 0;
	for( i=0; i<memvec_entries; i++ )
	{
		mem_ptrs[i] = memvec[i].baseaddr;
		mem_lengths[i] = memvec[i].len;
		size += memvec[i].len;
	}

	for( i=0; i<filevec_entries; i++ )
	{
		file_offsets[i] = filevec[i].offset;
		file_lengths[i] = filevec[i].len;
	}

	UPC_ADIO_WriteStrided( fd, memvec_entries, mem_ptrs, mem_lengths, filevec_entries, file_offsets, 
			       file_lengths, &error_code );

	free( mem_ptrs );
	free( mem_lengths );
	free( file_offsets );
	free( file_lengths );

	/*------------------------------------------------------------------*/
	/* upc sync mode                                                    */
	/*------------------------------------------------------------------*/
    if( sync_mode & UPC_IN_NOSYNC )
        ;
    else if( sync_mode & UPC_IN_MYSYNC )
        upc_barrier;
    else
        upc_barrier;

	return size;
}



