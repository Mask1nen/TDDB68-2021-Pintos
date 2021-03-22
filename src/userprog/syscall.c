#include "userprog/syscall.h"
#include "threads/vaddr.h"
#include "pagedir.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
  if(!validate_pointer(f->esp)) {
    exit(-1);
  }
  char *syscall_num = f->esp;
  int *fd;
  int *status;
  unsigned *size;
  unsigned *position;
  void ** buf;
  void ** desp;
  switch (*syscall_num) {

    case SYS_HALT:
      //printf("HALT!\n");
      halt();
      return;

    case SYS_CREATE:
      //printf("CREATE!\n");
      desp = f->esp + 4;
      size = f->esp + 8;
      if(validate_pointer(desp) && validate_pointer(size)) {
        if(validate_string(*desp)) {
          f->eax = create((char*)*desp, *size);
          return;
        }
      }
      exit(-1);
      return;

    case SYS_OPEN:
      //printf("OPEN!\n");
      desp = f->esp + 4;
      if(validate_pointer(desp)) {
        if(validate_pointer(*desp)) {
          f->eax = open(*desp);
          return;
        }
      }
      exit(-1);
      return;

    case SYS_CLOSE:
      //printf("CLOSE!\n");
      fd = f->esp + 4;
      close(*fd);
      return;

    case SYS_READ:
      //printf("READ!\n");
      fd = f->esp + 4;
      buf = f->esp + 8;
      size = f->esp + 12;
      if(validate_pointer(buf) && validate_pointer(size) && validate_pointer(fd)) {
        if(validate_buffer(*buf, *size)) {
          f->eax = read(*fd, *buf, *size);
          return;
        }
      }
      exit(-1);
      return;

    case SYS_WRITE:
      //printf("WRITE!\n");
      fd = f->esp + 4;
      buf = f->esp + 8;
      size = f->esp + 12;
      if(validate_pointer(buf) && validate_pointer(size) && validate_pointer(fd)) {
        if(validate_buffer(*buf, *size)) {
          f->eax = write(*fd, *buf, *size);
          return;
        }
      }
      exit(-1);
      return;

    case SYS_EXEC:
      //printf("EXEC!\n");
      desp = f->esp + 4;
      if(validate_pointer(desp)) {
        if(validate_string(*desp)) {
          f->eax = exec((char*)*desp);
          return;
        }
      }
      exit(-1);
      return;

    case SYS_EXIT:
      //printf("EXIT!\n");
      status = f->esp + 4;
      if(validate_pointer(status)) {
        exit(*status);
        return;
      }
      exit(-1);
      return;

    case SYS_WAIT:
      //printf("WAIT!\n");
      desp = f->esp + 4;
      if(validate_pointer(desp)) {
        f->eax = wait((tid_t)*desp);
        return;
      }
      exit(-1);
      return;

    case SYS_SEEK:
      fd = f->esp + 4;
      position = f->esp + 8;
      if(validate_pointer(fd) && validate_pointer(position)) {
        seek(*fd, *position);
        return;
      }
      exit(-1);
      return;

    case SYS_TELL:
      fd = f->esp + 4;
      if(validate_pointer(fd)) {
        f->eax = tell(*fd);
        return;
      }
      exit(-1);
      return;

    case SYS_FILESIZE:
      fd = f->esp + 4;
      if(validate_pointer(fd)) {
        f->eax = filesize(*fd);
        return;
      }
      exit(-1);
      return;

    case SYS_REMOVE:
      desp = f->esp + 4;
      if(validate_pointer(desp)) {
        if(validate_string(*desp)) {
          f->eax = remove(*desp);
          return;
        }
      }
      exit(-1);
      return;
  }
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
open(void *esp) {
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
    if(file != NULL){
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
  if(!fd || fd == 1 || fd < 0 || fd > 129) return -1;
  struct file *f = currentThread->fd[fd];
  if (!f) return -1;
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
  const char* charbuf = buf;
  if(!fd || fd == 0 || fd < 0 || fd > 129) return -1;
  struct file *f = currentThread->fd[fd];
  if (!f) return -1;

  if(fd == 1){
    putbuf(charbuf, (size_t) size);
    return size;
  }
  return (int) file_write(f,buf,(off_t)size);
}

void
exit(int status){
  thread_current()->pc->exit_status = status;
  printf("%s: exit(%d)\n", thread_name(), thread_current()->pc->exit_status);
  thread_exit();
}

tid_t
exec(const char* cmdline){
  tid_t pid = process_execute(cmdline);
  return pid;
}

int
wait(tid_t pid){
  return process_wait(pid);
}

void seek(int fd, unsigned position) {
  struct file *file = thread_current()->fd[fd];
  file_seek(file, position);
}

unsigned tell(int fd) {
  struct file *file = thread_current()->fd[fd];
  return file_tell(file);
}

int filesize (int fd) {
  struct file *file = thread_current()->fd[fd];
  return file_length(file);
}

bool remove (const char *file_name) {
  return filesys_remove(file_name);
}

bool validate_pointer(void *pntr) {
  if(!pntr) {
    return false;
  }
  else if(!is_user_vaddr(pntr)) {
    return false;
  }
  else if(pagedir_get_page(thread_current()->pagedir, pntr) == NULL) {
    return false;
  }
  return true;
}

bool validate_string(char *string) {
  if(!validate_pointer(string)) return false;
  int counter = 0;
  while(true) {
    if(!validate_pointer((void*)(string + counter))) {
      return false;
    }
    else if(*(char*)(string + counter) == '\0') return true;
    counter++;
  }
  return false;
}

bool validate_buffer(void *buffer, int size) {
  if(!validate_pointer(buffer)) return false;
  for (int i = (int)buffer; i < (int)buffer + size; i++) {
    if(!validate_pointer((void*)i)) {
      return false;
    }
  }
  return true;
}
