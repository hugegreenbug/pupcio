/*********************************************
  UPC-IO Reference Implementation         
  Internal test cases

  HPCL, The George Wasnington University
  Author: Yiyi Yao
  E-mail: yyy@gwu.edu
*********************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <upc.h>
#include "upc_io.h"

#define BLOCKSIZE 3

int main()
{
	 upcio_file_t *fd;
	shared [BLOCKSIZE] char *buffer;
	uint32_t size, i;
	struct upc_io_shared_memvec memvec[2];
	struct upc_io_filevec filevec[2];
	upc_flag_t sync;
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fread_list_shared with %d Threads\n", THREADS);

	size = 10;
	buffer = (shared [BLOCKSIZE] char *)upc_global_alloc(ceil((float) sizeof(char)*size/BLOCKSIZE), BLOCKSIZE);

	memvec[0].baseaddr = (shared void *)&buffer[0];
	memvec[0].blocksize = BLOCKSIZE;
	memvec[0].len = 4;
	memvec[1].baseaddr = (shared void *)&buffer[6];
	memvec[1].blocksize = BLOCKSIZE;
	memvec[1].len = 3;
	filevec[0].offset = 4*MYTHREAD;
	filevec[0].len = 3;
        filevec[1].offset = 8+4*MYTHREAD;
        filevec[1].len = 4;
	sync = 0;

	for(i=0; i<size; i++)
		buffer[i] = '0';

	fd=upc_all_fopen( fname, UPC_COMMON_FP|UPC_RDONLY, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	size = upc_all_fread_list_shared(fd, 2, (struct upc_io_shared_memvec const *)&memvec[0], 2, 
					 (struct upc_io_filevec const *)&filevec[0], sync);
	if( size == -1 )
                printf("TH%2d: read_list_shared error \n",MYTHREAD);

	upc_barrier;
	for(i=0; i<10; i++)
		printf("TH%2d: read %c\n",MYTHREAD, buffer[i]);

        upc_barrier;
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}
	
	if(!MYTHREAD)
		printf("upcio test: Done with fread_list_shared_block testing\n");

	upc_free(buffer);
	return 0;
}
	
