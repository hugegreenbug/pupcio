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

int UPC_ADIO_TransOpen ( Plfs_fd **fd, const char *filename,
			  int access_mode,
			  mode_t mode, int tid,
			  int *error_code )
{
	int err;
	Plfs_open_opt *opt;

	opt = (Plfs_open_opt *) malloc(sizeof(Plfs_open_opt));
	opt->tid = tid;
	opt->index_stream = NULL;
	opt->buffer_index = 0;
	opt->reopen = 0;
	opt->pinter = PLFS_API;
	err = plfs_open(fd, filename, access_mode, (uint32_t) MYTHREAD, mode, opt);
	free(opt);
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
