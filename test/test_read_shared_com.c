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
	upc_off_t ret_size, i;
	uint32_t nmemb, blocksize, size;
	upc_flag_t sync;
	shared [] char *buffer;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fread_shared with %d Threads\n", THREADS);

	nmemb = 1;
	blocksize = 0;
	size = 5;
	sync = 0;
	buffer = (shared [] char *)upc_all_alloc(1,sizeof(char)*size*nmemb);
	fd=upc_all_fopen( fname, UPC_COMMON_FP|UPC_RDONLY, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	ret_size = upc_all_fread_shared(fd, (shared void *)buffer, blocksize, size, nmemb, sync);
	if( ret_size == -1 )
		printf("upcio test: fread_shared error on TH%2d\n",MYTHREAD);
	else
	{
		if(!MYTHREAD) {
			printf("upcio test - read: ");
			for (i = 0; i < ret_size; i++) {
				printf("%c", buffer[i]);
			}	    
		}
	}
	
	upc_barrier;
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
	{
		printf("upcio test: Done with fread_shared testing\n");
		upc_free(buffer);
	}

	return 0;
}
	
