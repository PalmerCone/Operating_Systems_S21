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
#include <sys/wait.h>
#include <unistd.h>


//Struct used in main and thread_func
//It is the arg passed to the thread function and will keep track of values
typedef struct thread_data{
  long threadID;
  int initial;
  int product;
}tdata_t;


//Global Variable Declaration
//Number of threads
#define NTHREADS 2
//Number of Command line arguments needed
#define NUMARGS 4
//Contains the Thread IDs of the threads
static pthread_t threads[NTHREADS];

static tdata_t arg[NTHREADS]; 




//Thread function. Recieves a null pointer to a thread_data struct from the calling process
//The thread data struct will have its initial value stored by the process
//the product is calculated and stored along with the ThreadID
void *thread_func(void * arg){

  tdata_t *threaddat = (tdata_t *)arg;
  threaddat->threadID = (long)pthread_self();
  int num;
  num = threaddat->initial;
  //product claculation (initial * 10 = product)
  num = num * 10;
  threaddat->product = num;
  //sets the values in arg for main and printing once finished.
  arg = (tdata_t *)threaddat;


  pthread_exit(NULL);
}







int main (int argc, char ** argv){
    //Sanity check for number of CLA
    //Trips error if the wrong number of args is recieved.
    if (argc != NUMARGS){
      printf("error: incorrect number of command line arguments. \n\t Number of CLA needed: %i\n", NUMARGS);
      printf("\t Number of CLA recieved %i\n",argc);
      return (1);
    }
    //args to be sent to the thread function
    tdata_t *arg[NTHREADS];
    
    //MEM ALLOCATION
    for (int i = 0; i < NTHREADS; i++){
      //allocating memory
      arg[i] = malloc(sizeof(tdata_t));
      //storing initial values into the struct to be used by the thread function to be multiplied.
      arg[i]->initial = atoi(argv[2+i]);
    }


    //creates child process
    int ret_id = fork();
    //error codes and returns
    int err;
    //Checks error statuses on fork()
    if (ret_id < 0){
      printf("Error: The attempted fork() to create a child process was unsuccessful");
      return ret_id;
    }

    //checks if child or parent
    if (ret_id == 0){

        //child process
        //Replaces the process image of the child with the process image of child_function
        err = execl("./child_function","child_function",argv[1],NULL);
        //Checks if process image failed to swap
        if(err<0){

          printf("Error: Failed to change process image from asg1 to child_function.");
          return err;
        }
    }


    else{
      //Parent process
      //waits until child is completed
      wait(NULL);
      //Print Parent info
      printf("Parent process: PID %i and PPID %i\n", getpid(), getppid());
      for (int i = 0; i < NTHREADS; i++){

        //Thread creation. Sends a void pointer pointing at a thread_data structure as the arg for the 
        //thread function
        err = pthread_create(&threads[i], NULL, (void *)&thread_func, (void *)arg[i]);


        //Error check for thread creation
        if(err){

          printf("Thread creation failed with error %d\n",err);
          return err;
        }
      }
      

      //Thread joins
      for(int i = 0; i < NTHREADS; i++){

        //Joins the threads together
        err = pthread_join(threads[i], NULL);

        //error check for joining threads
        if (err != 0){
          printf( "Failed to join Thread with error %d\n", err);
          return err;
        }

        //Outputting Results   
        printf("\tThread %lu, Initial value: %i, Result is: %i\n", arg[i]->threadID,arg[i]->initial,arg[i]->product);
      }
        
      //Free statements for dynamically allocated memory
      for(int i = 0; i < NTHREADS; i++){
        free(arg[i]);
      }
    }

    
    return 0;
}
