#ifndef _AM_OPENAT_ZBAR_H_
#define _AM_OPENAT_ZBAR_H_

typedef struct
{
	bool result;
	uint32 decodeTime;
	unsigned char *data;
	uint32 len;
}OPENAT_DCQR_DECODE_T;


int OPENAT_zbar_scanner_open(int width, int height, int size, unsigned char *data);

char * OPENAT_zbar_get_data(int handle, int *len);

BOOL OPENAT_zbar_find_nextData(int handle);

char *OPENAT_zbar_get_type(int handle);
void OPENAT_zbar_scanner_close(int handle);

#endif
