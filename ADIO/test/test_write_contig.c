#include <stdio.h>
#include <assert.h>
#include <stdio.h>
#include "adio.h"
#include "utils.h"

#define MAX_BUFFER 255

static char fn[] = "/mnt/plfs/foo";
static char buf[MAX_BUFFER];
static int const dim = 20;

int test_write_contig( Plfs_fd *fd )
{
	int err, ret, i;
	uint64_t offset = MYTHREAD*dim;
	uint32_t len = dim;

	memset( buf, MYTHREAD+'0', len);
	ret = UPC_ADIO_WriteContig( fd, buf, len, offset, &err );
	UPC_ADIO_Flush ( fd, &err );

	return ret;
}

int verify_write_contig( Plfs_fd *fd )
{
	int err, ret, i;
	uint64_t offset = MYTHREAD*dim;
	int len = dim;

	upc_barrier;
	memset( buf, 0, len );
	ret = UPC_ADIO_ReadContig( fd, buf, len, offset, &err );
	assert ( ret == len );
	printf("Thread: %d read: ", MYTHREAD);
	for( i = 0; i< len; i++ ) {
		printf("%c", buf[i]);
		assert( buf[i] == MYTHREAD+'0' );
	}

	printf("\n");

	return 0;
}


int main()
{
	int err = 0;
	Plfs_fd *fd = NULL;
	int open_flags = O_RDWR | O_CREAT | O_TRUNC;

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

	/* 
	   Write to the file 
	*/
        test_write_contig( fd );

	upc_barrier;

	/*
	  Verify that the correct data was written
	*/
	verify_write_contig( fd );

	/*
	  Close the file
	*/
	UPC_ADIO_Close( fd, open_flags, &err );

        pass();
}
