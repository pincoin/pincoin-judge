#include <stdio.h>
#include <signal.h>

/* hello_loop.c*/
void hello(int signum){
  printf("Hello World!\n");
}

int main(){

  //Handle SIGINT with hello
  signal(SIGINT, hello);

  //loop forever!
  while(1);

}
