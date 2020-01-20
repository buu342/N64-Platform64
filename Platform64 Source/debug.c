/***************************************************************
                            debug.c
                               
Allows communication between the N64 cartridge and the PC
***************************************************************/

#include <ultra64.h> 
#include "debug.h"


/*********************************
       Configuration Macros
*********************************/

#define BUFFER_SIZE     512
#define DEBUG_CIADD     0x3F00000
#define DEBUG_CIADDSIZE 1024*1024


/*********************************
          64Drive Macros
*********************************/

// Cartridge Interface definitions. Obtained from 64Drive's Spec Sheet
#define D64_BASE_ADDRESS   0xB0000000
#define D64_CIREG_ADDRESS  0x08000000
#define D64_CIBASE_ADDRESS 0xB8000000

#define D64_REGISTER_STATUS  0x00000200
#define D64_REGISTER_COMMAND 0x00000208
#define D64_REGISTER_LBA     0x00000210
#define D64_REGISTER_LENGTH  0x00000218
#define D64_REGISTER_RESULT  0x00000220

#define D64_REGISTER_MAGIC    0x000002EC
#define D64_REGISTER_VARIANT  0x000002F0
#define D64_REGISTER_BUTTON   0x000002F8
#define D64_REGISTER_REVISION 0x000002FC

#define D64_REGISTER_USBCOMSTAT 0x00000400
#define D64_REGISTER_USBP0R0    0x00000404
#define D64_REGISTER_USBP1R1    0x00000408

#define D64_ENABLE_ROMWR  0xF0
#define D64_DISABLE_ROMWR 0xF1
#define D64_COMMAND_WRITE 0x08

// Cartridge Interface return values
#define D64_MAGIC    0x55444556
#define D64_USB_IDLE 0x00
#define D64_USB_BUSY 0xF0
#define D64_CI_IDLE  0x00
#define D64_CI_BUSY  0x10


/*********************************
         Everdrive Macros
*********************************/

// TODO


/*********************************
           Other Macros
*********************************/

#define CART_NONE      0
#define CART_64DRIVE   1
#define CART_EVERDRIVE 2


/*********************************
         OS RAW Functions
*********************************/

// These functions were renamed in 2.0L so I have to redefine them
extern s32 __osPiRawWriteIo(u32, u32);
extern s32 __osPiRawReadIo(u32, u32 *);
extern s32 __osPiRawStartDma(s32, u32, void *, u32);

#define osPiRawWriteIo(a, b) __osPiRawWriteIo(a, b)
#define osPiRawReadIo(a, b) __osPiRawReadIo(a, b)
#define osPiRawStartDma(a, b, c, d) __osPiRawStartDma(a, b, c, d)

/*********************************
        Function Prototypes
*********************************/

static void debug_64drive_print(char* message);


/*********************************
             Globals
*********************************/

static s8  debug_cart = CART_NONE;
static u8  debug_buffer[BUFFER_SIZE];


/*********************************
         Debug functions
*********************************/

/*==============================
    debug_initialize
    Check if the game is running on a 64Drive or Everdrive.
==============================*/
static void debug_initialize()
{
    u32 buff;
    
    // Initialize the debug related globals
    memset(debug_buffer, 0, BUFFER_SIZE);
    
    // Read the cartridge and check if we have a 64Drive.
    osPiRawReadIo(D64_CIBASE_ADDRESS + D64_REGISTER_MAGIC, &buff);
    if (buff == D64_MAGIC)
        debug_cart = CART_64DRIVE;
}

/*==============================
    _debug_print
    Print a message to the developer's command prompt
    @param A string to print
==============================*/

void _debug_print(char* message)
{
    // If the cartridge hasn't been initialized, do so.
    if (debug_cart == CART_NONE)
        debug_initialize();
        
    // Decide what to do based on which debug cart was detected
    switch(debug_cart)
    {
        case CART_NONE:
            return;
        case CART_64DRIVE:
            debug_64drive_print(message);
            break;
        case CART_EVERDRIVE:
            // TODO
            break;
    }
}


/*********************************
        64Drive functions
*********************************/

/*==============================
    debug_64drive_wait
    Wait until the 64Drive is ready
    @return 0 if success or -1 if failure
==============================*/

static s8 debug_64drive_wait()
{
    u32 ret;
    u32 timeout = 0; // I wanted to use osGetTime() but that requires the VI manager
    
    // Wait until the cartridge interface is ready
    do
    {
        osPiReadIo(D64_CIBASE_ADDRESS + D64_REGISTER_STATUS, &ret); 
        
        // Took too long, abort
        if((timeout++) > 1000000)
            return -1;
    }
    while((ret >> 8) & D64_CI_BUSY);
    
    // Success
    return 0;
}

/*==============================
    debug_64drive_setwritable
    Set the write mode on the 64Drive
    @param A boolean with whether to enable or disable
==============================*/

static void debug_64drive_setwritable(u8 enable)
{
    debug_64drive_wait();
	osPiRawWriteIo(D64_CIBASE_ADDRESS + D64_REGISTER_COMMAND, enable ? D64_ENABLE_ROMWR : D64_DISABLE_ROMWR); 
	debug_64drive_wait();
}


/*==============================
    debug_64drive_waitidle
    Waits for the 64Drive's USB to be idle
==============================*/

static u32 debug_64drive_waitidle()
{
    u32 status;
    do 
    {
        osPiRawReadIo(D64_CIBASE_ADDRESS + D64_REGISTER_USBCOMSTAT, &status);
        status = (status >> 4) & D64_USB_BUSY;    
    }
    while(status != D64_USB_IDLE);
}


/*==============================
    debug_64drive_print
    Prints a message when a 64Drive is connected
    @param A string to print
==============================*/

static void debug_64drive_print(char* message)
{
    u32 i;
    u32 length = strlen(message) + 1;
            
    // Don't allow messages that are too large
    if(length > BUFFER_SIZE)
        length = BUFFER_SIZE;
    
    // Copy the message to the global buffer
    memcpy(debug_buffer, message, length);
    
    // If the message is not 32-bit aligned, pad it
    if(length%4 != 0)
    {
        u32 length_new = (length & ~3)+4;
        for(i=length; i<length_new; i++) 
            debug_buffer[i] = 0;
        length = length_new;
    }
    
    // Spin until the write buffer is free and then set the cartridge to write mode
    debug_64drive_waitidle();
    debug_64drive_setwritable(TRUE);
    
    // Set up DMA transfer between RDRAM and the PI
    osWritebackDCacheAll();
	osPiRawStartDma(OS_WRITE, D64_BASE_ADDRESS + DEBUG_CIADD, debug_buffer, length);
	
    // Write the data to the 64Drive
	osPiRawWriteIo(D64_CIBASE_ADDRESS + D64_REGISTER_USBP0R0, (DEBUG_CIADD) >> 1);
	osPiRawWriteIo(D64_CIBASE_ADDRESS + D64_REGISTER_USBP1R1, (length & 0xffffff) | (0x01 << 24));
	osPiRawWriteIo(D64_CIBASE_ADDRESS + D64_REGISTER_USBCOMSTAT, D64_COMMAND_WRITE);
    
    // Spin until the write buffer is free and then disable write mode
    debug_64drive_waitidle();
    debug_64drive_setwritable(FALSE);
}