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

// adio_iwrite_strided.c
// ADIO using PLFS
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "adio.h"

struct thread_data
{
	Plfs_fd   *fd;
	uint32_t  mem_list_count;
	void*     mem_ptrs;
	uint64_t* mem_lengths;
	uint32_t  file_list_count;
	uint64_t* file_offsets;
	uint64_t* file_lengths;
	int64_t* ret;
};

struct thread_data arg;

void *IwriteStrided ( void *thread_arg )
{
	int error_code;
	int err;
	struct thread_data * ptr;

	ptr = (struct thread_data *)thread_arg;
	err = UPC_ADIO_WriteStrided( ptr->fd,
	                             ptr->mem_list_count,
	                             ptr->mem_ptrs,
	                             ptr->mem_lengths,
	                             ptr->file_list_count,
	                             ptr->file_offsets,
	                             ptr->file_lengths,
	                             &error_code );

	//Set the ret pointer to contain -1 on error, or the size written on success
	*ptr->ret = err;

	return (void *) ptr;
}

void UPC_ADIO_IwriteStrided ( Plfs_fd *fd,
                              uint32_t mem_list_count,
                              void* mem_ptrs,
                              uint64_t* mem_lengths,
                              uint32_t  file_list_count,
                              uint64_t* file_offsets,
                              uint64_t* file_lengths,
                              UPC_ADIO_Request *request,
			      int64_t *ret,
                              int *error_code )

{
	pthread_t io_thread;
	int iret;

	/* Initialize the argument structure */
	arg.fd = fd;
	arg.mem_list_count = mem_list_count;
	arg.mem_ptrs = mem_ptrs;
	arg.mem_lengths = mem_lengths;
	arg.file_list_count = file_list_count;
	arg.file_offsets = file_offsets;
	arg.file_lengths = file_lengths;
	arg.ret = ret;

	/* Create the thread to perform the async IO */
	iret = pthread_create( &io_thread, NULL, IwriteStrided, (void *)&arg );

	if( iret == 0 )
	{
		*request = io_thread;
		*error_code = UPC_ADIO_SUCCESS;
	}
	else
		*error_code = UPC_ADIO_FAILURE;

	return;
}
