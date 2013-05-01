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
/************************************************************************/
// adio_done.c
// ADIO using POSIX/NFS
#include <sys/types.h>
#include <pthread.h>
#include "adio.h"

void UPC_ADIO_Done ( UPC_ADIO_Request* request,
                     int *error_code )
{
	int ret;

	/* block the main thread to wait for the async I/O */
	ret = pthread_join( *request, NULL );

	if( !ret )
		*error_code = UPC_ADIO_SUCCESS;
	else
		*error_code = UPC_ADIO_FAILURE;
}
