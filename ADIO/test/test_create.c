#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

static char fn[] = "/mnt/plfs/foo_not_exist";

int test_open( void )
{
	int err;
	Plfs_fd *fd = NULL;
	int open_flags = O_RDWR | O_CREAT | O_TRUNC;

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
	upc_barrier;
	UPC_ADIO_Close( fd, open_flags, &err );

	return 0;
}

int main()
{
	test_open();
	pass();
}
