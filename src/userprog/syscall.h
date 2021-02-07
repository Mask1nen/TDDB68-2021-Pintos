#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <stdint.h>

void syscall_init (void);
bool createFile(void * esp);
static char * get_user(const uint8_t *uaddr);

#endif /* userprog/syscall.h */
