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
// upc_fsync.c
#include <upc.h>
#include "upc_io.h"

int upc_all_fsync( upcio_file_t *fh_shared )
{
	int error_code;
	Plfs_fd *fd;
	struct __struct_thread_upc_file_t *fh;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if(fh_shared == NULL)
	{
#ifdef DEBUG
		printf("sync: invalid file handler\n");
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
	/* make sure there is no asynchrounouse ops pending                 */
	/*------------------------------------------------------------------*/
	if( fh->async_flag == 1 )
	{
#ifdef DEBUG
		if( !MYTHREAD )
			printf("sync: pending async ops\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* call ADIO_Flush to sync the file handler                         */
	/*------------------------------------------------------------------*/
	UPC_ADIO_Flush( fd, &error_code );
	upc_barrier;
	if(error_code != UPC_ADIO_SUCCESS)
	{
#ifdef DEBUG
		printf("sync: sync error\n");
#endif
		return -1;
	}

	return 0;
}
