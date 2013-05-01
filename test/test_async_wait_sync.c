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
	uint32_t size, nmemb;
	char *buffer;
	int i, err;
	int flag;
	void *dummy;
	upc_flag_t sync_mode = 0;	
	char fname[] = "/mnt/plfs/upcio.test";
	

	if(!MYTHREAD)
		printf("upcio test: test fread_local_async with %d Threads\n", THREADS);

	nmemb = 1;
	size = 10;
	buffer = (char *)malloc(sizeof(char)*size*nmemb);
	fd=upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_RDWR, 0666);
	upc_barrier;

	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	upc_all_fseek(fd, 10*MYTHREAD, UPC_SEEK_SET);
	/* Initialize the buffer, then write */
	for(i=0; i<size; i++)
		buffer[i]= MYTHREAD + 48;
	
	upc_all_fwrite_local_async(fd, (void *)buffer, size, nmemb, &ret_size, 
				   sync_mode);
	
	err = upc_all_fwait_async(fd);
	if( err == -1 )
		printf("TH%2d: fwait Error\n",MYTHREAD);
	else
		printf("TH%2d: fwait returns %d\n",MYTHREAD, err);
	
	/* read, and check what we read */	
	for(i=0; i<size; i++)
		buffer[i]= 0;
	
	upc_barrier;
	
	upc_all_fseek(fd, 10*MYTHREAD, UPC_SEEK_SET);
	upc_barrier;
	upc_all_fread_local(fd, (void *)buffer, size, nmemb, 0);
	
	for( i = 0; i < size; i++)
		if( buffer[i] != MYTHREAD + 48)
			printf("TH%2d: read fails %c.\n", MYTHREAD, buffer[i]);
	
		
	if(upc_all_fclose(fd)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	if(!MYTHREAD)
		printf("upcio test: Done with fread_local_async testing\n");
	
	
	free((void *)buffer);
	return 0;
}
	
