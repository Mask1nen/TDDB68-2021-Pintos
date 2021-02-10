#include "userprog/syscall.h"
#include "userprog/process.h"
#include <stdio.h>
#include <string.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "devices/input.h"
#include "lib/kernel/console.h"


static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  char *syscall_num = f->esp;
  printf("%d\n",*syscall_num);
  int *fd;
  int *status;
  unsigned *size;
  void ** buf;

  switch (*syscall_num) {
    case SYS_HALT:
      halt();
      break;

    case SYS_CREATE:
      f->eax = create(f->esp + 4);
      break;

    case SYS_OPEN:
      f->eax = open(f->esp + 4);
      break;

    case SYS_CLOSE:
      fd = f->esp + 4;
      close(*fd);
      break;

    case SYS_READ:
      fd = f->esp + 4;
      buf = f->esp + 8;
      size = f->esp + 12;
      f->eax = read(*fd, *buf, *size);
      break;

    case SYS_WRITE:
      fd = f->esp + 4;
      printf("%d\n",*fd);
      buf = f->esp + 8;
      size = f->esp + 12;
      printf("%d\n",*size);
      f->eax = write(*fd, *buf, *size);
      break;

    case SYS_EXIT:
      status = f->esp + 4;
      exit(*status);
      break;

  }
  thread_exit ();
}

void
halt(){
  power_off();
}

bool
create(void * esp){
  char *name = esp;
  unsigned *size = (esp + 4);
  return filesys_create(name, *size);
}

int
open(void * esp) {
  struct thread *currentThread = thread_current();
  size_t arraySize = sizeof(currentThread->fd) / 4;
  if(arraySize < 130) {
    struct file *file = filesys_open(esp);
    if(file) {
      for (size_t i = 2; i < 130; i++) {
        if(!currentThread->fd[i]) {
          currentThread->fd[i] = file;
          return i;
        }
      }
    }
  }
  return -1;
}

void
close(int fd){
  if(fd < 130 && fd > 1){
    struct thread *currentThread = thread_current();
    size_t arraySize = sizeof(currentThread->fd) / 4;
    if(arraySize > 2) {
      if(currentThread->fd[fd]){
        file_close(currentThread->fd[fd]);
        currentThread->fd[fd] = NULL;
      }
    }
  }
}

int
read(int fd, void * buf, unsigned size){
  struct thread *currentThread = thread_current();
  struct file *f = currentThread->fd[fd];
  if(!f) return -1;
  if(fd == 0){
    uint8_t c[size];
    for (size_t i = 0; i < size; i++) {
      c[i] = input_getc();
    }
    memcpy(buf, (void*) &c, size);
    int bytesw = sizeof(c) / sizeof(uint8_t);
    return bytesw;
  }
  return file_read(f,buf,size);
}

int
write(int fd, void * buf, unsigned size){
  struct thread *currentThread = thread_current();
  struct file *f = currentThread->fd[fd];
  if(!f) return -1;
  if(fd == 1){
    putbuf((char*)buf, size);
    return size;
  }
  return (int) file_write(f,buf,size);
}

void
exit(int status){
  struct thread *currentThread = thread_current();
  for (size_t i = 2; i < 130; i++) {
    if(currentThread->fd[i]){
      close(i);
    }
  }
  process_exit();
}
