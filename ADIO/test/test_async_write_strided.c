#include <stdio.h>
#include <assert.h>
#include "adio.h"
#include "utils.h"

#define MAX_BUFFER 32
static char fn[] = "/mnt/plfs/foo";

static char mem_buffers[4][MAX_BUFFER];
static uint64_t mem_lengths[4];
static uint64_t file_offsets[4];
static uint64_t file_lengths[4];

static char vbuf[MAX_BUFFER];

int test_async_write_strided( Plfs_fd *fd )
{
    int error_code, ret, i, j, k;
    uint32_t mem_list_count;
    uint32_t file_list_count;
    char* mem_ptrs[4];
    UPC_ADIO_Request request;
    uint64_t size;

    if ( !MYTHREAD )
    {
        /* Here is a simplified version for overlapped memory */
        mem_list_count = 2;
        file_list_count = 3;
        /* 30 byte spanned in 2 memory block */
        mem_lengths[0] = 13;
        mem_lengths[1] = 30 - 13;
    }
    else
    {
        /* overlapped file block */
        mem_list_count = 3;
        file_list_count = 2;

        /* 20 bytes spanned in 3 mem block */
        mem_lengths[0] = 3;
        mem_lengths[1] = 10; 
        mem_lengths[2] = 7; 
    }


    for( i = 0; i< file_list_count; i++ )
    {
        file_offsets[i] = i*30 + MYTHREAD*100;
        file_lengths[i] = 10;
    }

    for( i = 0; i< mem_list_count; i++ )
    {
        mem_ptrs[i] = mem_buffers[i];
        memset( mem_ptrs[i], MYTHREAD+'0', mem_lengths[i] );
    }

    UPC_ADIO_IwriteStrided( fd, mem_list_count, mem_ptrs, mem_lengths, 
			    file_list_count, file_offsets, file_lengths, 
			    &request, &size, &error_code );
    assert( error_code != -1 );

    UPC_ADIO_Done(&request, &error_code);
    UPC_ADIO_Flush ( fd, &error_code );

    return 0;
}

int verify_write_strided( Plfs_fd *fd )
{
    int err, i, j;    
    uint32_t const len = 10;
    uint64_t const offset = 30;
    int count = MYTHREAD ? 2 : 3;

    for( i = 0; i< count; i++ )
    {
        UPC_ADIO_ReadContig( fd, vbuf, len, offset*i+MYTHREAD*100, &err );
        assert( err != -1 );
	printf("Thread: %d read: ", MYTHREAD);
        for( j = 0; j< len; j++ ) {
	  assert( vbuf[j] == '0'+MYTHREAD );
	  printf("%c", vbuf[j]);
	}
	printf("\n");
    }
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

	test_async_write_strided( fd );
        verify_write_strided( fd );

	UPC_ADIO_Close( fd, open_flags, &err );
        pass();
}
