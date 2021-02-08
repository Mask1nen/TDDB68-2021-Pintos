#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <stdint.h>

void syscall_init (void);
bool createFile(void * esp);
int openFile(void * esp);
void closeFile(int fd);

#endif /* userprog/syscall.h */
