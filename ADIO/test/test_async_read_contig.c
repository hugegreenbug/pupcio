#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

#define MAX_BUFFER 255
static char fn[] = "/mnt/plfs/foo2";

static char buf[MAX_BUFFER];

int test_iread_contig( Plfs_fd *fd )
{
	int err, ret, i;
	uint64_t offset = MYTHREAD*10;
	uint32_t len = 10;
	UPC_ADIO_Request request;
	uint64_t size = 0;

	UPC_ADIO_IreadContig( fd, buf, len, offset, &request, &size, &err );
	UPC_ADIO_Done(&request, &err);
	printf("Thread: %d read size: %ld\n", MYTHREAD, size);
	/*
	  assert ( size == len );
	  
	  for( i = 0; i< len; i++ )
	  {   
	  assert( buf[i] == ((i+offset) % 10)+'0' );
	  }
	*/

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
	test_iread_contig( fd );

	upc_barrier;
	UPC_ADIO_Close( fd, open_flags, &err );

	pass();
}
