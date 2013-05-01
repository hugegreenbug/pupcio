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

// adio_read_contig.c
// ADIO using PLFS
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "adio.h"

/*
  Reads from the given Plfs_fd
  Returns the size read
 */
int64_t UPC_ADIO_ReadContig (
       Plfs_fd *fd, 
       void *buf, 
       uint32_t len, 
       uint64_t offset, 
       int *error_code ) 
{
    int64_t err = 0;

    if ( err == -1 )
    {
	*error_code = UPC_ADIO_FAILURE;
        return 0;
    }

    err = plfs_read( fd, buf, len, offset );
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
