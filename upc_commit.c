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
// upc_commit.c
#include <stdio.h>
#include <upc.h>
#include "upc_io.h"

int upc_all_commit(upcio_file_t *fh)
{
	int error_code;
	Plfs_fd *fd;
	struct __struct_thread_upc_file_t *fh_local;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if(fh == NULL)
		return -1;

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh_local = (struct __struct_thread_upc_file_t *)fh->th[MYTHREAD];
	fd = (Plfs_fd *)fh_local->adio_fd;
	
	if (MYTHREAD == 0) {
		UPC_ADIO_Commit(fd, &error_code);
		if (error_code != UPC_ADIO_SUCCESS) {
			return -1;
		}
	}
	
	return 0;
}

