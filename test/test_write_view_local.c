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
	upc_flag_t sync = 0;
	void *buffer;
	uint32_t i, count, block_size;
	char fname[] = "/mnt/plfs/upcio.test";
	upc_file_view *lfile_view;

	if(!MYTHREAD) {
		printf("upcio test: test fwrite_view_local with %d Threads\n", THREADS);		
	}

	count = 1;
	block_size = 1;	
	if (MYTHREAD % 2 == 0) {
	  buffer = (int *)malloc(sizeof(int) * count * block_size);       
	  ((int *) buffer)[0] = 2;
	} else {
	  buffer = (float *)malloc(sizeof(float) * count * block_size);       
	  ((float *) buffer)[0] = 5.0f;
	}

	lfile_view = malloc(sizeof(upc_file_view));
	lfile_view->count = THREADS;
	lfile_view->elem_type = UPC_INT;
	lfile_view->block_lengths = malloc(THREADS * sizeof(uint32_t));
	lfile_view->block_displacements = malloc(THREADS * sizeof(uint32_t));
	lfile_view->types = malloc(THREADS * sizeof(uint32_t));
	for (i = 0; i < THREADS; i++) {
	  lfile_view->block_lengths[i] = block_size;
	}
	
	lfile_view->block_displacements[0] = 0;
	lfile_view->block_displacements[1] = 1;
	lfile_view->block_displacements[2] = 3;
	lfile_view->block_displacements[3] = 4;       
	for (i = 0; i < THREADS; i++) {
	  if (i % 2 == 0) {
	    lfile_view->types[i] = UPC_INT;
	  } else {
	    lfile_view->types[i] = UPC_FLOAT;
	  }
	}
	
	upc_barrier;
	fd=upc_all_fopen_view( fname, UPC_INDIVIDUAL_FP|UPC_WRONLY|UPC_CREATE, 0666, &lfile_view, 0);
	if(fd==NULL)
	{
		printf("TH%2d: File open Error\n",MYTHREAD);
		upc_global_exit(-1);
	}

	upc_barrier;
	ret_size = upc_all_fwrite_view_local(fd, (void *)buffer, count, sync, lfile_view);
	upc_barrier;

	if( ret_size == -1 )
		printf("upcio test: fwrite_local error on TH%2d\n",MYTHREAD);
	
	upc_barrier;
	upc_all_fsync(fd);
	if(upc_all_fclose_view(fd, lfile_view)!=0)
	{
                printf("TH%2d: File close Error\n",MYTHREAD);
                upc_global_exit(-1);
	}

	free((void *)buffer);
	return 0;
}
	
