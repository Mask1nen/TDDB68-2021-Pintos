#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "filesys/filesys.h"
#include "filesys/file.h"


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
  printf("SYS_WRITE=%d\n",SYS_WRITE);
  printf ("system call!\n=%d\n",*syscall_num);
  char *name = f->esp + 4;
  unsigned *size = (f->esp + 8);
  printf("Hej!\n");
  printf("name: %d\nsize: %i\n",*name, *size);
  switch (*syscall_num) {
    case SYS_HALT:
      power_off();
      break;

    case SYS_CREATE:
      f->eax = createFile(f->esp + 4);
      break;

    case SYS_OPEN:
      f->eax = openFile(f->esp+4);
      break;

    case SYS_CLOSE:
      ; // empty statement in order to declare int
      int *fd = f->esp+4;
      closeFile(*fd);
      break;

    case SYS_READ:
      //f->eax = readFile(f->esp+4);
      break;

    case SYS_WRITE:
      //f->eax = writeFile(f->esp+4);
      break;

    case SYS_EXIT:
      //exit();
      break;

  }
  thread_exit ();
}

bool
createFile(void * esp){
  char *name = esp;
  unsigned *size = (esp + 4);
  printf("Hej!\n");
  printf("name: %d\nsize: %i\n",*name, *size);
  return filesys_create(name, *size);
}

int
openFile(void * esp) {
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
closeFile(int fd){
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
