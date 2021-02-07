#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "filesys/filesys.h"


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

    case SYS_WRITE:
      printf("%s\n", "TELL ME WHY?!");
  }
  thread_exit ();
}

bool
createFile(void * esp){
  char *name = get_user(esp);
  unsigned *size = (esp - 4);
  printf("Hej!"\n);
  printf("name: %d\nsize: %i\n",*name, *size);
  return filesys_create(name, *size);
}

static char
* get_user(const uint8_t *uaddr){
  char *result;
  asm ("mov1 $1f, %0; movzbl %1, %0; 1:"
      : "=&a" (result) : "m" (*uaddr));
  return result;
}
