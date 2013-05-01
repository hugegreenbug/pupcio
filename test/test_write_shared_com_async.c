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
	upc_off_t ret_size;
	uint32_t size, nmemb, blocksize;
	shared [] char *buffer;
	int i, err;
	int flag;
	void *dummy;
	upc_flag_t sync_mode = 0;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fwrite_shared_com_async with %d Threads\n", THREADS);

	nmemb = 1;
	blocksize = 0;
	size = 30;
	buffer = (shared [] char *)upc_all_alloc(1,sizeof(char)*size*nmemb);
	if( !MYTHREAD )
	{
		for(i=0; i<size; i++)
			buffer[i] = '5';
	}

	fd=upc_all_fopen( fname, UPC_COMMON_FP|UPC_WRONLY, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_all_fwrite_shared_async(fd, (shared void *)buffer, blocksize, size, 
				    nmemb, &ret_size, sync_mode);
	dummy = NULL;
        if( upc_all_fcntl(fd, UPC_ASYNC_OUTSTANDING, dummy) )
                printf("TH%2d has an outstanding ASYNC OP\n",MYTHREAD);
        else
                printf("TH%2d does not has outstanding ASYNC OPs\n",MYTHREAD);
        upc_barrier;

	err = upc_all_ftest_async(fd, &flag);
	if( err == -1 )
                printf("TH%2d: ftest Error\n",MYTHREAD);
	else
	{
		if( flag )
		{
			printf("TH%2d: Async op done\n",MYTHREAD);
			printf("TH%2d: Async return %d\n",MYTHREAD, err);
		}
		else
			printf("TH%2d: Async pending\n",MYTHREAD);
	}
	
	err = upc_all_fwait_async(fd);
        if( err == -1 )
                printf("TH%2d: fwait Error\n",MYTHREAD);
	else
		printf("TH%2d: fwait returns %d\n",MYTHREAD, err);

	upc_barrier;

        if( upc_all_fcntl(fd, UPC_ASYNC_OUTSTANDING, dummy) )
                printf("TH%2d has an outstanding ASYNC OP\n",MYTHREAD);
        else
                printf("TH%2d does not has outstanding ASYNC OPs\n",MYTHREAD);

        upc_barrier;

	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
	{
		printf("upcio test: Done with fwrite_shared_com_async testing\n");
		upc_free(buffer);
	}

	return 0;
}
	
