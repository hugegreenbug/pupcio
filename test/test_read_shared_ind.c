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
	shared [] char *buffer;
	int i;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: open and close with %d Threads\n", THREADS);

	nmemb = 1;
	blocksize = 0;
	size = 10*(MYTHREAD+1);
	buffer = (shared [] char *)upc_alloc(sizeof(char)*size*nmemb);
	fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_RDONLY, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	size = upc_all_fread_shared(fd, (shared void *)buffer, blocksize, size, nmemb, sync);
	if( size == -1 )
		printf("upcio test: fread_shared error on TH%2d\n",MYTHREAD);
	else {
	  if( !MYTHREAD ) {
	    printf("upcio test - read: ");
	    for (i = 0; i < size; i++) {
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
		printf("upcio test: Done with fread_shared testing\n");

	upc_free(buffer);

	return 0;
}
	
