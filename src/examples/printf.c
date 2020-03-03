#include <stdio.h>
#include <syscall.h>

int
main (int argc, char **argv)
{
  //printf ("You got it, use your debugging skills during the labs!\n");
  //create((char*)0, 1);
  create("adam", 4);
  return EXIT_SUCCESS;
}
