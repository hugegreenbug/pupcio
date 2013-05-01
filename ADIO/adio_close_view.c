/************************************************************************/
/*                       UPC-ADIO Implementation                        */
/*                                                                      */
/*  This implementation is an ADIO for UPC. For more information about  */
/*  the ADIO definition or for licence information, please refer to     */
/*  the README file.                                                    */
/*                                                                      */
/*              HPCL, The George Wasnington University                  */
/*                        Author: Yiyi Yao                              */
/*                       E-mail: yyy@gwu.edu                            */
/*                                                                      */
/*          Modified for PLFS by: Hugh Greenberg <hng@lanl.gov>         */
/************************************************************************/

// adio_close.c
// ADIO using PLFS
#include <fcntl.h>
#include "adio.h"

//Returns the number of references to the Plfs_fd remaining after the close
int UPC_ADIO_CloseView ( Plfs_fd *fd,
			 int open_flags,
			 int *error_code,
			 upc_file_view *file_view )
{
	int err;
	uid_t uid;

	uid = getuid();

	err = plfs_upc_close( fd , (uint32_t) MYTHREAD, uid, open_flags, NULL, file_view);
	upc_barrier;	
	*error_code = UPC_ADIO_SUCCESS;

	return err;
}
