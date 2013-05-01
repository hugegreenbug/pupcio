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
/*         Modified for PLFS by: Hugh Greenberg <hng@lanl.gov>          */
/************************************************************************/

// adio_flush.c
// ADIO using PLFS
#include "adio.h"

void UPC_ADIO_Flush ( Plfs_fd *fd,
                      int *error_code )
{
	int err;

	err = plfs_sync(fd);

	upc_barrier;
	if( err == -1 )
		*error_code = UPC_ADIO_FAILURE;
	else
		*error_code = UPC_ADIO_SUCCESS;

	return;
}
