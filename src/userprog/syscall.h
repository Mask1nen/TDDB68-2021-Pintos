#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <stdint.h>

void syscall_init (void);
void halt(void);
bool create(void * esp);
int open(void * esp);
void close(int fd);
int read(int fd, void * buf, unsigned size);
int write(int fd, void * buf, unsigned size);
void exit(int status);

#endif /* userprog/syscall.h */
