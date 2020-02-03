#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
void syscall_init (void);

void halt();
bool create (const char *file, unsigned inital_size);
int open (const char *file);
void close(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, void *buffer, unsigned size);
void exit(int status);

#endif /* userprog/syscall.h */
