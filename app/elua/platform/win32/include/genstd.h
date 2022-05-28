#ifndef __GENSTD_H__
#define __GENSTD_H__

#include "type.h"

// STD device name (for devman)
#define STD_DEV_NAME         "/std"

#define STD_INFINITE_TIMEOUT    PLATFORM_UART_INFINITE_TIMEOUT
#define STD_INTER_CHAR_TIMEOUT  10000

// Send/receive function types
typedef void ( *p_std_send_char )( int fd, char c );
typedef int ( *p_std_get_char )( s32 to );

// STD functions
void std_set_send_func( p_std_send_char pfunc );
void std_set_get_func( p_std_get_char pfunc );

#endif
