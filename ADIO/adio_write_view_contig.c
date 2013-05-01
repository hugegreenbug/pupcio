/************************************************************************/
/*                       UPC-ADIO Implementation                        */
/*                                                                      */
/*  This implementation is an ADIO for UPC. For more information about  */
/*  the ADIO definition or for licence information, please refer to     */
/*  the README file.                                                    */
/*                                                                      */
/*              HPCL, The George Wasnington University                  */
/*                        Author: Kun Xi				*/
/*                       E-mail: kunxi@gwu.edu                          */
/*                                                                      */
/*         Modified for PLFS by: Hugh Greenberg <hng@lanl.gov>          */
/************************************************************************/

// adio_write_contig.c
// ADIO using PLFS
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>
#include "adio.h"

int64_t UPC_ADIO_WriteViewContig (
	Plfs_fd *fd, 
	void *buf, 
	uint32_t count, 
	int *error_code, 
	upc_file_view *file_view ) 
{
	int64_t err;
	int unused;

	err = plfs_upc_write( fd, buf, (uint32_t) MYTHREAD, count, file_view);

	if( err == -1 )
	{
		*error_code = UPC_ADIO_FAILURE;
		return 0;
	}
	else
	{
		*error_code = UPC_ADIO_SUCCESS;
		return err;
	}
}
