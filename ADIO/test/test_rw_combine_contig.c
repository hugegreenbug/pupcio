#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

#define MAX_BUFFER 255
static char fn[] = "/mnt/plfs/io_combine.foo";

static char buf[MAX_BUFFER];
static int const dim = 20;

int test_read_contig( Plfs_fd *fd )
{
	int err, ret, i;
	uint64_t offset = MYTHREAD*dim;
	int len = dim;
	ret = UPC_ADIO_ReadContig( fd, buf, len, offset, &err );
	assert( err == UPC_ADIO_SUCCESS );

	for( i = 0; i< len; i++ )
		assert( buf[i] == MYTHREAD+'0' );

	return 0;
}

int test_write_contig( Plfs_fd *fd )
{
	int err, ret, i;
	uint64_t offset = MYTHREAD*dim;
	int len = dim;
	memset( buf, MYTHREAD+'0', len);
	ret = UPC_ADIO_WriteContig( fd, buf, len, offset, &err );
	assert( err == UPC_ADIO_SUCCESS );

	UPC_ADIO_Flush ( fd, &err );
	assert( err == UPC_ADIO_SUCCESS );

	assert ( ret == len );

	return 0;
}

int main()
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
        test_write_contig( fd );
        upc_barrier;
	test_read_contig( fd );
	UPC_ADIO_Close( fd, open_flags, &err );
        pass();
}
