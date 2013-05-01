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
// upc_fread_local.c
#include <upc.h>
#include "upc_io.h"

int64_t upc_all_fread_local( upcio_file_t *fh_shared,
			     void *buffer,
			     upc_off_t size,
			     uint32_t nmemb,
			     upc_flag_t sync_mode )
{
	int error_code;
	Plfs_fd *fd;
	upc_off_t complete_size;
	struct __struct_thread_upc_file_t *fh;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if(fh_shared == NULL)
		return -1;

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh = (struct __struct_thread_upc_file_t *)fh_shared->th[MYTHREAD];
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

	complete_size = UPC_ADIO_ReadContig( fd, buffer, size*nmemb, fh->private_pointer, &error_code );
	if(error_code != UPC_ADIO_SUCCESS)
	{
		/* Error handling here */
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* increment the file pointer                                       */
	/*------------------------------------------------------------------*/
	fh->private_pointer += complete_size;

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



