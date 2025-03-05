#include <stdio.h>
#include <unistd.h>

int main(void)
{   
  printf ("PID: %d\n", (int)getpid());

  for(int i = 0; i < 10; i++) 
    {
      write(1, "Hello, World!\n", 14);
      sleep(2);
    }
  
  getchar();
  
  return 0;
}
