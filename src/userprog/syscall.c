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
syscall_handler (struct intr_frame *f UNUSED) {
  int system_call = *(int*)(f->esp);


  switch(system_call){

    case SYS_HALT:{
      halt();
      break;
    }
    case SYS_EXIT:{
      int status = *(int*)(f->esp+4);
      exit(status);
      break;
    }
    case SYS_EXEC:{
      const char *cmd_line = *(char**)(f->esp+4);
      f->eax = exec(cmd_line);
      break;
    }
    case SYS_WAIT:{

    }
    case SYS_CREATE:{
      const char* file = *(char**)(f->esp+4);
      unsigned inital_size = *(unsigned*)(f->esp+8);
      f->eax = create(file, inital_size);
      break;
    }
    case SYS_REMOVE:{

    }
    case SYS_OPEN:{
      const char *file_name = *(char**)(f->esp+4);
      f->eax = open(file_name);
      break;
    }
    case SYS_FILESIZE:{

    }
    case SYS_READ:{
      int fd = *(int*)(f->esp+4);
      void *buffer = *(void**)(f->esp+8);
      unsigned size = *(unsigned*)(f->esp+12);
      f->eax = read(fd, buffer, size);
      break;

    }
    case SYS_WRITE:{
      int fd = *(int*)(f->esp+4);
      void *buffer = *(void**)(f->esp+8);
      unsigned size = *(unsigned*)(f->esp+12);
      f->eax = write(fd, buffer, size);
      break;
    }
    case SYS_SEEK:{


    }
    case SYS_TELL:{

    }
    case SYS_CLOSE:{
      int fd = *(int*)(f->esp+4);
      close(fd);
      break;
    }

  }

}


// ***************** System call implementation *************************

/*    Questions



*/


void halt(){
  power_off();
}

tid_t exec (const char *cmd_line){
    return process_execute(cmd_line);
}


bool create (const char *file, unsigned inital_size){
  return filesys_create(file, inital_size);
}


int open (const char *file_name){
  struct file *file = filesys_open(file_name);
  if(file != NULL){
    for(int i = 2; i < 130 ; i++){
      if(thread_current()->fd_list[i] == NULL){
        thread_current()->fd_list[i] = file;
        return i;
      }
    }
    return -1;
  }
  else{
    return -1;
  }
}


void close(int fd){
  if(thread_current()->fd_list[fd] != NULL){
    file_close(thread_current()->fd_list[fd]);
    thread_current()->fd_list[fd] = NULL;
  }
}


int read(int fd, void *buffer, unsigned size){
  if(fd == 0){
    char* bufferc = (char*)buffer;
    for(int i = 0; i < size; i++){
      bufferc[i] = input_getc();
      bufferc[i+1] = '\0';
    }
    return size;
  }
  else if(fd >= 2 && fd <= 130){
    if(thread_current()->fd_list[fd] != NULL){
      return file_read(thread_current()->fd_list[fd], buffer, size);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}


int write(int fd, void *buffer, unsigned size){
  if(fd == 1){
    putbuf(buffer, size);
    return size;
  }
  else if(fd >= 2 && fd <= 130){
    if(thread_current()->fd_list[fd] != NULL){
      return file_write(thread_current()->fd_list[fd], buffer, size);
    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

void exit(int status){
  thread_exit();
}
