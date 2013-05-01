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
// upc_fwrite_local.c
#include <upc.h>
#include "upc_io.h"

int64_t upc_all_fwrite_view_local(upcio_file_t *fh_shared,
				  void *buffer,
				  uint32_t count,
				  upc_flag_t sync_mode,
				  upc_file_view *file_view)
{
	int error_code;
	Plfs_fd *fd;
	upc_off_t complete_size;
	struct __struct_thread_upc_file_t *fh;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if(fh_shared == NULL)
	{
#ifdef DEBUG
		printf("write_local: invalid file handlers\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh = (struct __struct_thread_upc_file_t *)fh_shared->th[MYTHREAD];
	fd = (Plfs_fd *)fh->adio_fd;

	/*------------------------------------------------------------------*/
	/* make sure the file is not opened with read only                  */
	/*------------------------------------------------------------------*/
	if( fh->flags & UPC_RDONLY )
	{
#ifdef DEBUG
		printf("write_local: read only file handler\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* make sure there is no asynchrounouse ops pending                 */
	/*------------------------------------------------------------------*/
	if( fh->async_flag == 1 )
	{
#ifdef DEBUG
		printf("write_local: pending async ops\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* upc sync mode                                                    */
	/*------------------------------------------------------------------*/
	if( sync_mode & UPC_IN_NOSYNC )
		;
	else if( sync_mode & UPC_IN_MYSYNC )
		upc_barrier;
	else
		upc_barrier;

	complete_size = UPC_ADIO_WriteViewContig( fd, buffer, count,
						  &error_code, file_view );
	if(error_code != UPC_ADIO_SUCCESS)
	{
#ifdef DEBUG
		printf("write_local: ADIO_WriteContig error\n");
#endif
		/* Error handling here */
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* upc sync mode                                                    */
	/*------------------------------------------------------------------*/
	if( sync_mode & UPC_IN_NOSYNC )
		;
	else if( sync_mode & UPC_IN_MYSYNC )
		upc_barrier;
	else
		upc_barrier;

	return complete_size;
}

