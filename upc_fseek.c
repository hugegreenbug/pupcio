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
// upc_fseek.c
#include <stdio.h>
#include <upc.h>
#include "upc_io.h"

int64_t upc_all_fseek(upcio_file_t *fh, upc_off_t offset, int origin)
{
	int error_code;
	Plfs_fd *fd;
	upc_off_t file_size;
	struct __struct_thread_upc_file_t *fh_local;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if(fh == NULL)
		return -1;

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh_local = (struct __struct_thread_upc_file_t *)fh->th[MYTHREAD];
	fd = (Plfs_fd *)fh_local->adio_fd;

	/*------------------------------------------------------------------*/
	/* shared pointer first                                             */
	/*------------------------------------------------------------------*/
	if( fh_local->flags & UPC_COMMON_FP )
	{
		switch(origin)
		{
		case UPC_SEEK_SET:
			fh_local->shared_pointer = offset;
			break;
		case UPC_SEEK_CUR:
			fh_local->shared_pointer += offset;
			break;
		case UPC_SEEK_END:
			UPC_ADIO_GetSize( fd, fh_local->fname, &file_size, &error_code );
			if(error_code != UPC_ADIO_SUCCESS)
				return -1;
			fh_local->shared_pointer = file_size + offset;
			break;
		default:
			if(MYTHREAD == 0)
				fprintf(stderr, "UPC-IO: Must specify a valid Origin offset for upc_all_fseek function\n");
			return -1;
		}

		/*------------------------------------------------------------------*/
		/* file pointer can not go pass the begining of the file            */
		/*------------------------------------------------------------------*/
		if( fh_local->shared_pointer < 0 )
		{
			fh_local->shared_pointer = 0;
			return -1;
		}

		return fh_local->shared_pointer;
	}
	/*------------------------------------------------------------------*/
	/* then private pointer                                             */
	/*------------------------------------------------------------------*/
	else
	{
		switch(origin)
		{
		case UPC_SEEK_SET:
			fh_local->private_pointer = offset;
			break;
		case UPC_SEEK_CUR:
			fh_local->private_pointer += offset;
			break;
		case UPC_SEEK_END:
			UPC_ADIO_GetSize( fd, fh_local->fname, &file_size, &error_code );
			if(error_code != UPC_ADIO_SUCCESS)
				return -1;
			fh_local->private_pointer = file_size + offset;
			break;
		default:
			if(MYTHREAD == 0)
				fprintf(stderr, "UPC-IO: Must specify a valid Origin offset for upc_all_fseek function\n");
			return -1;
		}

		/*------------------------------------------------------------------*/
		/* file pointer can not go pass the begining of the file            */
		/*------------------------------------------------------------------*/
		if( fh_local->private_pointer < 0 )
		{
			fh_local->private_pointer = 0;
			return -1;
		}

		return fh_local->private_pointer;
	}
}

