#include <stdio.h>
#include <signal.h>
#include <stdio.h>
#include <signal.h>
#include <execinfo.h>
 #include <cstdlib>
#include <vector>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <functional>

// using namespace std;

char* exe = 0;

int initialiseExecutableName() 
{
    char link[1024];
    exe = new char[1024];
    snprintf(link,sizeof link,"/proc/%d/exe",getpid());
    if(readlink(link,exe,sizeof link)==-1) {
        fprintf(stderr,"ERRORRRRR\n");
        exit(1);
    }
    printf("Executable name initialised: %s\n",exe);
}

const char* getExecutableName()
{
    if (exe == 0)
        initialiseExecutableName();
    return exe;
}

/* get REG_EIP from ucontext.h */
#include <ucontext.h>

// void *secret;
// siginfo_t *info;

void bt_sighandler(int sig, siginfo_t *info, void *secret) {

  void *trace[16];
  char **messages = (char **)NULL;
  int i, trace_size = 0;
  ucontext_t *uc = (ucontext_t *)secret;

  /* Do something useful with siginfo_t */
  
  std::cout<<"Do something useful with info"<<std::endl;
  if (sig == SIGSEGV){
    
    std::cout<<"In if"<<std::endl;
    printf("Got signal %d, faulty address is %p, "
           "from %p\n", sig, info->si_addr, 
           uc->uc_mcontext.gregs[REG_RIP]);
  }
  else
  {
    printf("Got signal %d\n", sig);
  }
  
  std::cout<<"after if else"<<std::endl;

  trace_size = backtrace(trace, 16);
  
  std::cout<<"Overwrite sigaction with callers address"<<std::endl;
  /* overwrite sigaction with caller's address */
  trace[1] = (void *) uc->uc_mcontext.gregs[REG_RIP];

  messages = backtrace_symbols(trace, trace_size);
  
  std::cout<<"skip first stack frame"<<std::endl;
  /* skip first stack frame (points here) */
  printf("[bt] Execution path:\n");
  for (i=1; i<trace_size; ++i)
  {
    printf("[bt] %s\n", messages[i]);

    std::cout<<"find first occurance of"<<std::endl;
    /* find first occurence of '(' or ' ' in message[i] and assume
     * everything before that is the file name. (Don't go beyond 0 though
     * (string terminator)*/
    size_t p = 0;
    while(messages[i][p] != '(' && messages[i][p] != ' '
            && messages[i][p] != 0)
    {++p;

    char syscom[256];
    sprintf(syscom,"addr2line %p -e %.*s", trace[i] , p, messages[i] );
    
//     std::cout<<"last param is the filename of the symbol"<<std::endl;
           //last parameter is the filename of the symbol
    system(syscom);}

  }
  exit(0);
}


int func_a(int a, char b) {

  char *p = (char *)0xdeadbeef;

  a = a + b;
  *p = 10;  /* CRASH here!! */

  return 2*a;
}


int func_b() {

  int res, a = 5;

  res = 5 + func_a(a, 't');

  return res;
}

int func_c()
{
    int *p = NULL;
    *p = 1;
}


int main() {

 
  /* Install our signal handler */
  struct sigaction sa;

  sa.sa_handler = (void(*)(int)) bt_sighandler;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;

  sigaction(SIGSEGV, &sa, NULL);
  sigaction(SIGUSR1, &sa, NULL);
  /* ... add any other signal here */
  
//   std::cout<<"Through main before seg fault."<<std::endl;

  /* Do something */
  
   printf("%d\n", func_b());
  

}