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

// adio_open.c
// ADIO using PLFS
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "adio.h"

int UPC_ADIO_ViewOpen ( Plfs_fd **fd, const char *filename,
			int access_mode,
			mode_t mode,
			int *error_code,
			upc_file_view **file_view)
{
	int err;

	err = plfs_upc_open(fd, filename, access_mode, (uint32_t) MYTHREAD, 
			    mode, NULL, file_view);

        if ( err == -1 || fd == NULL || *fd == NULL)
	{
		*error_code = UPC_ADIO_FAILURE;
		return -1;
	}
	else
	{
		*error_code = UPC_ADIO_SUCCESS;
		return 0;
	}
}
