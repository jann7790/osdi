#include "mailbox.h"

// MMIO read/write functions
static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int *)reg = val;
}

static inline unsigned int mmio_read(unsigned long reg) {
    return *(volatile unsigned int *)reg;
}

/**
 * mailbox_call - Make a mailbox call to the GPU
 * @mbox: Pointer to mailbox message buffer (must be 16-byte aligned)
 * @channel: Mailbox channel number (usually 8 for property tags)
 * 
 * Returns: 0 on success, -1 on failure
 */
int mailbox_call(unsigned int *mbox, unsigned char channel) {
    // Combine message address (upper 28 bits) with channel (lower 4 bits)
    // The message buffer address must be 16-byte aligned
    unsigned int r = ((unsigned int)((unsigned long)mbox) & ~0xF) | (channel & 0xF);
    
    // Wait until mailbox is not full
    while (mmio_read(MAILBOX_STATUS) & MAILBOX_FULL) {
        // Wait
    }
    
    // Write the message address + channel to Mailbox 1 Write register
    mmio_write(MAILBOX_WRITE, r);
    
    // Wait for the response
    while (1) {
        // Wait until mailbox is not empty
        while (mmio_read(MAILBOX_STATUS) & MAILBOX_EMPTY) {
            // Wait
        }
        
        // Read from Mailbox 0 Read register
        unsigned int response = mmio_read(MAILBOX_READ);
        
        // Check if the response matches our request
        if (response == r) {
            // Check if the request was successful
            return mbox[1] == REQUEST_SUCCEED ? 0 : -1;
        }
    }
    
    return -1;
}

/**
 * get_board_revision - Get the board revision number
 * 
 * Returns: Board revision number (e.g., 0xa020d3 for RPi3 B+)
 */
unsigned int get_board_revision(void) {
    // Message buffer must be 16-byte aligned
    unsigned int __attribute__((aligned(16))) mailbox[7];
    
    mailbox[0] = 7 * 4;                 // Buffer size in bytes
    mailbox[1] = REQUEST_CODE;          // Request code
    // Tags begin
    mailbox[2] = GET_BOARD_REVISION;    // Tag identifier
    mailbox[3] = 4;                     // Value buffer size
    mailbox[4] = TAG_REQUEST_CODE;      // Request/response code
    mailbox[5] = 0;                     // Value buffer (response)
    // Tags end
    mailbox[6] = END_TAG;               // End tag
    
    if (mailbox_call(mailbox, MAILBOX_CH_PROP) == 0) {
        return mailbox[5];
    }
    
    return 0;
}

/**
 * get_arm_memory - Get ARM memory base address and size
 * @base: Pointer to store base address
 * @size: Pointer to store memory size
 */
void get_arm_memory(unsigned int *base, unsigned int *size) {
    // Message buffer must be 16-byte aligned
    unsigned int __attribute__((aligned(16))) mailbox[8];
    
    mailbox[0] = 8 * 4;                 // Buffer size in bytes
    mailbox[1] = REQUEST_CODE;          // Request code
    // Tags begin
    mailbox[2] = GET_ARM_MEMORY;        // Tag identifier
    mailbox[3] = 8;                     // Value buffer size (2 x 4 bytes)
    mailbox[4] = TAG_REQUEST_CODE;      // Request/response code
    mailbox[5] = 0;                     // Base address (response)
    mailbox[6] = 0;                     // Size (response)
    // Tags end
    mailbox[7] = END_TAG;               // End tag
    
    if (mailbox_call(mailbox, MAILBOX_CH_PROP) == 0) {
        *base = mailbox[5];
        *size = mailbox[6];
    } else {
        *base = 0;
        *size = 0;
    }
}
