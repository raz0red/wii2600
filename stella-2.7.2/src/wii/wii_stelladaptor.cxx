
#include <gctypes.h>
#include <gccore.h>
#include <ogc/ipc.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include "wii_stelladaptor.hxx"

#define DEVLIST_MAXSIZE 0x08

int wii_stelladaptor_open()
{
    s32 fd = 0;	
#if 0
    return USB_OpenDevice( "oh0", 0x4d8, 0xbeef, &fd );
#endif

    fprintf( stderr, "%d\n", IOS_Open("/dev/usb/oh0/03d8/beef", 1 ) );

    // Iterate devices
	static u8 *buffer = 0;
      
	if( !buffer ) 
    {
		buffer = (u8*)memalign( 32, DEVLIST_MAXSIZE << 3 );
	}

	if( buffer == NULL ) 
    {
		return -100;
	}
	memset( buffer, 0, DEVLIST_MAXSIZE << 3 );

	u8 dummy;
	u16 vid,pid;

	if( USB_GetDeviceList( "/dev/usb/oh0", buffer, DEVLIST_MAXSIZE, 0, &dummy) < 0 )
	{
		free( buffer );
		buffer = 0;
		return -101;
	}	

	int i;

	for( i = 0; i < DEVLIST_MAXSIZE; i++ )
	{
		memcpy( &vid, ( buffer + (i << 3) + 4), 2 );
		memcpy( &pid, ( buffer + (i << 3) + 6), 2 );
		
		if( ( vid==0 ) && ( pid==0 ) )
			continue;

        for( int j = 0; j < 3; j++ )
        {
            fd = 0;
            int retval = USB_OpenDevice( "oh0", vid, pid, &fd );
            fprintf( stderr, "%x, %x, %d, %d\n", vid, pid, retval, fd );
        }
    }

    return -1;
}
