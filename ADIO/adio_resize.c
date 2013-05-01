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

// adio_prealloc.c
// ADIO using PLFS
#include <sys/types.h>
#include <sys/stat.h>
#include "adio.h"

void UPC_ADIO_Resize( Plfs_fd *fd,
		      const char *path,
		      uint64_t size,
		      int *error_code )
{
	struct stat st;
	int err;
	
	if (path == NULL)
	{
		err = -1;
	}
	else {
		err = plfs_getattr(fd, path, &st, 0);
	}
	
	if( err )
	{
		*error_code = UPC_ADIO_FAILURE;
		return;
	} 

	/* only resize if request is not equal to what we currently have */
	if( size != st.st_size )
	{
		err = plfs_trunc(fd, path, size, 1);

		if( !err )
			*error_code = UPC_ADIO_SUCCESS;
		else
			*error_code = UPC_ADIO_FAILURE;
	}
	else
		*error_code = UPC_ADIO_SUCCESS;

	return;
}
