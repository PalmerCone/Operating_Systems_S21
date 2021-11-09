/*
*Palmer Cone
*CPSC 3220 - Operating Systems - Summer 21
*Assignment 2 - Scheduling
*7/9/2021
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
struct list{
    struct task *task;
    struct list *next;
};
//Stucture used to hold each task
struct task{  
    //task identifier. 0-25 is supported
    int  task_id,   
    arrival_time, 
    //Total time spent working until finished
    service_time, 
    //Remaining service time left
    remaining_time,
    //current time at completion
    completion_time,
    //Total time from arrival to finish
    response_time,
    //Time spent waiting. Stops incrementing when remaining time = 0.
    wait_time,
    //boolean expression only used for FIFO
    working;
    //Pointer to next task, If null, that is the end of the list.
    struct task *next;
    };

/*Constructor for the task list. Initilizes Variables of allocated structures.

Params: 
int arrival - Arrival time of task to initialize
int service - Service time required
int counter - task id number
struct task *new_task - pointer to allocated memory for the structure
*/
void constructor(int arrival, int service, int counter, struct task *new_task){

    new_task->task_id = counter;
    new_task->arrival_time=arrival;
    new_task->service_time=service;
    new_task->remaining_time=service;
    new_task->working = 0;
    new_task->next = NULL;
    new_task->wait_time = 0;
    
}
/*
Fuction used for printing the ready queue for the fifo scheduling
struct task *first - pointer to the first task recieved
int time - current time in the scheduling process

*/
void printReadyQueue(struct task *first, int time){
    //placeholder
    struct task *ptr;
    ptr = first;
    //checks if able to immediately go to print and exit.
    if (ptr->next != NULL){
        //iterates through the tasks to see if any are in ready queue.
        while(ptr->next != NULL){
            ptr=ptr->next;
            if (ptr->arrival_time <= time){
                printf("%c%i",('A'+ptr->task_id),ptr->remaining_time);
                if(ptr->next != NULL && ptr->next->arrival_time <= time){
                    printf(", ");
                    
                }
                else{
                    
                    break;
                }
            }
            else{
                printf("--");
                break;
            }
        }
    }
    else{
        printf("--");
    }
}
/*
Fuction used for printing the ready queue for the sjf and rr scheduling.
Allows fucntion to send the ID of the working address so it isnt printed in the ready queue.
struct task *first - pointer to the first task recieved
int time - current time in the scheduling process
int omit_id - id of working task

*/
void printReadyQueue_Omit(struct task *first, int time, int omit_id){
    //placeholder
    struct task *ptr;
    ptr = first;
    //exists is a boolean to determine if there should be 
    //a comma due to it not being the first to be printed
    int exists = 0;
    if(first == NULL ){
        return;
    }
    //iterates through the tasks to see if any are in ready queue.
    while(ptr != NULL){
        if(ptr->task_id == omit_id){
            ptr=ptr->next;
            continue;
        }
        else{
            if(!(ptr->arrival_time > time)){
                if (exists){
                    if(ptr->remaining_time  >0){
                        printf(", ");
                        printf("%c%i",('A'+ptr->task_id),ptr->remaining_time);
                    }
                }
                else{
                    if(ptr->remaining_time > 0){
                        exists = 1;
                        printf("%c%i",('A'+ptr->task_id),ptr->remaining_time);
                    }
                }
            }
        }
        ptr=ptr->next;




    }
    if(!exists){
       printf("--"); 
    }
}
/* 
Helper function used in finding the shortest service time for a service/waittime table.

ptr points to the first element to be checked. it then searches for smaller elements

*/

int findMin(struct task *ptr){
    struct task *temp = ptr;
    int min = INT_MAX;
    if (temp->next == NULL){
        return temp->service_time;
    }
    while (temp != NULL){
        if (temp->service_time < min){
            min = temp->service_time;
        }
            temp=temp->next;
    }
    return min;
    
    
}
/*
Finds and prints the service and wait time tables.

ptr points to the first task.
*/

void minServiceTime(struct task *ptr){
    //temporary and placeholders.
    struct task *first = ptr;
    struct task *temp = ptr;
    struct task *behind = NULL;
    //holds the minimum
    int min = INT_MAX;
    //boolean to leave b/c done printing
    int leave = 0;
    printf("service\t\twait\n");
    printf(" time\t\ttime\n");
    printf("---------------------------------------------------------------\n");
    //Iterates through and prints
    while (temp != NULL){
       if(temp != NULL){
           min = findMin(temp);
       }
       if (temp->service_time == min)
       {    //prints the line if this is the current minimum to be printed
            printf("  %i\t\t%i\n",temp->service_time,temp->wait_time);
            if (temp->next == NULL && behind == NULL){
                //last element
                if(temp != NULL){
                    //frees last element
                    free(temp);
                    temp=NULL;
                }
                break;
            }
            else{
                //not last element
                if (behind != NULL){
                    //not first element
                    if(temp->next == NULL){
                        //only instance remaining
                        if(temp != NULL){
                            //not null. -- Just making sure it didnt somehow get through uncaught
                            if(temp == first){
                                //shouldnt enter this
                                leave=1;
                            }

                            //release allocated memory
                            free(temp);
                            temp = NULL;
                            
                            if (leave){
                                //ends
                                break;
                            }
                            
                        }
                        //reset to beginning
                        behind->next = NULL;
                        temp = first;
                    }
                    else{
                        //not at end of list
                        behind-> next = temp -> next;
                        if(temp != NULL){
                            free(temp);
                            temp = NULL;
                        }
                        temp = first;
                        behind=NULL;
                        
                        
                    }


                }
                else{
                    //first element
                    if (temp->next==NULL){
                        //first and last element
                        if(temp != NULL){
                            //tautalogous
                            
                            //free memory
                            free(temp);
                            temp=NULL;
                        }
                        
                        break;
                    }
                    else{
                        //first but not last
                        first = temp->next;
                        if(temp != NULL){
                            free(temp);
                            temp=NULL;
                            }
                        temp = first;
                    }
                }

            }
       }
       //iterate
       behind = temp;
       temp = temp->next;
       if(temp == NULL &&first != NULL){
           //reset to start
           temp = first;
       }
    }
    
}
/*
NOW UNUSED
REPLACED BY link_list() AND unlink_list() for -rr
moves task from start of queue to the end.
Prempts the task.

First- pointer to the first task that will go to the end.
*/
/*struct task *push_back(struct task *first){
    struct task *temp = first;
    
    if (first->next != NULL){
        while(temp->next != NULL){
            temp=temp->next;
        }
        temp->next=first;
        temp = first;
        first = first -> next;
        temp->next = NULL;
    }
    return first;
}*/
/*
    prints the table of resulting information stored in each task.
    task *ptr first task in list.
*/
void printResults(struct task *ptr){
    //table headers
    printf("\tarrival\tservice\tcompletion\tresponse\twait\n");
    printf("tid\ttime\ttime\ttime\t\ttime\t\ttime\n");
    printf("---------------------------------------------------------------\n");
    //iteration variable
    struct task *tmp = ptr;
    //iterates through and prints
    while (tmp != NULL){
        printf("%c\t%i\t%i\t%i\t\t%i\t\t%i\n",('A'+tmp->task_id), tmp->arrival_time, tmp->service_time,tmp->completion_time, tmp->response_time,tmp->wait_time);
        tmp=tmp->next;
    }
    tmp = ptr;
    //prints the service time tables in accending order of service time
    minServiceTime(ptr);
}
/*
    finds and returns a pointer to the shortest task available at a given point in time
    params: 
    struct task *first - pointer to first task
    int time - current time


*/
struct task *find_shortest_task(struct task *first,int time){
    //placeholder
    struct task *shortest = first;
    //iterator
    struct task *i = first;
    //iterate through and keep the value of the shortest task found so far in shortest
    while(i != NULL){
        if((i->arrival_time <= time) && (i->remaining_time > 0) && (i->remaining_time < shortest->remaining_time || shortest -> remaining_time <= 0)){
            shortest = i;
        }
        i=i->next;
    }
    //returns pointer to shortest task available
    return shortest;
}
/*
    Used in -sjf schedules to increment the wait_time of tasks in the ready queue.
    it might be more efficient and easier to calculate this mathematically.
    struct task *waiting - pointer to running task 
    int working_id - id to skip
    int current_time - current time in schedule
*/
void increment_wait(struct task *waiting,int working_id, int current_time){
    //iterator
    struct task *ptr = waiting;
    //ends if arrival time of any pointer is higher than current time because 
    // they are in order
    while(ptr != NULL && ptr -> arrival_time <= current_time){
        //checks if task is available and not finished
        if (ptr->remaining_time > 0 && ptr -> task_id != working_id){
            //increments wait time
            ptr -> wait_time= ptr->wait_time + 1;
        }
        ptr = ptr -> next;
    }
}
/*
    Used in -rr schedules to increment the wait_time of tasks in the ready queue.
    it might be more efficient and easier to calculate this mathematically.
    Cannot stop early like -sjf because -rr has its order changed
    struct task *waiting - pointer to running task 
    int current_time - current time in schedule
*/
void increment_wait_rr(struct task *waiting, int current_time){
    //iterator
    struct task* tmp = waiting;
    while (tmp != NULL){
        //ends if arrival time of any pointer is higher than current time because 
        // they are in order
        if(tmp->arrival_time <= current_time && tmp->remaining_time > 0){
            //increment wait time
            tmp->wait_time++;
        }
        //iterates with next in list
        tmp= tmp->next;
    }
}
/*
Turns singly linked list into a circular singular linked list
struct task *start - Current first element
*/

void link_list(struct task *start){
    struct task *temp = start;
    while(temp->next != NULL){
        temp = temp->next;
    }
    //attaches last element to first
    temp->next = start;
}
/*
Turns circular singular linked list into a singular linked list. Allows user to break the list at any point
struct task *start - Element to become the first element
*/
void unlink_list(struct task *start){

    struct task *temp = start;
    //Finding the last element
    while(temp->next != start){
        temp = temp->next;
    }
    //detaches back from front
    temp->next = NULL;
}

/*
    Finds the number of tasks from a point - n and returns it
    struct task *first - first task

*/
int get_n(struct task* first){
    int n = 0;
    while(first != NULL){
        //counting elements
        n++;
        first=first->next;
    }
    return n;
}
/*
Uses circular linked list to find the next task that is available 
at a time and returns a pointer to it.
struct task *first - first task
int time - current time

*/
struct task *find_available_task(struct task *first,int time){
    struct task *temp = first;
    //starts on first task but that is used to end the while loop, so a do while loop is used
    do{
        //checks availability
        if(temp->arrival_time <= time && temp -> remaining_time > 0){
            return temp;
        }
        else{
            //iterates if not available
            temp = temp -> next;
        }
    }while(temp != first);
    //if no task is available, return null
    return NULL;

}
/*
Round Robin Scheduling with a time slice of 1.
Uses a circular linked list to do 1 time worth of work 
on a task and then find the next available one.
argv[1] = "-rr"
params:
struct task* first - first element of the list. Holds this location so the list may be unlinked
at different points

*/

void rr(struct task* first){
    //iterators and placeholders
    struct task *ptr = first;
    struct task *tmp = first;
    //holds current time of schedule
    int current_time = 0;
    //number of tasks in list to be completed
    int n = get_n(first);
    //number of tasks that have remaining_time = 0.
    int count = 0;
    //Header
    printf("RR scheduling results (time slice is 1)\n");
    printf("time\tcpu\tready queue (tid/rst)\n");
    printf("----------------------------------\n");

    //creates circularly linked list
    link_list(ptr);
    //When all tasks have completed the exit condition will be satisfied.
    while(count != n){
        tmp = ptr;
        //Null if no task available at that time
        tmp = find_available_task(tmp, current_time);
        if(tmp != NULL){
            ptr = tmp;
            //turns to singly linked list at the available task
            unlink_list(ptr);
            //prints info for table
            printf("%i\t%c%i\t",current_time,('A' + ptr->task_id), ptr->remaining_time);
            ptr->remaining_time= ptr->remaining_time - 1;
            //increments wait on other processes
            increment_wait_rr(ptr->next,current_time);
            //Sends ready queue to output
            printReadyQueue_Omit(ptr,current_time,ptr->task_id);
            //Task completes
            if(ptr->remaining_time == 0){
                
                count++;
                //calculates and stores response and completion time
                ptr->response_time = ptr->wait_time + ptr->service_time;
                ptr->completion_time = current_time + 1;
            }
            //Formatting
            printf("\n");
            //relinks unlinked list
            link_list(ptr);
            //Goes to next element
            ptr=ptr->next;
        }
        else{
            //Disconnects list
            unlink_list(ptr);
            //prints info
            printf("%i\t \t",current_time);
            //Prints ready queue of "--"
            printReadyQueue_Omit(ptr,current_time, ptr->task_id);
            //formatting
            printf("\n");
            //relinks list
            link_list(ptr);
        }
        //increments time to the next
        current_time++;
        
        if(current_time == 1000){
            //Extra break for errors. In event of infinite loop
            printf("Error: Loop Limit 1000 reached.");
            break;
        }   
    }
}

/*
Shortest Job First Scheduling (premptive)
performs work on the shortest job available
argv[1] = "-sjf"
struct task *first - first task in list
*/
void sjf(struct task *first){
    //temp
    struct task *ptr;
    //iterator
    struct task *waiting = first;
    //finds and holds the pointer to shortest available task
    struct task *shortest;
    //print table header
    printf("SJF(preemptive) scheduling results\n");
    printf("time\tcpu\tready queue (tid/rst)\n");
    printf("----------------------------------\n");
    //current time of clock
    int current_time = 0;
    
    while (waiting != NULL){
        ptr = waiting;
        shortest = NULL;
        if(current_time < ptr->arrival_time){
            //Just increments time because time is not here yet and all tasks so far are done
            //print statements
            printf("%i\t \t",current_time);
            //prints ready queue "--"
            printReadyQueue(ptr,current_time);
            //formatting
            printf("\n");
        }
        else{
            //finds shortest task in list, null if none
            shortest = find_shortest_task(ptr, current_time);
            if (shortest == NULL){
                break;
            }
            //waiting tasks increment
            increment_wait(waiting, shortest->task_id, current_time);
            //Prints info to table
            printf("%i\t%c%i\t",current_time,('A' + shortest->task_id), shortest->remaining_time);
            //Decrements time reamining until task completion
            shortest->remaining_time= shortest->remaining_time-1;
            //Prints ready queue of available non working tasks
            printReadyQueue_Omit(waiting,current_time,shortest->task_id);
            
            if (shortest->remaining_time == 0){
                //if shortest task just finished
                //Calculate Variables
                shortest->response_time = shortest->wait_time + shortest->service_time;
                shortest->completion_time = current_time + 1;
                if (waiting->remaining_time == 0){
                    while (waiting != NULL && waiting->remaining_time == 0){
                        //if next NULL is also the task recieved longest that is unfinished
                        waiting = waiting -> next;
                    }
                }
            }
            ///formatting
            printf("\n");
        }
    //increment time
    current_time++;
    if(current_time == 1000){
        //Extra break for errors. In event of infinite loop
        printf("Error: Loop Limit 1000 reached.");
        break;
    }


    }


}
/*
First In, First Out Scheduling (premptive)
performs work on the job that has been there the longest until completion
argv[1] = "-fifo"
struct task *first - first task in list
*/
void fifo(struct task *first){
    //placeholder
    struct task *ptr = first;
    //setting up clock
    int current_time=0;
    //Table header
    printf("FIFO scheduling results\n");
    printf("time\tcpu\tready queue (tid/rst)\n");
    printf("----------------------------------\n");
    //iterates through tasks and works on each until completion
    while (ptr != NULL){
        //Checks if next task is here yet.
        if(current_time < ptr->arrival_time){
            //Just increment time because its not here yet/skip
            //print statements
            printf("%i\t \t",current_time);
            //"--"
            printReadyQueue(ptr,current_time);
            //formatting
            printf("\n");
        }
        else{
            //task arrived
            if (!ptr->working){
                ptr->working = 1;
                if(ptr->arrival_time == current_time){
                    //if no wait
                    ptr->wait_time = 0;
                }
                else{
                    //wait time calculation
                    ptr->wait_time = current_time - ptr->arrival_time; //check this
                }

            }
            
            
            //print statements
            printf("%i\t%c%i\t",current_time,('A' + ptr->task_id), ptr->remaining_time);
            //decrements the time remaining until completion
            ptr->remaining_time= ptr->remaining_time-1;
            //Prints table info
            printReadyQueue(ptr,current_time);
            //did task just finish?
            if(ptr->remaining_time <= 0){
                //calculates info for results
                ptr->completion_time = current_time + 1;//check this
                ptr->response_time = ptr->completion_time - ptr->arrival_time;
                //not working
                ptr->working = 0;
                //Next task
                ptr=ptr->next;
            }
            //formatting
            printf("\n");
           
            
        }
        //increments timer
        current_time++;
        if(current_time == 1000){
        //Extra break for errors. In event of infinite loop
        printf("Error: Loop Limit 1000 reached.");
        break;
    }
    }

}

int main (int argc, char *argv[]){
    //Mallocs first task
    struct task *first = (struct task *)malloc(sizeof(struct task));
    //determines if its the first task and to call malloc
    int DoMalloc = 0;
    //current task to be added on to
    struct task *curr = first;
    //number of tasks, sets task_id
    int task_counter = 0;
    //User input recieved from CLA determines method used
    char *method = argv[1];
    
    
    //Data from file
    int arrival;//arrival_time
    int service;//service_time
    //No input sent. ERROR
    if (feof(stdin)){
        printf("Program requires data to be sent via command line. Cancelling execution.\n\n");
        return 1;
    }
    
    while(!feof(stdin) && scanf("%i %i",&arrival,&service)==2){
        if(DoMalloc){
            //not first in list
            curr->next = (struct task *)malloc(sizeof(struct task));
            curr = curr->next;
        }
        else{
            //malloc already done.
            DoMalloc = 1;
        }

        //Fills the alloocated data.
        constructor(arrival,service,task_counter,curr);
       //Number of tasks increment
        task_counter++;
    }
    //Determines which scheduling user selected

    if(!strcmp(method,"-fifo")){ 
        fifo(first);
    }
    else if(!strcmp(method, "-sjf")){
        sjf(first);
    }
    else if(!strcmp(method, "-rr")){
        rr(first);
        //unlinks circularly linked list
        unlink_list(first);
    }
    else{
        //invalid input
        printf("ERROR: Input Invalid. The only scheduling options available are -fifo, -sjf, and -rr");
    }


    //prints the service/wait time table
    printResults(first);

    return 0;
}



