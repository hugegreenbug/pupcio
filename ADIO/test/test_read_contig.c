#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

#define MAX_BUFFER 255
static char fn[] = "/mnt/plfs/foo";

static char buf[MAX_BUFFER];

int test_read_contig( Plfs_fd *fd )
{
	int err, ret, i;
	uint64_t offset = MYTHREAD*20;
	int len = 20;
	ret = UPC_ADIO_ReadContig( fd, buf, len, offset, &err );
	assert ( ret == len );

	for( i = 0; i< len; i++ )
	{   
		assert( buf[i] == ( MYTHREAD +'0' ) );
	}

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
	upc_barrier;
	test_read_contig(fd);
	upc_barrier;
	UPC_ADIO_Close( fd, open_flags, &err );
        pass();
}
