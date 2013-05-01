/************************************************************************/
/*                       UPC-ADIO Implementation                        */
/*                                                                      */
/*  This implementation is an ADIO for UPC. For more information about  */
/*  the ADIO definition or for licence information, please refer to     */
/*  the README file.                                                    */
/*                                                                      */
/*              HPCL, The George Wasnington University                  */
/*                        Author: Hugh Greenberg                        */
/*                       E-mail: hng@lanl.gov                           */
/*                                                                      */
/************************************************************************/

// adio_commit.c
// ADIO using PLFS
#include <fcntl.h>
#include "adio.h"

//Returns the number of references to the Plfs_fd remaining after the commit
int UPC_ADIO_Commit ( Plfs_fd *fd,
		     int *error_code )
{
	int err;

	err = plfs_commit( fd );
	if (err) {
	  *error_code = UPC_ADIO_SUCCESS;
	} else {
	  *error_code = UPC_ADIO_FAILURE;
	}

	return err;
}
