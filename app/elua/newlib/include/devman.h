// Device manager interface for Newlib (stubs and additional IOCTL implementation)

#ifndef __DEVMAN_H__
#define __DEVMAN_H__

#include "type.h"
#include <reent.h>
#include <unistd.h>

// Maximum number of devices in the system
#define DM_MAX_DEVICES        8
#define DM_MAX_DEVICES_BITS   3

// Maximum number of a device name
#define DM_MAX_DEV_NAME       12

// GLOBAL maximum file length (on ALL supported filesystem)
#define DM_MAX_FNAME_LENGTH   30

// Pack/unpack descriptions
// Even if a file descriptor is an 'int', newlib treats it as a short, so we need to stuff
// everything into 16 bits. Actually 15, since negative fd's are actually error indications

/*+\liulean\2015.1.28\将文件句柄改成32位，否则放不下*/
#define DM_MAKE_DESC( devid, fd ) ( ( ( devid ) << ( 31 - DM_MAX_DEVICES_BITS ) ) | ( fd ) )
#define DM_GET_DEVID( desc )      ( ( desc ) >> ( 31 - DM_MAX_DEVICES_BITS ) )
#define DM_GET_FD( desc )         ( ( desc ) & ( ( 31 << ( 31 - DM_MAX_DEVICES_BITS ) ) - 1 ) )
/*-\liulean\2015.1.28\将文件句柄改成32位，否则放不下*/

// STDIO file number
#define DM_STDIN_NUM              0
#define DM_STDOUT_NUM             1
#define DM_STDERR_NUM             2

// Our platform independent "dirent" structure (for opendir/readdir)
struct dm_dirent {
  u32 fsize;
  const char *fname;
  u32 ftime;
};
typedef struct {
  u8 devid;
  void *userdata;
} DM_DIR;

// A device structure with pointers to all the device functions
typedef struct
{
  char name[ DM_MAX_DEV_NAME + 1 ];
  int ( *p_open_r )( const char *path, int flags, int mode );
  int ( *p_close_r )( int fd );
  _ssize_t ( *p_write_r ) ( int fd, const void *ptr, size_t len );
  _ssize_t ( *p_read_r )( int fd, void *ptr, size_t len );  
  off_t ( *p_lseek_r )( int fd, off_t off, int whence );
  void* ( *p_opendir_r )( const char* name );
  struct dm_dirent* ( *p_readdir_r )( void *dir );  
  int ( *p_closedir_r )( void* dir );  
} DM_DEVICE;

// Errors
#define DM_ERR_ALREADY_REGISTERED   (-1)
#define DM_ERR_NOT_REGISTERED       (-2)
#define DM_ERR_NO_SPACE             (-3)
#define DM_ERR_INVALID_NAME         (-4)
#define DM_ERR_NO_DEVICE            (-5)
#define DM_OK                       (0)

// Add a device
int dm_register( const DM_DEVICE *pdev );
// Unregister a device
int dm_unregister( const char* name );
// Get a device entry
const DM_DEVICE* dm_get_device_at( int idx );
// Returns the number of registered devices
int dm_get_num_devices();
// Initialize device manager
int lua_dm_init();

// DM specific functions (uniform over all the installed filesystems)
DM_DIR *dm_opendir( const char* dirname );
struct dm_dirent* dm_readdir( DM_DIR *d );
int dm_closedir( DM_DIR *d );

#endif

