#include "userprog/syscall.h"



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
  int *fd;
  int *status;
  unsigned *size;
  void ** buf;
  void ** desp;
  switch (*syscall_num) {
    case SYS_HALT:
      halt();
      return;

    case SYS_CREATE:
      desp = f->esp + 4;
      unsigned *size = f->esp + 8;
      f->eax = create((char*)*desp, *size);
      return;

    case SYS_OPEN:
      desp = f->esp + 4;
      f->eax = open(*desp);
      return;

    case SYS_CLOSE:
      fd = f->esp + 4;
      close(*fd);
      return;

    case SYS_READ:
      fd = f->esp + 4;
      buf = f->esp + 8;
      size = f->esp + 12;
      f->eax = read(*fd, *buf, *size);
      return;

    case SYS_WRITE:
      fd = f->esp + 4;
      buf = f->esp + 8;
      size = f->esp + 12;
      f->eax = write(*fd, *buf, *size);
      return;

    case SYS_EXEC:
        desp = f->esp + 4;
        f->eax = exec((char*)*desp);
      return;

    case SYS_EXIT:
      status = f->esp + 4;
      exit(*status);
      return;
  }
  thread_exit();
}

void
halt(){
  power_off();
}

bool
create(const char* fname, unsigned size){
  return filesys_create(fname, size);
}

int
open(void * esp) {
  struct thread *currentThread = thread_current();
  int fd = -1;
  for (size_t i = 2; i < 130; i++) {
    if(currentThread->fd[i] == NULL) {
      fd = i;
      break;
    }
  }
  if(fd != -1){
    struct file *file = filesys_open((char*)esp);
    if(file!=NULL){
      currentThread->fd[fd]=file;
    }else{
      fd = -1;
    }
  }
  return fd;
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
  if((f == NULL && fd != 0) || fd == 1) return -1;
  if(fd == 0){
    uint8_t c[size];
    for (size_t i = 0; i < size; i++) {
      c[i] = input_getc();
      printf("%c", c[i]);
    }
    memcpy(buf, (void*) &c, size);
    int bytesw = sizeof(c) / sizeof(uint8_t);
    return bytesw;
  }
  return file_read(f,buf,size);
}

int
write(int fd, const void * buf, unsigned size){
  struct thread *currentThread = thread_current();
  struct file *f = currentThread->fd[fd];
  const char* charbuf = buf;
  if((f == NULL && fd != 1) || fd == 0){
    return -1;
  }
  if(fd == 1){
    putbuf(charbuf, (size_t) size);
    return size;
  }
  return (int) file_write(f,buf,(off_t)size);
}

void
exit(int status){
  printf("%s: exit(%d)\n", thread_name(), thread_current()->pc->exit_status);
  thread_exit();
}

tid_t
exec(const char* cmdline){
  tid_t pid = process_execute(cmdline);
  return pid;
}
