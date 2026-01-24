#ifndef MAILBOX_H
#define MAILBOX_H

// Mailbox registers
#define MMIO_BASE       0x3F000000
#define MAILBOX_BASE    (MMIO_BASE + 0xB880)

#define MAILBOX_READ    (MAILBOX_BASE + 0x00)
#define MAILBOX_STATUS  (MAILBOX_BASE + 0x18)
#define MAILBOX_WRITE   (MAILBOX_BASE + 0x20)

// Mailbox status flags
#define MAILBOX_EMPTY   0x40000000
#define MAILBOX_FULL    0x80000000

// Mailbox channels
#define MAILBOX_CH_POWER   0
#define MAILBOX_CH_FB      1
#define MAILBOX_CH_VUART   2
#define MAILBOX_CH_VCHIQ   3
#define MAILBOX_CH_LEDS    4
#define MAILBOX_CH_BTNS    5
#define MAILBOX_CH_TOUCH   6
#define MAILBOX_CH_COUNT   7
#define MAILBOX_CH_PROP    8  // Property tags (ARM -> VC)

// Request/Response codes
#define REQUEST_CODE        0x00000000
#define REQUEST_SUCCEED     0x80000000
#define REQUEST_FAILED      0x80000001
#define TAG_REQUEST_CODE    0x00000000
#define END_TAG             0x00000000

// Property tags
#define GET_BOARD_REVISION  0x00010002
#define GET_ARM_MEMORY      0x00010005
#define GET_VC_MEMORY       0x00010006

// Framebuffer tags
#define ALLOCATE_BUFFER     0x00040001
#define SET_PHYSICAL_WH     0x00048003
#define SET_VIRTUAL_WH      0x00048004
#define SET_VIRTUAL_OFFSET  0x00048009
#define SET_DEPTH           0x00048005
#define SET_PIXEL_ORDER     0x00048006
#define GET_PITCH           0x00040008

// Functions
int mailbox_call(unsigned int *mbox, unsigned char channel);
unsigned int get_board_revision(void);
void get_arm_memory(unsigned int *base, unsigned int *size);

#endif
