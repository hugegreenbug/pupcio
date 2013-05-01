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
// upc_io.h

#ifndef _UPC_IO_H
#define _UPC_IO_H 1

#include "adio.h"
#include "upc_map.h"
//#warning "UPC-IO implementation based on UPC ADIO"

/* upc_off_t */
#undef upc_off_t

typedef uint64_t upc_off_t;

typedef int upcio_flag_t;

/* definition of upc_fcntl function constants */
#undef UPC_GET_CA_SEMANTICS
#undef UPC_SET_WEAK_CA_SEMANTICS
#undef UPC_SET_STRONG_CA_SEMANTICS
#undef UPC_GET_FP
#undef UPC_SET_COMMON_FP
#undef UPC_SET_INDIVIDUAL_FP
#undef UPC_GET_FL
#undef UPC_GET_FN
#undef UPC_GET_HINTS
#undef UPC_SET_HINT
#undef UPC_ASYNC_OUTSTANDING

#define UPC_GET_CA_SEMANTICS          1
#define UPC_SET_WEAK_CA_SEMANTICS     2
#define UPC_SET_STRONG_CA_SEMANTICS   3
#define UPC_GET_FP                    4
#define UPC_SET_COMMON_FP             5
#define UPC_SET_INDIVIDUAL_FP         6
#define UPC_GET_FL                    7
#define UPC_GET_FN                    8
#define UPC_GET_HINTS                 9
#define UPC_SET_HINT                  10
#define UPC_ASYNC_OUTSTANDING         11

/* definition of types of the async operations */
#define __REF_UPC_NONE_ASYNC                    0
#define __REF_UPC_READ_LOCAL_ASYNC              1
#define __REF_UPC_READ_SHARED_ASYNC             2
#define __REF_UPC_WRITE_LOCAL_ASYNC             3
#define __REF_UPC_WRITE_SHARED_ASYNC            4
#define __REF_UPC_READ_LIST_LOCAL_ASYNC         5
#define __REF_UPC_READ_LIST_SHARED_ASYNC        6
#define __REF_UPC_WRITE_LIST_LOCAL_ASYNC        7
#define __REF_UPC_WRITE_LIST_SHARED_ASYNC       8
#define __REF_UPC_WAIT_ASYNC                    9
#define __REF_UPC_TEST_ASYNC                    10

/* definition of flags with UPC-IO access mode */
#undef UPC_CREATE
#undef UPC_RDONLY
#undef UPC_WRONLY
#undef UPC_RDWR
#undef UPC_DELETE_ON_CLOSE
#undef UPC_EXCL
#undef UPC_APPEND
#undef UPC_INDIVIDUAL_FP
#undef UPC_COMMON_FP
#undef UPC_STRONG_CA
#undef UPC_TRUNC

#define UPC_CREATE              O_CREAT
#define UPC_RDONLY              O_RDONLY
#define UPC_WRONLY              O_WRONLY
#define UPC_RDWR                O_RDWR
#define UPC_EXCL                O_EXCL
#define UPC_APPEND              O_APPEND
#define UPC_TRUNC               O_TRUNC
#define UPC_INDIVIDUAL_FP       UPC_ADIO_INDIVIDUAL_FP
#define UPC_COMMON_FP           UPC_ADIO_COMMON_FP
#define UPC_STRONG_CA           UPC_ADIO_STRONG_CA

/* definition of upc seek function constants */
#undef UPC_SEEK_SET
#undef UPC_SEEK_CUR
#undef UPC_SEEK_END

#define UPC_SEEK_SET            UPC_ADIO_SET
#define UPC_SEEK_CUR            UPC_ADIO_CUR
#define UPC_SEEK_END            UPC_ADIO_END

/* struct upc_io_local_memvec */
struct upc_io_local_memvec
{
        void *baseaddr;
        upc_off_t len;
};

/* struct upc_io_shared_memvec */
struct upc_io_shared_memvec
{
        shared void *baseaddr;
        uint32_t blocksize;
        upc_off_t len;
};

/* upc_io_filevec_t */
struct upc_io_filevec
{
        upc_off_t offset;
        upc_off_t len;
};

/* upc_io_hint */
struct upc_io_hint
{
        const char *key;
        const char *value;
};


typedef shared struct __struct_upc_file_t upcio_file_t;
struct __struct_thread_upc_file_t
{
	Plfs_fd *adio_fd;                          		// File handler of ADIO
	int flags;						// access mode
	char *fname;						// filename
	upc_off_t private_pointer;				// private file pointer
	upc_off_t shared_pointer;				// shared file pointer
	UPC_ADIO_Request request;				// Async request
	int async_op;						// Async operation types
	int async_flag;						// Async operation pending flag
	unsigned char *local_ptr;				// local pointer used in async operations
	uint32_t blocksize;					// store the blocksize of the async calls
	shared void *sh_ptr;					// shared pointer used in async operations
	upc_off_t size;						// total size of transfered data
	uint32_t num_memvec;					// number of memory sections
	struct upc_io_shared_memvec const * shared_memvec;	// shared memvec struct
	upc_off_t *disparray;				        // disparray to be deleted
	upc_off_t *dispsize;				        // dispsize to be deleted
	void **mem_ptrs;					// mem_ptrs to be deleted
        upc_off_t * mem_lengths;			        // mem_lengths to be deleted
	upc_off_t * file_offsets;				// file_offsets to be deleted
	upc_off_t * file_lengths;			      	// file_lengths to be deleted
	int debug_size;
	upc_file_view *file_view;                               // Semantic information describing the data
	int free_view_in_plfs;                                  // Should the view memory be freed in plfs or upc?
};

/* Stores the file view */
typedef shared struct {
  uint32_t count;
  shared uint32_t *block_lengths;
  shared uint32_t *block_displacements;
  shared uint32_t *types;
  uint32_t elem_type;
} upc_file_view_shared;

struct __struct_upc_file_t
{
	shared struct __struct_thread_upc_file_t *th[THREADS];
};

/* UPC IO functions */
extern upcio_file_t * upc_all_fopen( const char *filename,
				   int flags, mode_t mode);
extern upcio_file_t * upc_all_fopen_view( const char *filename,
					  int flags, mode_t mode, 
					  upc_file_view **file_view,
					  int populate);
extern int upc_all_fclose( upcio_file_t *fd );
extern int upc_all_fclose_view( upcio_file_t *fd, upc_file_view *file_view );
extern int upc_all_commit(upcio_file_t *fh);
extern int upc_all_fsync(upcio_file_t *fd);
extern int64_t upc_all_fread_local(upcio_file_t *fd, void *buffer, upc_off_t size, uint32_t nmemb, 
				   upcio_flag_t sync_mode);
extern upcio_file_t * upc_all_fopen_trans( const char *filename,
					   int flags, mode_t mode, int tid );
extern int64_t upc_all_fread_view_local(upcio_file_t *fh_shared, void *buffer,
					uint32_t count, upc_flag_t sync_mode,
					upc_file_view *file_view);
extern int64_t upc_all_fread_shared(upcio_file_t *fd, shared void *buffer, uint32_t blocksize, upc_off_t size, 
				    uint32_t nmemb, upcio_flag_t sync_mode);
extern int64_t upc_all_fwrite_local(upcio_file_t *fd, void *buffer, upc_off_t size, uint32_t nmemb, 
				    upcio_flag_t sync_mode);
extern int64_t upc_all_fwrite_view_local(upcio_file_t *fd, void *buffer, uint32_t count,  
					 upcio_flag_t sync_mode, upc_file_view *file_view);
extern int64_t upc_all_fwrite_shared(upcio_file_t *fd, shared void *buffer, uint32_t blocksize, 
				     upc_off_t size, uint32_t nmemb, upcio_flag_t sync_mode);
extern int64_t upc_all_fseek(upcio_file_t *fd, upc_off_t offset, int origin);
extern int upc_all_fset_size(upcio_file_t *fd, upc_off_t size);
extern int64_t upc_all_fget_size(upcio_file_t *fd);
extern int upc_all_fpreallocate(upcio_file_t *fd, upc_off_t size);
extern int upc_all_fcntl(upcio_file_t *fd, int cmd, void *arg);
extern int64_t upc_all_fread_list_local(upcio_file_t *fd, uint32_t memvec_entries, 
					  struct upc_io_local_memvec const *memvec, 
					  uint32_t filevec_entries, 
					  struct upc_io_filevec const *filevec, 
					  upcio_flag_t sync_mode);
extern int64_t upc_all_fread_list_shared(upcio_file_t *fd, uint32_t memvec_entries, 
					   struct upc_io_shared_memvec const *memvec, 
					   uint32_t filevec_entries, 
					   struct upc_io_filevec const *filevec, 
					   upcio_flag_t sync_mode);
extern int64_t upc_all_fwrite_list_local(upcio_file_t *fd, uint32_t memvec_entries, 
					   struct upc_io_local_memvec const *memvec, 
					   uint32_t filevec_entries, struct 
					   upc_io_filevec const *filevec, 
					   upcio_flag_t sync_mode);
extern int64_t upc_all_fwrite_list_shared(upcio_file_t *fd, uint32_t memvec_entries, 
					    struct upc_io_shared_memvec const *memvec, 
					    uint32_t filevec_entries, 
					    struct upc_io_filevec const *filevec, 
					    upcio_flag_t sync_mode);
extern void upc_all_fread_list_local_async(upcio_file_t *fd, uint32_t memvec_entries, 
					   struct upc_io_local_memvec const *memvec, 
					   uint32_t filevec_entries, 
					   struct upc_io_filevec const *filevec,
					   int64_t *ret,
					   upcio_flag_t sync_mode);
extern void upc_all_fread_list_shared_async(upcio_file_t *fd, uint32_t memvec_entries, 
					    struct upc_io_shared_memvec const *memvec, 
					    uint32_t filevec_entries, 
					    struct upc_io_filevec const *filevec,
					    int64_t *ret,
					    upcio_flag_t sync_mode);
extern void upc_all_fwrite_list_local_async(upcio_file_t *fd, uint32_t memvec_entries, 
					    struct upc_io_local_memvec const *memvec, 
					    uint32_t filevec_entries, 
					    struct upc_io_filevec const *filevec, 
					    int64_t *ret,
					    upcio_flag_t sync_mode);
extern void upc_all_fwrite_list_shared_async(upcio_file_t *fd, uint32_t memvec_entries, 
					     struct upc_io_shared_memvec const *memvec, 
					     uint32_t filevec_entries, 
					     struct upc_io_filevec const *filevec, 
					     int64_t *ret,
					     upcio_flag_t sync_mode);
extern void upc_all_fread_local_async(upcio_file_t *fh, void *buffer, upc_off_t size, uint32_t nmemb, 
				      int64_t *ret, upcio_flag_t sync_mode);
extern void upc_all_fwrite_local_async(upcio_file_t *fh, void *buffer, upc_off_t size, uint32_t nmemb, 
				       int64_t *ret, upcio_flag_t sync_mode);
extern void upc_all_fread_shared_async(upcio_file_t *fh, shared void *buffer, uint32_t blocksize, 
				       upc_off_t size, uint32_t nmemb, int64_t *ret, 
				       upcio_flag_t sync_mode);
extern void upc_all_fwrite_shared_async(upcio_file_t *fh, shared void *buffer, uint32_t blocksize, 
					upc_off_t size, uint32_t nmemb, int64_t *ret,
					upcio_flag_t sync_mode);
extern int64_t upc_all_fwait_async( upcio_file_t *fh );
extern int64_t upc_all_ftest_async( upcio_file_t *fh, int *flag );

#endif /*_UPC_IO_H*/

