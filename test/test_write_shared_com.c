/*********************************************
  UPC-IO Reference Implementation         
  Internal test cases

  HPCL, The George Wasnington University
  Author: Yiyi Yao
  E-mail: yyy@gwu.edu
*********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <upc.h>
#include "upc_io.h"

int main()
{
	 upcio_file_t *fd;
	uint64_t ret_size;
	uint32_t size, nmemb, blocksize;
	upc_flag_t sync = 0;
	int i;
	void *dummy;
	shared [] char *buffer;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fwrite_shared with %d Threads\n", THREADS);

	nmemb = 1;
	blocksize = 0;
	size = 10;
	buffer = (shared [] char *)upc_local_alloc(1,sizeof(char)*size*nmemb);
	fd=upc_all_fopen( fname, UPC_COMMON_FP|UPC_WRONLY|UPC_CREATE, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}
        upc_barrier;

	if( MYTHREAD == 0 )
	{
		printf("TH%2d: size is %d\n",MYTHREAD, size);
        	for(i=0; i<size; i++)
                        buffer[i] = '8';
        }
        upc_barrier;

	dummy = NULL;
	if( upc_all_fcntl(fd, UPC_ASYNC_OUTSTANDING, dummy) )
		printf("TH%2d has an outstanding ASYNC OP\n",MYTHREAD);
	else
		printf("TH%2d does not has outstanding ASYNC OPs\n",MYTHREAD);
	upc_barrier;

	size = upc_all_fwrite_shared(fd, (shared void *)buffer, blocksize, size, nmemb, sync);
	printf("TH%2d: return size is %d\n", MYTHREAD, size);
	if( size == -1 )
		printf("upcio test: fwrite_shared error on TH%2d\n",MYTHREAD);
	
	upc_barrier;
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
		printf("upcio test: Done with fwrite_shared testing\n");

	upc_free(buffer);
	return 0;
}
	
