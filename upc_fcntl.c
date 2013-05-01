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
// upc_fcntl.c
#include <upc.h>
#include "upc_io.h"

int upc_all_fcntl(upcio_file_t *fh_shared, int cmd, void *arg)
{
	struct __struct_thread_upc_file_t *fh;
	Plfs_fd *fd;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if( fh_shared == NULL)
		return -1;

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh = (struct __struct_thread_upc_file_t *)fh_shared->th[MYTHREAD];
	fd = (Plfs_fd *)fh->adio_fd;

	switch( cmd )
	{
	case UPC_GET_CA_SEMANTICS:
		if( fh->async_flag == 1 )
			return -1;
		if( fh->flags & UPC_STRONG_CA )
			return UPC_STRONG_CA;
		break;
	case UPC_SET_WEAK_CA_SEMANTICS:
		if( fh->async_flag == 1 )
			return -1;
		/*------------------------------------------------------------------*/
		/* file sync and an implicit barrier here                           */
		/*------------------------------------------------------------------*/
		upc_all_fsync( fh_shared );
		/* Set the semantics to be weak */
		fh->flags = fh->flags & ~UPC_STRONG_CA;
		break;
	case UPC_SET_STRONG_CA_SEMANTICS:
		if( fh->async_flag == 1 )
			return -1;
		/*------------------------------------------------------------------*/
		/* file sync and an implicit barrier here                           */
		/*------------------------------------------------------------------*/
		upc_all_fsync( fh_shared );
		/* Set the semantics to be strong */
		fh->flags = fh->flags | UPC_STRONG_CA;
		break;
	case UPC_GET_FP:
		if( fh->async_flag == 1 )
			return -1;
		if( fh->flags & UPC_COMMON_FP )
			return UPC_COMMON_FP;
		else
			return UPC_INDIVIDUAL_FP;
		break;
	case UPC_SET_COMMON_FP:
		if( fh->async_flag == 1 )
			return -1;
		/*------------------------------------------------------------------*/
		/* file sync and an implicit barrier here                           */
		/*------------------------------------------------------------------*/
		upc_all_fsync( fh_shared );
		if( fh->flags & UPC_INDIVIDUAL_FP )
		{
			fh->flags = fh->flags ^ UPC_INDIVIDUAL_FP;
			fh->flags = fh->flags ^ UPC_COMMON_FP;
		}

		fh->shared_pointer = 0;
		break;
	case UPC_SET_INDIVIDUAL_FP:
		if( fh->async_flag == 1 )
			return -1;
		/*------------------------------------------------------------------*/
		/* file sync and an implicit barrier here                           */
		/*------------------------------------------------------------------*/
		upc_all_fsync( fh_shared );
		if( fh->flags &  UPC_COMMON_FP)
		{
			fh->flags = fh->flags ^ UPC_COMMON_FP;
			fh->flags = fh->flags ^ UPC_INDIVIDUAL_FP;
		}

		fh->private_pointer = 0;
		break;
	case UPC_GET_FL:
		return fh->flags;
		break;
	case UPC_GET_FN:
		arg = (char *)(fh->fname);
		break;
	case UPC_GET_HINTS:
		break;
	case UPC_SET_HINT:
		break;
	case UPC_ASYNC_OUTSTANDING:
		return fh->async_flag;
		break;
	}

	return 0;
}
