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

// adio_iwrite_contig.c
// ADIO using PLFS
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "adio.h"

struct thread_data
{
	Plfs_fd *fd;
	void *buf;
	uint32_t len;
	uint64_t offset;
	int64_t *ret;
};

struct thread_data arg;

void *IwriteContig ( void *thread_arg )
{
	uint64_t err = 0;
	struct thread_data *ptr;
  
	ptr = (struct thread_data *)thread_arg;
	err = plfs_write( ptr->fd, ptr->buf, ptr->len, ptr->offset, MYTHREAD);

	//Set the actual amount written
	*ptr->ret = err;
  
	return (void *) ptr;
}

void UPC_ADIO_IwriteContig ( Plfs_fd *fd,
                             void *buf,
                             uint32_t len,
                             uint64_t offset,
                             UPC_ADIO_Request* request,
			     int64_t *ret,
                             int *error_code )
{
	pthread_t io_thread;
	int iret;

	/* Initialize the argument structure */
	arg.fd = fd;
	arg.buf = buf;
	arg.len = len;
	arg.offset = offset;
	arg.ret = ret;

	/* Create the thread to perform the async IO */
	iret = pthread_create( &io_thread, NULL, IwriteContig, (void *)&arg );

	if( iret == 0 )
	{
		*request = io_thread;
		*error_code = UPC_ADIO_SUCCESS;
	}
	else
		*error_code = UPC_ADIO_FAILURE;

	return;
}
