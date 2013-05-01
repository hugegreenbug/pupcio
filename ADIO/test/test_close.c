#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

static char fn[] = "/mnt/plfs/foo";

int test_close( Plfs_fd *fd, int open_flags )
{
	int err;
	UPC_ADIO_Close( fd, open_flags, &err );
	assert( err != -1 );
    	return 0;
}


int main()
{
    int err;
    Plfs_fd *fd = NULL;
    int open_flags = O_RDONLY | O_CREAT;

    /*
      Have thread 0 open the file first to ensure that it is created 
    */
    if (!MYTHREAD) {
      UPC_ADIO_Open( &fd, fn, open_flags, 
		     0666, &err );	
    }

    upc_barrier;
    err = 0;
    
    /*
      Let the other threads open the file
    */
    if (MYTHREAD) {
      UPC_ADIO_Open( &fd, fn, open_flags, 
		     0666, &err );
    }
    
    upc_barrier;	
    assert (fd != NULL);
    test_close(fd, open_flags);
    pass();
}
