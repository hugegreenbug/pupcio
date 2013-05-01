#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

static char fn[] = "/mnt/plfs/foo";

int test_getsize( Plfs_fd *fd )
{
	int err;
	uint64_t size;
	
	UPC_ADIO_GetSize( fd, fn, &size, &err );
	assert( err != -1 );
	printf("Size of file: %s is: %lu\n", fn, size);

	return 0;
}


int main()
{
	int err;
	Plfs_fd *fd = NULL;
	int open_flags = O_RDONLY;

	/*
	  Have thread 0 open the file first to ensure that it is created 
	*/
	if (!MYTHREAD) {
		UPC_ADIO_Open( &fd, fn, open_flags, 0666, &err );	      
	}
	
	upc_barrier;
	err = 0;
	
	/*
	  Let the other threads open the file
	*/
	if (MYTHREAD) {
		UPC_ADIO_Open( &fd, fn, open_flags, 0666, &err );
	}
    
	assert (fd != NULL);
	test_getsize( fd );
	UPC_ADIO_Close( fd, open_flags, &err );
	pass();
}
