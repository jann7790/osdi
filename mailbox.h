#ifndef MAILBOX_H
#define MAILBOX_H

#define MAILBOX_REQUEST    0
#define MAILBOX_RESPONSE_SUCCESS 0x80000000
#define MAILBOX_RESPONSE_ERROR   0x80000001

// Tags
#define TAG_GET_BOARD_REVISION  0x00010002
#define TAG_GET_ARM_MEMORY      0x00010005
#define TAG_END                 0x00000000

// Channel
#define MAILBOX_CH_PROP 8

int mailbox_call(unsigned int* mailbox, unsigned char channel);
unsigned int get_board_revision(void);
void get_arm_memory(unsigned int* base, unsigned int* size);

#endif
