/*
*Palmer Cone
*CPSC 3220 - Operating Systems - Summer 21
*Assignment 1 - Threads
*6/6/2021
*/
//Header Files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>

//Global Variables
#define NUMARGS 2


int main(int argc, char *argv[]){
  //Checking for correct number of CLAs
  if (argc != NUMARGS){
      printf("error: incorrect number of command line arguments. \n\t Number of CLA needed: %i\n", NUMARGS);
      printf("\t Number of CLA recieved %i\n",argc);
      return (1);
  }

  //Recieve and increment number
  int num = atoi(argv[argc-1]);
  int num2 = num + 10;

  //Print results
  printf("Child process: PID %i and PPID %i", getpid(), getppid());
  printf(", initial value: %i, result: %i\n\n", num, num2);


  
  exit(0);
}
