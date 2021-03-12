#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
#include <stdint.h>
#include "threads/thread.h"
#include "userprog/process.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/init.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"
#include "lib/kernel/console.h"

void syscall_init (void);
void halt(void);
bool create(const char* fname, unsigned size);
int open(void * esp);
void close(int fd);
int read(int fd, void * buf, unsigned size);
int write(int fd, const void * buf, unsigned size);
tid_t exec(const char* cmdline);
void exit(int status);
int wait(tid_t pid);
bool validate_pointer(void *pntr);
bool validate_string(char *string);
bool validate_buffer(void *buffer, int size);
bool validate_esp(void *esp);

#endif /* userprog/syscall.h */
