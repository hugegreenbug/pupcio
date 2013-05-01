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
	shared [] char *buffer;
	int size, i;
	struct upc_io_shared_memvec memvec[2];
	struct upc_io_filevec filevec[2];
        upc_off_t ret_size;
	int err;
        int flag;
        void *dummy;
	upc_flag_t sync_mode = 0;
	char fname[] = "/mnt/plfs/upcio.test";
	
	
	if(!MYTHREAD)
		printf("upcio test: test fread_list_shared_async with %d Threads\n", THREADS);

	size = 10;
	buffer = (shared [] char *)upc_alloc(sizeof(char)*size);
	
	memvec[0].baseaddr = (shared void *)&buffer[0];
	memvec[0].blocksize = 0;
	memvec[0].len = 4;
	memvec[1].baseaddr = (shared void *)&buffer[4];
	memvec[1].blocksize = 0;
	memvec[1].len = 3;
	filevec[0].offset = 4*MYTHREAD;
	filevec[0].len = 3;
        filevec[1].offset = 4+4*MYTHREAD;
        filevec[1].len = 4;

	for(i=0; i<size; i++)
		buffer[i] = 'z';

	fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_RDONLY, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	upc_all_fread_list_shared_async(fd, 2, (struct upc_io_shared_memvec const *)&memvec[0], 
					2, (struct upc_io_filevec const *)&filevec[0], &ret_size, 
					sync_mode);
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

	if (!MYTHREAD) { 
	  
	  printf("TH%2d: read ",MYTHREAD);
	  for (i = 0; i < size; i++) {
	    printf("%c", buffer[i]);
	  }
	}

	printf("\n");
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
		printf("upcio test: Done with fread_list_shared_async testing\n");

	upc_free(buffer);

	return 0;
}
	
