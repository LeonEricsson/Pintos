#ifndef USERPROG_PROCESS_H
#define USERPROG_PROCESS_H

#include "threads/thread.h"
#include "threads/synch.h"

struct parent_child{
  int exit_status;
  int alive_count;
  tid_t tid;
  struct semaphore sema;
  void *file_name;
  bool success;
  struct list_elem elem;
  struct lock lock;
};

tid_t process_execute (const char *file_name);
int process_wait (tid_t);
void process_exit (void);
void process_activate (void);

#endif /* userprog/process.h */
