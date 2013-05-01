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
// upc_fread_list_shared.c
#include <upc.h>
#include <stdlib.h>
#include "upc_io.h"

int64_t upc_all_fread_list_shared( upcio_file_t *fh_shared,
                                     uint32_t memvec_entries,
                                     struct upc_io_shared_memvec const *memvec,
                                     uint32_t filevec_entries,
                                     struct upc_io_filevec const *filevec,
                                     upc_flag_t sync_mode )
{
	int error_code;
	uint32_t i, j, pos;
	upc_off_t size;
	unsigned char *buf;
	upc_off_t roundsize;
	uint32_t round, start_th;
	uint32_t my_th;
	upc_off_t count, blocksize_byte;
	shared unsigned char *buffer_char;
	struct __struct_thread_upc_file_t *fh;
	Plfs_fd *fd;
	upc_off_t *file_offsets;
	upc_off_t *file_lengths;

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
	/* make sure the file is not opened with write only                 */
	/*------------------------------------------------------------------*/
	if( fh->flags & UPC_WRONLY )
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
	size = 0;
	file_offsets = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);
	file_lengths = (upc_off_t *)malloc(sizeof(upc_off_t)*filevec_entries);
	if( !file_offsets || !file_lengths )
		return -1;

	for( i=0; i<filevec_entries; i++ )
	{
		size += filevec[i].len;
		file_offsets[i] = filevec[i].offset;
		file_lengths[i] = filevec[i].len;
	}

	buf = (unsigned char *)malloc(size*sizeof(unsigned char));
	if( !buf )
		return -1;

	UPC_ADIO_ReadStrided( fd, 1, buf, &size, filevec_entries, file_offsets, file_lengths, &error_code );

	/* Write the strides to the destination */
	pos = 0;
	for( i=0; i<memvec_entries; i++ )
	{
		if( !memvec[i].blocksize )
		{
			upc_memput( memvec[i].baseaddr, (const unsigned char *)&buf[pos], memvec[i].len );
		}
		else
		{
			start_th = upc_threadof( memvec[i].baseaddr );
			blocksize_byte = memvec[i].blocksize;
			count = memvec[i].len;
			roundsize = blocksize_byte * THREADS;
			buffer_char = (shared unsigned char *)(memvec[i].baseaddr);
			//start_th = upc_threadof( buffer_char );

			my_th = start_th;
			round = 0;
			for( j=0; j<count-(count%blocksize_byte); j+=blocksize_byte)
			{
				upc_memput(buffer_char+round*roundsize+my_th-start_th, &buf[pos+j], blocksize_byte);
				my_th++;
				if(my_th == THREADS)
				{
					my_th = 0;
					round++;
				}
			}

			upc_memput(buffer_char+round*roundsize+my_th-start_th, &buf[pos+j], count-j);
		}
		pos += memvec[i].len;
	}

	free( buf );
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



