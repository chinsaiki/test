#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "Schd_algorithms.h"

#define Task_10 10
#define Task_25 25
#define Diffrent_Util 10
#define M 3

//uniform_distribution calculator
double uniform_distribution(double rangeLow, double rangeHigh) {
    double myRand = rand()/(1.0 + RAND_MAX);
    double range = rangeHigh - rangeLow;
    double myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

//Function to generate the period using  uniform_distribution
double *period_generation(double *Period,unsigned int Tasks){
  unsigned int Task_division,i;
  Task_division = ceil(Tasks/M);
  for(i=0;i<Tasks;i++)
  {
    if(i<Task_division){
      Period[i] =uniform_distribution(10, 100);
    }
    else if((i>= Task_division) && i<(2*Task_division)){
      Period[i] =uniform_distribution(100, 1000);
    }
    else{
      Period[i] =uniform_distribution(1000, 10000);
    }
  }
  return Period;
}

//Function to generate Utilization for each task in the taskset
double *Unifast(double *Task_Utilization, unsigned int Tasks,unsigned int U){
  unsigned int i;
  double sumU, nextSumU,temp, temp1,random1;
  temp =(double)(U);
  sumU = (double)temp/100;
  for(i=0;i<Tasks-1;i++){
    temp = (double)(Tasks-i);
    temp1 = (double)rand()/(double)RAND_MAX;
    random1 = pow(temp1,1/temp);
    nextSumU = sumU * random1;
    Task_Utilization[i] = sumU - nextSumU;
    sumU = nextSumU;
  }
  Task_Utilization[i] = sumU;
  return Task_Utilization;
}

int main(){
  unsigned int i,j,k,t=1,ret;
  unsigned int EDF_schedulable_count=0,RM_schedulable_count=0,DM_schedulable_count=0;
  unsigned int *Tasksets;
  double *Period_10tasks, *Period_25tasks;
  double *Task_Util_10tasks, *Task_Util_25tasks;
  double *WCET_10tasks, *WCET_25tasks;
  double *Deadline_10tasks, *Deadline_25tasks;
  double temp;

  FILE *fp1,*fp2,*fp3,*fp4;                                     //File pointers for the 4 plots
  fp1=fopen("plot1.csv","w+");                                  //create 4 .csv files to store the data
  fp2=fopen("plot2.csv","w+");
  fp3=fopen("plot3.csv","w+");
  fp4=fopen("plot4.csv","w+");
  fprintf(fp1,"PLOT1\n");
  fprintf(fp1,"Utilization, EDF, RM, DM");                      //Name the columns as Utilization, EDF, RM, DM
  fprintf(fp2,"PLOT2\n");
  fprintf(fp2,"Utilization, EDF, RM, DM");
  fprintf(fp3,"PLOT3\n");
  fprintf(fp3,"Utilization, EDF, RM, DM");
  fprintf(fp4,"PLOT3\n");
  fprintf(fp4,"Utilization, EDF, RM, DM");

//This loop generates data which are required for plot1 and plot3
  for(t=1;t<=2;t++){
    for(i=5;i<100;i=i+10){
      EDF_schedulable_count = 0;                           //make the schedulable tasks to 0
      RM_schedulable_count = 0;
      DM_schedulable_count = 0;
      temp = (double)i;
      if(t==1)                                            //For plot1
        fprintf(fp1,"\n%f",temp/100);
      else
        fprintf(fp3,"\n%f",temp/100);
      for(j=0;j<5000;j++){
        Tasksets = (unsigned int *)malloc(sizeof(unsigned int)*2);       //dynamically create the arrays of size 10
        Period_10tasks = (double *)malloc(sizeof(double)*Task_10);       //to store the period, WCET, deadline, Utilization
        Task_Util_10tasks = (double *)malloc(sizeof(double)*Task_10);
        WCET_10tasks = (double *)malloc(sizeof(double)*Task_10);
        Deadline_10tasks = (double *)malloc(sizeof(double)*Task_10);
        Tasksets[0]=j;                                                   //store the Taskset number in Tasksets[0]
        Tasksets[1]=Task_10;                                             //store the number of Task
        period_generation(Period_10tasks,Task_10);                       //call the period generator
        Unifast(Task_Util_10tasks,Task_10,i);                            //call the utilization divider
        for(k=0;k<Task_10;k++){
          WCET_10tasks[k] = Task_Util_10tasks[k] * Period_10tasks[k];    //calculate the WCET based on the period & Utilization
          if(t==1){
            Deadline_10tasks[k] =uniform_distribution(WCET_10tasks[k], Period_10tasks[k]);   //calculate the deadline -plot1
          }
          else{
            temp = WCET_10tasks[k]+((Period_10tasks[k]-WCET_10tasks[k])/2);  //calculate the deadline -plot1
            Deadline_10tasks[k] =uniform_distribution(temp, Period_10tasks[k]);
          }
        }
        ret=EDF_schd(Tasksets, WCET_10tasks, Deadline_10tasks, Period_10tasks);    //call EDF Schedulability test
        if(ret==0){                                                         //if ret=0 => taskset is schedulable
          EDF_schedulable_count++;                                          //counter for Num of schedulable tasksets
        }
        ret=RM_schd(Tasksets, WCET_10tasks, Deadline_10tasks, Period_10tasks);     //call RM Schedulability test
        if(ret==0){                                                         //if ret=0 => taskset is schedulable
          RM_schedulable_count++;                                          //counter for Num of schedulable tasksets
        }
        ret=DM_schd(Tasksets, WCET_10tasks, Deadline_10tasks, Period_10tasks);    //call DM Schedulability test
        if(ret==0){                                                         //if ret=0 => taskset is schedulable
          DM_schedulable_count++;                                          //counter for Num of schedulable tasksets
        }
      }
      if(t==1){
        // printf("edf count %d\n",EDF_schedulable_count);
        // printf("rm count %d\n",DM_schedulable_count);
        // printf("dm count %d\n",DM_schedulable_count);
        fprintf(fp1,",%d ",RM_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp1,",%d ",EDF_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp1,",%d ",DM_schedulable_count);                   //store the counter values in respective .csv file
      }
      else{
        fprintf(fp3,",%d ",EDF_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp3,",%d ",RM_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp3,",%d ",DM_schedulable_count);                   //store the counter values in respective .csv file
      }

    }
  }

  for(t=1;t<=2;t++){
    for(i=5;i<100;i=i+10){
      EDF_schedulable_count = 0;
      RM_schedulable_count = 0;
      DM_schedulable_count = 0;
      temp = (double)i;
      if(t==1)
        fprintf(fp2,"\n%f",temp/100);
      else
        fprintf(fp4,"\n%f",temp/100);
      for(j=0;j<5000;j++){
        Tasksets = (unsigned int *)malloc(sizeof(unsigned int)*2);
        Period_25tasks = (double *)malloc(sizeof(double)*Task_25);
        Task_Util_25tasks = (double *)malloc(sizeof(double)*Task_25);           //create num of rows=10
        WCET_25tasks = (double *)malloc(sizeof(double)*Task_25);
        Deadline_25tasks = (double *)malloc(sizeof(double)*Task_25);
        Tasksets[0]=j;
        Tasksets[1]=Task_25;
        period_generation(Period_25tasks,Task_25);
        Unifast(Task_Util_25tasks,Task_25,i);
        for(k=0;k<Task_25;k++){
          WCET_25tasks[k] = Task_Util_25tasks[k] * Period_25tasks[k];
          if(t==1){
            Deadline_25tasks[k] =uniform_distribution(WCET_25tasks[k], Period_25tasks[k]);
          }
          else{
            temp = WCET_25tasks[k]+((Period_25tasks[k]-WCET_25tasks[k])/2);
            Deadline_25tasks[k] =uniform_distribution(temp, Period_25tasks[k]);
          }
        }
        ret=EDF_schd(Tasksets, WCET_25tasks, Deadline_25tasks, Period_25tasks);    //call EDF Schedulability test
        if(ret==0){
          EDF_schedulable_count++;                                       //counter for Num of schedulable tasksets
        }
        ret=RM_schd(Tasksets, WCET_25tasks, Deadline_25tasks, Period_25tasks);    //call RM Schedulability test
        if(ret==0){
          RM_schedulable_count++;                                       //counter for Num of schedulable tasksets
        }
        ret=DM_schd(Tasksets, WCET_25tasks, Deadline_25tasks, Period_25tasks);    //call DM Schedulability test
        if(ret==0){
          DM_schedulable_count++;                                       //counter for Num of schedulable tasksets
        }
      }
      if(t==1){
        fprintf(fp2,",%d ",EDF_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp2,",%d ",RM_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp2,",%d ",DM_schedulable_count);                   //store the counter values in respective .csv file
        //printf("count :%d \n",count);
      }
      else{
      //printf("count :%d \n",count);
        fprintf(fp4,",%d ",EDF_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp4,",%d ",RM_schedulable_count);                   //store the counter values in respective .csv file
        fprintf(fp4,",%d ",DM_schedulable_count);                   //store the counter values in respective .csv file
      }
    }
  }
  fclose(fp1);
  fclose(fp2);
  fclose(fp3);
  fclose(fp4);
  return 0;
}
