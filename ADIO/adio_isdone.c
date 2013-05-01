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
// adio_isdone.c
// ADIO using POSIX/NFS
#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include "adio.h"

void UPC_ADIO_IsDone ( UPC_ADIO_Request* request,
                       int *flag,
                       int *error_code )
{
	int ret;

	/* send the NULL signal to the thread to test if it is still alive */
	ret = pthread_kill ( *request, 0 );

	/* thread still working */
	if( !ret )
	{
		*flag= 0;
		*error_code = UPC_ADIO_SUCCESS;
	}
	/* thread finished already */
	else if( ret == ESRCH )
	{
		*flag= 1;
		*error_code = UPC_ADIO_SUCCESS;
	}
	/* real errors */
	else
		*error_code = UPC_ADIO_FAILURE;
}
