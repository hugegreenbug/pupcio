/************************************************************************/
/*                       UPC-ADIO Implementation                        */
/*                                                                      */
/*  This implementation is an ADIO for UPC. For more information about  */
/*  the ADIO definition or for licence information, please refer to     */
/*  the README file.                                                    */
/*                                                                      */
/*              HPCL, The George Wasnington University                  */
/*                        Author: Kun Xi				*/
/*                       E-mail: kunxi@gwu.edu                          */
/*                                                                      */
/*         Modified for PLFS by: Hugh Greenberg <hng@lanl.gov>          */
/************************************************************************/

// adio_write_stride.c
// ADIO using PLFS
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "adio.h"

int64_t UPC_ADIO_WriteStrided (
       Plfs_fd *fd, 
       uint32_t mem_list_count,
       void *mem_ptrs,
       uint64_t *mem_lengths,
       uint32_t file_list_count,
       uint64_t *file_offsets,
       uint64_t *file_lengths,
       int *error_code ) 
{
    int err;
    uint32_t i, j;
    uint64_t ml, fl;
    int64_t len;
    unsigned char* buf;
    unsigned char** mp;
    uint64_t offset;
    uint64_t total_written;

    i = j = 0;
    mp = (unsigned char**) mem_ptrs;
    buf = mp[0];
    offset = file_offsets[0];
    total_written = 0;
    ml = mem_lengths[0];
    fl = file_lengths[0];
    *error_code = UPC_ADIO_SUCCESS;

    while ( i< mem_list_count && j< file_list_count )
    {
        len = ml < fl ? ml : fl;
        len = UPC_ADIO_WriteContig( fd, buf, len, offset, &err );
        if( err != UPC_ADIO_SUCCESS )
        {
            *error_code = UPC_ADIO_FAILURE;
            return 0;
        }

        if( len <= 0 )
            break;

        buf += len;
	total_written += len;
        offset += len;
        ml -= len;
        fl -= len;

        if( ml == 0 )
        {
            i++;
            buf = mp[i];
            ml = mem_lengths[i];
        }

        if( fl == 0 )
        {
            j++;
            offset = file_offsets[j];
            fl = file_lengths[j];
        }
    }

    return total_written;
}

