#define _GNU_SOURCE
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include "Schd_algorithms.h"

size_t Total_tasksets;
unsigned int i;
double WCET[100][100], deadline[100][100];
double period[100][100];
unsigned int Tasksets[100][1];


//Function to split th text and return the tokens
char **strsplit(const char* str, const char* delim, size_t* numtokens) {
    char *s = strdup(str);
    size_t tokens_alloc = 1;
    size_t tokens_used = 0;
    char **tokens = calloc(tokens_alloc, sizeof(char*));
    char *token, *strtok_ctx;
    for (token = strtok_r(s, delim, &strtok_ctx);token != NULL;token = strtok_r(NULL, delim, &strtok_ctx)) {
        if (tokens_used == tokens_alloc) {
            tokens_alloc = tokens_alloc*2;
            tokens = realloc(tokens, tokens_alloc * sizeof(char*));
        }
        tokens[tokens_used++] = strdup(token);
    }
    if (tokens_used == 0) {
        free(tokens);
        tokens = NULL;
    }
    else {
        tokens = realloc(tokens, tokens_used * sizeof(char*));
    }
    *numtokens = tokens_used;
    free(s);
    return tokens;
}

int main(void) {
    cpu_set_t  mask;                                       //create a mask of cpu_set_t type
    CPU_ZERO(&mask);                                       //make the mask to zero
    CPU_SET(0, &mask);
    if((sched_setaffinity(0, sizeof(mask), &mask)) !=0){    //set the cpu affinity ----CPU0
      printf("CPU affinity not set\n");
      return 1;
    }
    char *line = NULL;
    char **tokens;
    size_t linelen,numtokens;
    unsigned int Taskset_num=0, Tasks_per_taskset,ret;
    double temp1;
    unsigned int temp_task,schd_flag=0;
    int toread_tasksets=1;
    FILE * fp;                                                  //file pointer

    double *WCET, *deadline, *period;
    unsigned int *Tasksets;

    fp = fopen("test.txt", "r");                               //open the text file
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while (getline(&line, &linelen, fp) != -1) {                    //get the new line from the file each time
      if(toread_tasksets ==1){                                      //to copy the no. of tasksets in the file i.e. 1st line
        tokens = strsplit(line, ", \t\n", &numtokens);              //call the string split function which returns the tokens
        Total_tasksets =atoi(tokens[0]);                            //convert the ascii value of the token to integer
        //printf("Total tasksets: %zu\n",Total_tasksets);
        toread_tasksets =0;                                         //make the variable to 0 as this should read only once
      }
      else{
        tokens = strsplit(line, ", \t\n", &numtokens);              //call the string split function which returns the tokens
        if(numtokens == 1){                                         //This says the no. of tasks in a taskset
          Tasks_per_taskset =atoi(tokens[0]);                       //convert the ascii value of the token to integer
          Taskset_num++;
          Tasksets = (unsigned int *)malloc(sizeof(unsigned int)*2);
          WCET = (double *)malloc(sizeof(double)*Tasks_per_taskset);
          period = (double *)malloc(sizeof(double)*Tasks_per_taskset);
          deadline = (double *)malloc(sizeof(double)*Tasks_per_taskset);
          Tasksets[0] = Taskset_num;
          printf("---------------------Taskset: %d----------------------\n", Tasksets[0]);
          Tasksets[1] = Tasks_per_taskset;
          temp_task =0;                                             //make the variable to 0 to store WCET, deadline & period
        }
        else{
          for(i = 0; i < numtokens; i++) {                         //loop to store WCET, deadline & period
            temp1 =atof(tokens[i]);                                //store the token in temp1 variable
            switch(i){
              case 0:                                              //1st token ->WCET
                WCET[temp_task] = temp1;
                printf("WCET[%u]      :%f\n",temp_task,WCET[temp_task]);
                break;
              case 1:                                              //2nd token ->deadline
                deadline[temp_task] = temp1;
                printf("deadline[%u]  :%f\n",temp_task,deadline[temp_task]);
                break;
              case 2:                                              //3rd token ->period
                period[temp_task] = temp1;
                printf("period[%u]    :%f\n",temp_task,period[temp_task]);
                break;
            }
          }
          temp_task++;
        }
        schd_flag++;
        if(schd_flag == Tasks_per_taskset+1){
          ret=EDF_schd(Tasksets, WCET, deadline, period);               //call the EDF schedule test
          if(ret==0){
            printf("Taskset:%d  Test:EDF  Result: Can be scheduled  Status: Success \n",Tasksets[0]);
          }
          else{
            printf("Taskset:%d  Test:EDF  Result: Cannot be scheduled Status: Failure\n",Tasksets[0]);
          }
          //printf("EM value: %d\n",ret);
          ret=RM_schd(Tasksets, WCET, deadline, period);               //call the RM schedule test
          if(ret==0){
            printf("Taskset:%d  Test:RM   Result: Can be scheduled  Status: Success \n",Tasksets[0]);
          }
          else{
            printf("Taskset:%d  Test:RM   Result: Cannot be scheduled Status: Failure\n",Tasksets[0]);
          }
          //printf("RM value: %d\n",ret);
          ret=DM_schd(Tasksets, WCET, deadline, period);               //call the DM schedule test
          if(ret==0){
            printf("Taskset:%d  Test:DM   Result: Can be scheduled  Status: Success \n",Tasksets[0]);
          }
          else{
            printf("Taskset:%d  Test:DM   Result: Cannot be scheduled Status: Failure\n",Tasksets[0]);
          }
          printf("------------------------------------------------------------------\n");
          schd_flag=0;
        }
      }
      }
    if (tokens != NULL)
      free(tokens);
                                                                   //as function argument
    if (line != NULL)
      free(line);
    return EXIT_SUCCESS;
}
