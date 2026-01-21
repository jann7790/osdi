#include "mailbox.h"

#define MMIO_BASE       0x3F000000
#define VIDEOCORE_MBOX  (MMIO_BASE + 0x0000B880)
#define MBOX_READ       (VIDEOCORE_MBOX + 0x0)
#define MBOX_STATUS     (VIDEOCORE_MBOX + 0x18)
#define MBOX_WRITE      (VIDEOCORE_MBOX + 0x20)

#define MBOX_FULL       0x80000000
#define MBOX_EMPTY      0x40000000

static inline void mmio_write(unsigned long reg, unsigned int val) {
    *(volatile unsigned int*)reg = val;
}

static inline unsigned int mmio_read(unsigned long reg) {
    return *(volatile unsigned int*)reg;
}

int mailbox_call(unsigned int* mailbox, unsigned char channel) {
    // Mailbox address must be 16-byte aligned
    unsigned long addr = (unsigned long)mailbox;
    addr &= ~0xF;
    
    // Wait for mailbox to be not full
    while (mmio_read(MBOX_STATUS) & MBOX_FULL);
    
    // Write address of message + channel to mailbox
    mmio_write(MBOX_WRITE, addr | (channel & 0xF));
    
    // Wait for response
    while (1) {
        // Wait for mailbox to be not empty
        while (mmio_read(MBOX_STATUS) & MBOX_EMPTY);
        
        // Read response
        unsigned int response = mmio_read(MBOX_READ);
        
        // Check if it's our message
        if ((response & 0xF) == channel && (response & ~0xF) == addr) {
            // Check response code
            return mailbox[1] == MAILBOX_RESPONSE_SUCCESS;
        }
    }
}

unsigned int get_board_revision(void) {
    unsigned int __attribute__((aligned(16))) mailbox[7];
    
    mailbox[0] = 7 * 4;                    // Buffer size
    mailbox[1] = MAILBOX_REQUEST;          // Request code
    mailbox[2] = TAG_GET_BOARD_REVISION;   // Tag identifier
    mailbox[3] = 4;                        // Value buffer size
    mailbox[4] = 0;                        // Request/response code
    mailbox[5] = 0;                        // Value buffer
    mailbox[6] = TAG_END;
    
    if (mailbox_call(mailbox, MAILBOX_CH_PROP)) {
        return mailbox[5];
    }
    return 0;
}

void get_arm_memory(unsigned int* base, unsigned int* size) {
    unsigned int __attribute__((aligned(16))) mailbox[8];
    
    mailbox[0] = 8 * 4;                    // Buffer size
    mailbox[1] = MAILBOX_REQUEST;          // Request code
    mailbox[2] = TAG_GET_ARM_MEMORY;       // Tag identifier
    mailbox[3] = 8;                        // Value buffer size
    mailbox[4] = 0;                        // Request/response code
    mailbox[5] = 0;                        // Base address
    mailbox[6] = 0;                        // Size
    mailbox[7] = TAG_END;
    
    if (mailbox_call(mailbox, MAILBOX_CH_PROP)) {
        *base = mailbox[5];
        *size = mailbox[6];
    } else {
        *base = 0;
        *size = 0;
    }
}
