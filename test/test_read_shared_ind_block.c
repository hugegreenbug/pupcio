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
#define SIZE 24

int main()
{
	 upcio_file_t *fd;
	upc_off_t ret_size;
	uint32_t size, nmemb, blocksize;
	upc_flag_t sync = 0;
	shared [5] char *buffer;
	int i;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fread_shared with %d Threads " 
		       "(INDIVIDUAL_FP with blocksize NOT equal to 0)\n", THREADS);

	nmemb = SIZE;
	blocksize = 5;
	size = 1;
	buffer = (shared [5] char *)upc_global_alloc(ceil((float) 2*size*nmemb/5),5*sizeof(char));
	for(i=0;i<2*SIZE;i++)
		buffer[i] = '0';

	fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_RDONLY, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	ret_size = upc_all_fread_shared(fd, (shared void *)&buffer[5], blocksize, size, nmemb, sync);
	upc_barrier;
	if( ret_size == -1 )
		printf("upcio test: fread_shared error on TH%2d\n",MYTHREAD);
	else
	{
		for(i=0; i<2*SIZE; i++)
			printf("TH%2d: buffer[%d]: %c\n",MYTHREAD, i, buffer[i]);
	}
	
	upc_barrier;
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}
	upc_barrier;

	if(!MYTHREAD)
	{
		printf("upcio test: Done with fread_shared testing\n");
		upc_free(buffer);
	}

	upc_barrier;
	return 0;
}
	
