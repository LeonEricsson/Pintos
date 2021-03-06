#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "userprog/process.h"
#include "userprog/pagedir.h"

static void syscall_handler (struct intr_frame *);



void
syscall_init (void)
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) {
  if(is_kernel_vaddr(f->esp) || pagedir_get_page(thread_current()->pagedir, f->esp) == NULL){
    exit(-1);
  }

  int system_call = *(int*)(f->esp);



  switch(system_call){

    case SYS_HALT:{
      halt();
      break;
    }
    case SYS_EXIT:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        int status = *(int*)(f->esp+4);
        exit(status);
        break;
      }
      else{
        exit(-1);
      }

    }
    case SYS_EXEC:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        const char *cmd_line = *(char**)(f->esp+4);
        int i = 0;
        while(true){
          if(is_kernel_vaddr(cmd_line) || pagedir_get_page(thread_current()->pagedir, cmd_line + i) == NULL){
                exit(-1);
            }
          if(cmd_line[i] == NULL) break;
          i++;
        }
        f->eax = exec(cmd_line);
        break;
      }
      else{
        exit(-1);
      }
    }

    case SYS_WAIT:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        tid_t tid = *(tid_t*)(f->esp+4);
        f->eax = wait(tid);
        break;
      }
      else{
        exit(-1);
      }

    }
    case SYS_CREATE:{
      if(is_user_vaddr(f->esp+8) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL
        && pagedir_get_page(thread_current()->pagedir, f->esp+8) != NULL){
        const char* file = *(char**)(f->esp+4);
        unsigned inital_size = *(unsigned*)(f->esp+8);
        int i = 0;
        while(true){
          if(is_kernel_vaddr(file + i) || pagedir_get_page(thread_current()->pagedir, file + i) == NULL){
                exit(-1);
            }
          if(file[i] == NULL) break;
          i++;
        }
        f->eax = create(file, inital_size);
        break;
      }
      else{
        exit(-1);
      }
    }
    case SYS_REMOVE:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        const char* file = *(char**)(f->esp+4);
        int i = 0;
        while(true){
          if(is_kernel_vaddr(file + i) || pagedir_get_page(thread_current()->pagedir, file + i) == NULL){
                exit(-1);
            }
          if(file[i] == NULL) break;
          i++;
        }
        f->eax = remove(file);
        break;
      }
      else{
        exit(-1);
      }


    }
    case SYS_OPEN:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        const char *file_name = *(char**)(f->esp+4);
        int i = 0;
        while(true){
          if(is_kernel_vaddr(file_name + i) || pagedir_get_page(thread_current()->pagedir, file_name + i) == NULL){
            exit(-1);
          }
          if(file_name[i] == NULL) break;
          i++;
        }
        f->eax = open(file_name);
        break;
      }
      else{
        exit(-1);
      }
    }
    case SYS_FILESIZE:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        int fd = *(int*)(f->esp+4);
        f->eax =  filesize(fd);
        break;
      }
      else{
        exit(-1);
      }


    }
    case SYS_READ:{
      if(is_user_vaddr(f->esp+12) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL
        && pagedir_get_page(thread_current()->pagedir, f->esp+8) != NULL
          && pagedir_get_page(thread_current()->pagedir, f->esp+12) != NULL){
        int fd = *(int*)(f->esp+4);
        void *buffer = *(void**)(f->esp+8);
        unsigned size = *(unsigned*)(f->esp+12);
        for(int i = 0; i < size; i++){
          if(is_kernel_vaddr(buffer + i) || pagedir_get_page(thread_current()->pagedir, buffer + i) == NULL){
            exit(-1);
          }
        }
        f->eax = read(fd, buffer, size);
        break;
      }
      else{
        exit(-1);
      }

    }
    case SYS_WRITE:{
      if(is_user_vaddr(f->esp+12) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL
        && pagedir_get_page(thread_current()->pagedir, f->esp+8) != NULL
          && pagedir_get_page(thread_current()->pagedir, f->esp+12) != NULL){
        int fd = *(int*)(f->esp+4);
        void *buffer = *(void**)(f->esp+8);
        unsigned size = *(unsigned*)(f->esp+12);
        for(int i = 0; i < size; i++){
          if(is_kernel_vaddr(buffer + i) || pagedir_get_page(thread_current()->pagedir, buffer + i) == NULL){
            exit(-1);
          }
        }
        f->eax = write(fd, buffer, size);
        break;
      }
      else{
        exit(-1);
      }
    }
    case SYS_SEEK:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL
        && pagedir_get_page(thread_current()->pagedir, f->esp+8) != NULL){
        int fd = *(int*)(f->esp+4);
        unsigned position = *(unsigned*)(f->esp+8);
        seek(fd, position);
        break;
      }
      else{
        exit(-1);
      }
    }
    case SYS_TELL:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        int fd = *(int*)(f->esp+4);
        f->eax = tell(fd);
        break;
      }
      else{
        exit(-1);
      }

    }
    case SYS_CLOSE:{
      if(is_user_vaddr(f->esp+4) && pagedir_get_page(thread_current()->pagedir, f->esp+4) != NULL){
        int fd = *(int*)(f->esp+4);
        if(fd < 0 || fd > 130){
          exit(-1);
        }
        close(fd);
        break;
      }
      else{
        exit(-1);
      }
    }

  }

}


// ***************** System call implementation *************************

/*    Assignment

Implement:

SYS_SEEK ✔
SYS_TELL ✔
SYS_FILESIZE ✔
SYS_REMOVE ✔


Synchronize:

SYS_READ ✔
SYS_WRITE ✔
SYS_CREATE ✔
SYS_OPEN ✔
SYS_CLOSE ✔


*/


void halt(){
  power_off();
}

int wait(tid_t tid){
    return process_wait(tid);
}

tid_t exec (const char *cmd_line){
    return process_execute(cmd_line);
}


bool create (const char *file, unsigned inital_size){
  return filesys_create(file, inital_size);
}

bool remove(const char *file){
  return filesys_remove(file);
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

int filesize(int fd){
  if(thread_current()->fd_list[fd] != NULL){
    return file_length(thread_current()->fd_list[fd]);
  }
  else{
    return -1;
  }
}

int read(int fd, void *buffer, unsigned size){
  if(fd == 0){
    char* bufferc = (char*)buffer;
    for(int i = 0; i < size; i++){
      bufferc[i] = input_getc();
    }
    return size;
  }
  else if(fd >= 2 && fd <= 130){
    struct thread *t = thread_current();
    if(t->fd_list[fd] != NULL){
      int read = file_read(t->fd_list[fd], buffer, size);
      return read;
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
    struct thread *t = thread_current();
    if(t->fd_list[fd] != NULL){
      int write = file_write(t->fd_list[fd], buffer, size);
      return write;

    }
    else{
      return -1;
    }
  }
  else{
    return -1;
  }
}

// Is setting pos to size the end of the file?

void seek(int fd, unsigned position){
  int size = filesize(fd);
  struct thread *t = thread_current();
  if(t->fd_list[fd] != NULL){
    if(position <= size){
      file_seek(t->fd_list[fd], position);
    }
    else{
      file_seek(t->fd_list[fd], size);
    }
  }
  else{

    return -1;
  }
}

unsigned tell(int fd){
  if(thread_current()->fd_list[fd] != NULL){
    return file_tell(thread_current()->fd_list[fd]);
  }
  else{
    return -1;
  }
}


void exit(int status){
  struct thread *t = thread_current();
  t->parent->exit_status = status;
  thread_exit();
}
