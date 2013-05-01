/************************************************************************/
/*                       UPC-ADIO Implementation                        */
/*                                                                      */
/*  This implementation is an ADIO for UPC. For more information about  */
/*  the ADIO definition or for licence information, please refer to     */
/*  the README file.                                                    */
/*                                                                      */
/*              HPCL, The George Wasnington University                  */
/*                        Author: Yiyi Yao                              */
/*                       E-mail: yyy@gwu.edu                            */
/*                                                                      */
/*         Modified for PLFS by: Hugh Greenberg <hng@lanl.gov>          */
/************************************************************************/

// adio.h
// ADIO using PLFS
#ifndef _ADIO_H
#define _ADIO_H 1

#include <pthread.h>
#include <unistd.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <plfs.h>
#include <stdint.h>
#include "upc_map.h"

/* Define ADIO_Success/Fail */
#define UPC_ADIO_SUCCESS                        0
#define UPC_ADIO_FAILURE                        -1

/* NOT_DEFINED is set to 2^20 */
#define NOT_DEFINED                         1048576

#define UPC_ADIO_STRONG_CA		    128
#define UPC_ADIO_INDIVIDUAL_FP              256
#define UPC_ADIO_COMMON_FP                  512

/* definition of upc seek function constants */
#define UPC_ADIO_SET                        0
#define UPC_ADIO_CUR                        1
#define UPC_ADIO_END                        2

/* Async request type */
typedef pthread_t UPC_ADIO_Request;

/* UPC ADIO API definitions */
int UPC_ADIO_Open( Plfs_fd **fd, const char *filename, int access_mode, mode_t mode, 
		   int *error_code);
int UPC_ADIO_TransOpen ( Plfs_fd **fd, const char *filename, int access_mode,
			  mode_t mode, int tid, int *error_code );
int UPC_ADIO_ViewOpen( Plfs_fd **fd, const char *filename, int access_mode, mode_t mode, 
		       int *error_code, upc_file_view **file_view);
int UPC_ADIO_Close(Plfs_fd *fd, int open_flags, int *error_code);
int UPC_ADIO_Commit(Plfs_fd *fd, int *error_code);
int UPC_ADIO_CloseView(Plfs_fd *fd, int open_flags, int *error_code, upc_file_view *file);
void UPC_ADIO_Flush (Plfs_fd *fd, int *error_code);
void UPC_ADIO_GetSize(Plfs_fd *fd, const char *path, uint64_t *size, int *error_code);
void UPC_ADIO_Resize(Plfs_fd *fd, const char *path, uint64_t size, int *error_code);
int64_t UPC_ADIO_WriteContig (Plfs_fd *fd, void *buf,
			      uint32_t len, uint64_t offset, 
			      int *error_code );
int64_t UPC_ADIO_ReadContig (Plfs_fd *fd, void *buf,
			     uint32_t len, uint64_t offset, 
			     int *error_code);
int64_t UPC_ADIO_ReadViewContig (Plfs_fd *fd, 
				 void *buf, 
				 uint32_t count, 
				 int *error_code,
				 upc_file_view *file_view);
int64_t UPC_ADIO_WriteStrided (Plfs_fd *fd, uint32_t mem_list_count, void *mem_ptrs,
			       uint64_t *mem_lengths, uint32_t file_list_count,
			       uint64_t *file_offsets, uint64_t *file_lengths,
			       int *error_code); 
int64_t UPC_ADIO_ReadStrided (Plfs_fd *fd, uint32_t mem_list_count, void *mem_ptrs,
			      uint64_t *mem_lengths, uint32_t file_list_count, 
			      uint64_t *file_offsets, uint64_t *file_lengths, 
			      int *error_code );

/* Non-blocking I/O */
void UPC_ADIO_IreadContig (Plfs_fd *fd, void *buf,
			   uint32_t len, uint64_t offset, 
			   UPC_ADIO_Request* request, int64_t *ret, 
			   int *error_code );

void UPC_ADIO_IwriteContig (Plfs_fd *fd, void *buf,
			    uint32_t len, uint64_t offset, 
			    UPC_ADIO_Request* request, int64_t *ret,
			    int *error_code );
void UPC_ADIO_IwriteStrided ( Plfs_fd *fd, uint32_t mem_list_count,
                              void* mem_ptrs, uint64_t* mem_lengths, 
			      uint32_t file_list_count, uint64_t* file_offsets,
                              uint64_t* file_lengths, UPC_ADIO_Request *request, 
			      int64_t *ret, int *error_code );

void UPC_ADIO_IreadStrided ( Plfs_fd *fd, uint32_t mem_list_count, 
			     void* mem_ptrs, uint64_t* mem_lengths,
                             uint32_t file_list_count, uint64_t* file_offsets,
                             uint64_t* file_lengths, UPC_ADIO_Request *request,
			     int64_t *ret, int *error_code );

void UPC_ADIO_IsDone ( UPC_ADIO_Request* request, int *flag, int *error_code  );
void UPC_ADIO_Done ( UPC_ADIO_Request* request, int *error_code );
#endif /*_ADIO_H*/

