#include "syscall.h"
void print(void *c)
{
	//PutChar('k');
  PutChar(* ((char *) c));
  // PutString("Thread ");
  // PutChar(*((char *) c) );
  // PutString(" is executing\n");
}

int main() {
  
  char ch = 'a';
  char* c = &ch;
  
  UserThreadCreate(print, (void *) c);
  PutChar('*');
  
  return 0;
}

