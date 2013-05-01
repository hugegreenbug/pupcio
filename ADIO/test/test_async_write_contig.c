#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

#define MAX_BUFFER 255
static char fn[] = "/mnt/plfs/foo2";

static char buf[MAX_BUFFER];

int test_async_write_contig( Plfs_fd *fd )
{
    int err, ret, i;
    uint64_t offset = MYTHREAD*10;
    uint32_t len = 10;
    UPC_ADIO_Request request;
    uint64_t size;

    memset( buf, MYTHREAD+'0', len);
    UPC_ADIO_IwriteContig( fd, buf, len, offset, &request, &size, &err );
    UPC_ADIO_Done(&request, &err);
    UPC_ADIO_Flush ( fd, &err );
    assert ( size == len );

    return 0;
}

int verify_write_contig( Plfs_fd *fd )
{
    int err, ret, i;
    uint64_t offset = MYTHREAD*10;
    uint32_t len = 10;
    memset( buf, 0, len );
    ret = UPC_ADIO_ReadContig( fd, buf, len, offset, &err );
    assert ( ret == len );
    for( i = 0; i< len; i++ )
        assert( buf[i] == MYTHREAD+'0' );

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

	test_async_write_contig( fd );
	upc_barrier;
        verify_write_contig( fd );
	UPC_ADIO_Close( fd, open_flags, &err );
        pass();
}
