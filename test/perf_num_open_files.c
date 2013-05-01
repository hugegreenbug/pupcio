/*********************************************
  UPC-IO Reference Implementation         
  Synthetic perf cases
  This test is to stress test the number of 
  opened file this I/O system can handle at
  a single time

  HPCL, The George Wasnington University
  Author: Yiyi Yao
  E-mail: yyy@gwu.edu
*********************************************/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <upc.h>
#include "upc_io.h"
#define NUM 100

int main()
{
	 upcio_file_t *fd[NUM];
	uint32_t i, j;
	
	char fname[] = "/mnt/plfs/upcio.test";

	if(!MYTHREAD)
		printf("upcio perf: test # of opened files with %d Threads\n", THREADS);

	for(i=0; i<NUM; i++)
	{
		upc_barrier;
		fd[i] = upc_all_fopen( fname, UPC_INDIVIDUAL_FP|UPC_WRONLY|UPC_CREATE, 0666);
		upc_barrier;
		if(fd[i]==NULL)
		{
			printf("TH%2d: Fail to open %d/%d file\n",MYTHREAD, i, NUM);
			printf("TH%2d: Closing up all the opened file and exit\n", MYTHREAD);
			for(j=0; j<i; j++)
			{
				if(upc_all_fclose(fd[j])!=0)
				{
                			printf("TH%2d: Double errors!! Fail to close %d/%d file\n",MYTHREAD, j, i);
                			upc_global_exit(-1);
				}
			}
			upc_global_exit(-1);
		}
	}

	for(j=0; j<NUM; j++)
	{
		upc_barrier;
		if(upc_all_fclose(fd[j])!=0)
		{
                	printf("TH%2d: Fail to close %d/%d file\n",MYTHREAD, j, NUM);
                	upc_global_exit(-1);
		}
	}

	if(!MYTHREAD)
		printf("upcio perf: Done with testing # of opened files\n");

	return 0;
}
	
