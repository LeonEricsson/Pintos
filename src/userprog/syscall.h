#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdbool.h>
void syscall_init (void);

void halt();
bool create (const char *file, unsigned inital_size);
int open (const char *file);
void close(int fd);
int read(int fd, void *buffer, unsigned size);
unsigned tell(int fd);
void seek(int fd, unsigned position);
bool remove(const char *file);
int filesize(int fd);
int write(int fd, void *buffer, unsigned size);
void exit(int status);

#endif /* userprog/syscall.h */
