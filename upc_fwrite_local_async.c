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
// upc_fwrite_local_async.c
#include <upc.h>
#include "upc_io.h"

void upc_all_fwrite_local_async( upcio_file_t *fh_shared,
                                 void *buffer,
                                 upc_off_t size,
                                 uint32_t nmemb,
				 int64_t *ret,
                                 upc_flag_t sync_mode )
{
	UPC_ADIO_Request request;
        Plfs_fd *fd;
	upc_off_t count;
	struct __struct_thread_upc_file_t *fh;
	int error_code;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if(fh_shared == NULL)
		return;

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
	UPC_ADIO_IwriteContig( fd, buffer, count, fh->private_pointer, 
   		               &request, ret, &error_code );
	
	/*------------------------------------------------------------------*/
	/* update the metadata                                              */
	/*------------------------------------------------------------------*/
	fh->request = request;
	fh->async_op = __REF_UPC_WRITE_LOCAL_ASYNC;
	fh->size = count;

	/*------------------------------------------------------------------*/
	/* increment the file pointer                                       */
	/*------------------------------------------------------------------*/
	fh->private_pointer += count;

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
