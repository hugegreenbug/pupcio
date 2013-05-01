/************************************************************************/
/*              UPC-IO Implementation Based on UPC-ADIO                 */
/*                                                                      */
/*  For more detail description of the UPC-IO, please refer to the UPC  */
/*  language specification. For licence information, please refer to    */
/*  the README file.                                                    */
/*                                                                      */
/*              HPCL, The George Wasnington University                  */
/*                        Author: Yiyi Yao                              */
/*                       E-mail: yyy@gwu.edu                            */
/************************************************************************/
// upc_fclose_view.c
#include <upc.h>
#include <stdlib.h>
#include "upc_io.h"

int upc_all_fclose_view( upcio_file_t *fh_shared, upc_file_view *file_view )
{
	int error_code;
	int refs;
	char * local_filename;
	struct __struct_thread_upc_file_t *fh;
	Plfs_fd *fd;

	/*------------------------------------------------------------------*/
	/* the file handler has to be valid                                 */
	/*------------------------------------------------------------------*/
	if(fh_shared == NULL)
	{
#ifdef DEBUG
		printf("close: invalid file handler\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* cast the local file handler into private ones                    */
	/* hopefully doing so will increase performance                     */
	/*------------------------------------------------------------------*/
	fh = (struct __struct_thread_upc_file_t *)fh_shared->th[MYTHREAD];
	fd = (Plfs_fd *)fh->adio_fd;
	local_filename = (char *)(fh->fname);

	/*------------------------------------------------------------------*/
	/* make sure there is no asynchrounouse ops pending                 */
	/*------------------------------------------------------------------*/
	if( fh->async_flag == 1 )
	{
#ifdef DEBUG
		printf("close: pending async ops\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* call upc_all_fsync before closing the file handler               */
	/*------------------------------------------------------------------*/
	error_code = upc_all_fsync( fh_shared );
	if(error_code == -1)
	{
#ifdef DEBUG
		printf("close: fsync failure\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* call ADIO_Close to close the file handler                        */
	/*------------------------------------------------------------------*/
	if (fh->free_view_in_plfs) {
		refs = UPC_ADIO_CloseView( fd, fh->flags, &error_code, file_view);
	} else {
		refs = UPC_ADIO_Close( fd, fh->flags, &error_code);	       
		free(file_view->block_displacements);
		free(file_view->block_lengths);
		free(file_view->types);
		free(file_view);
	}

	if(error_code != UPC_ADIO_SUCCESS)
	{
#ifdef DEBUG
		printf("close: close failure\n");
#endif
		return -1;
	}

	/*------------------------------------------------------------------*/
	/* free up the memory                                               */
	/*------------------------------------------------------------------*/
	free(local_filename);
	upc_free(fh_shared->th[MYTHREAD]);
	upc_barrier;

	/*------------------------------------------------------------------*/
	/*Free the shared structure if MYTHREAD == 0                        */
	/*------------------------------------------------------------------*/
	if (!MYTHREAD) {
		upc_free(fh_shared);
	}
	
	return 0;
}
