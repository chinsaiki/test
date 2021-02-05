#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>
#include <unistd.h>
#include "Schd_algorithms.h"

#define min(a,b) (a<b?a:b)

unsigned int i,density;


//This function calculates the ultilization of the tasks in the taskset
double Schedulability(unsigned int *Tset, double *WCET, double *deadline, double *period){
  unsigned int Num_of_tasks;
  Num_of_tasks = Tset[1];
  double Utilization=0;
  for(i=0;i<Num_of_tasks;i++){
    if(density == 1){                                                      //if deadline < period take minimum of them
      Utilization = Utilization + (WCET[i]/min(deadline[i],period[i]));
    }
    else
      Utilization = Utilization + (WCET[i]/period[i]);
  }
  return Utilization;
}

//This function compares whether the deadline and period
int Comparator(unsigned int *Tset, double *deadline, double *period){
  unsigned int Num_of_tasks;
  unsigned int result=0;
  Num_of_tasks = Tset[1];
  for(i=0;i<Num_of_tasks;i++){
    if(deadline[i] < period[i])
      result =1;
  }
  return result;
}


//This function calculates the loading factor for EDF test
int loading_factor_analysis(unsigned int *Tset, double *WCET, double *deadline, double *period){
  unsigned int Num_of_tasks,j=0,k=0;
  unsigned int ToPrint_flag=1;
  double Load_Utilization=0;
  double busyperiod1=0.0,busyperiod2=0.0,temp_WCET=0, Abs_deadline=0;
  double Array_Abs_deadline[10000],Cumulative_WCET[10000];
  Num_of_tasks = Tset[1];

  //Busy perid calculator
  for(k=0;k<Num_of_tasks;k++){
    busyperiod1 = busyperiod1 + WCET[k];
  }
  while(1){
    busyperiod2=0;
    for(j=0;j<Num_of_tasks;j++){
      busyperiod2 = busyperiod2 + (ceil(busyperiod1/period[j])*WCET[j]);
    }
    if(busyperiod1 == busyperiod2){
      break;
    }
    else if(busyperiod2 > 100000){
      busyperiod1 =0;
    }
    else{
      busyperiod1 = busyperiod2;
    }
  }
  k=0;

  //calculate the absoulte deadline and cumulative WCET for loading factor analysis
  if(busyperiod1 !=0){
  while(1){
    for(j=0;j<Num_of_tasks;j++){
      if(Abs_deadline < busyperiod1){
        Array_Abs_deadline[j+k] = ((period[j])*(k/Num_of_tasks)) + deadline[j];
        Abs_deadline = Array_Abs_deadline[j+k];
      }
      else
        Array_Abs_deadline[j+k] = Abs_deadline;
      Cumulative_WCET[j+k] = temp_WCET + WCET[j];
      Load_Utilization = Cumulative_WCET[j]/Array_Abs_deadline[j];
      if(Load_Utilization > 1){
        //t=(j%Num_of_tasks) +1;
        ToPrint_flag =0;
      }
      temp_WCET = Cumulative_WCET[j+k];
      if(temp_WCET >= busyperiod1)
        break;
    }
    k=j+k;
    if(temp_WCET >= busyperiod1){
      break;
    }
  }}
  if(ToPrint_flag ==0){
    return 1;
  }
  else
    return 0;
}


//EDF Schedulability macro
int EDF_schd(unsigned int *Tset, double *WCET, double *deadline, double *period){
  unsigned int ret;
  unsigned int density;
  double Utilization=0;
  density =Comparator(Tset,deadline,period);                           //comapare the deadline and period
  Utilization=Schedulability(Tset,WCET,deadline,period);               //calculate the utilization
  if((Utilization <= 1) && (density==0)){                              //sufficient & necessary condition
    return 0;                                                          //return 0 if schedulable
  }
  else if((Utilization > 1) && (density==0)){                          //if U>1 ----> not schedulable
    return 1;                                                          //return 1 if not schedulable
  }
  else if(density ==1){                                               //if D<P go for loading_factor_analysis
    ret =loading_factor_analysis(Tset,WCET,deadline,period);
    return ret;                                                          //return 0 if schedulable
  }
  return 0;
}

//This function sorts the deadline, period, WCET with respective priorities and test
float density_period_comparator(unsigned int *Tset, double *WCET, double *deadline, double *period, unsigned int RM_or_DM){
  double *deadline_priority_wcet, *period_priority_wcet;
  double *deadline_priority_period, *period_priority_deadline;
  double *temp_period, *temp_deadline;

  unsigned int Num_of_tasks,i,j;
  float temp;
  unsigned int period_index[25]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
  unsigned int deadline_index[25]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25};
  Num_of_tasks = Tset[1];

//store the pointer into temporary pointer
  deadline_priority_wcet = WCET;
  period_priority_wcet =WCET;
  deadline_priority_period = period;
  period_priority_deadline = deadline;
  temp_period = period;
  temp_deadline = deadline;

//comapare and sort the deadline and swap the Wcet accoringly
   for(j=0;j<Num_of_tasks-1;j++)
   {
        for(i=0; i<Num_of_tasks-j-1; i++)
        {
              if(deadline[i]>deadline[i+1])
              {
                  temp=temp_deadline[i];
                  temp_deadline[i]=temp_deadline[i+1];
                  temp_deadline[i+1]=temp;

                  temp=deadline_priority_period[i];
                  deadline_priority_period[i]=deadline_priority_period[i+1];
                  deadline_priority_period[i+1]=temp;

                  temp=deadline_priority_wcet[i];
                  deadline_priority_wcet[i]=deadline_priority_wcet[i+1];
                  deadline_priority_wcet[i+1]=temp;

                  temp=deadline_index[i];
                  deadline_index[i]=deadline_index[i+1];
                  deadline_index[i+1]=temp;
              }
        }
   }

//comapare and sort the period and swap the Wcet accoringly
   for(j=0;j<Num_of_tasks-1;j++)
   {
        for(i=0; i<Num_of_tasks-j-1; i++)
        {
              if(period[i]>period[i+1])
              {
                  temp=temp_period[i];
                  temp_period[i]=temp_period[i+1];
                  temp_period[i+1]=temp;

                  temp=period_priority_deadline[i];
                  period_priority_deadline[i]=period_priority_deadline[i+1];
                  period_priority_deadline[i+1]=temp;

                  temp=period_priority_wcet[i];
                  period_priority_wcet[i]=period_priority_wcet[i+1];
                  period_priority_wcet[i+1]=temp;

                  temp=period_index[i];
                  period_index[i]=period_index[i+1];
                  period_index[i+1]=temp;
              }
        }
   }
   if(RM_or_DM ==0){
     WCET = period_priority_wcet;
     deadline = period_priority_deadline;
     period = temp_period;
   }
   else{
     WCET = deadline_priority_wcet;
     deadline = temp_deadline;
     period = deadline_priority_period;
   }
   //if deadline priority == period priority  ------> RM else non-RM
      for(j=0; j<Num_of_tasks; j++)
      {
          if(period_index[j] != deadline_index[j]){
            return 1;
          }
      }
      return 0;

}


//This fuction calculates the response time
double response_time_analysis(unsigned int *Tset, double *WCET, double *deadline, double *period, unsigned int RM_or_DM){

  int order_checker =1,j;
  unsigned int Num_of_tasks;
  double r,t_old,t;
  Num_of_tasks = Tset[1];

  if(order_checker == 1){
     for(j=0;j<Num_of_tasks;j++){
       t=t+ WCET[j];
     }
     while(1){
       r=0;
       for(j=0;j<Num_of_tasks-1;j++){
         if(RM_or_DM == 0){
           r=r+(ceil(t/period[j])*WCET[j]);
         }
         else{
           r=r+(ceil(t/deadline[j])*WCET[j]);
         }
       }
       r = r+ WCET[j];
       t_old = t;
       t = r;
       if(t > deadline[j]){
         return 1;
       }
       if(t == t_old){
         return 0;
       }
     }
   }
   return 0;
}

// Following code will perform Non-RM analysis, if the results from the
// UB analysis come back as negative
// Ensure that this part of the code goes into a separate function
// for clarity purposes. Current implementation is not a separate
// function because of static data structures
int effective_util(unsigned int *Tset, double *WCET, double *deadline, double *period,unsigned int RM_or_DM){
  double UB_value,t;
  int hn,i,j;
  unsigned int Num_of_tasks,ret=0;
  Num_of_tasks = Tset[1];

     for(i=Num_of_tasks;i>0;i--){
       ret=0;
        t=0;
        hn=0;
        if(RM_or_DM==1)
          t= WCET[i]/min(deadline[i],period[i]);
        else
          t= WCET[j]/period[i];
        for(j=0;j<i-1;j++){
            if(period[j] <  min(deadline[i],period[i])){
                 t=t+(WCET[j]/period[j]);
                 hn = hn + 1;
            } else {
                 t=t+(WCET[j]/period[i]);
            }
        }
        hn = hn + 1;
        UB_value = hn * (pow(2,1.0/hn) -1);
        if(t>UB_value){
          Tset[1] =Num_of_tasks;
          ret =response_time_analysis(Tset,WCET,deadline,period,0);
        }
        if(ret == 1){
          return ret;
        }
  }
  return ret;
}



// Following code will perform RM analysis, if the results from the
// UB analysis come back as negative
// Ensure that this part of the code goes into a separate function
// for clarity purposes. Current implementation is not a separate
// function because of static data structures
int RM_schd(unsigned int *Tset, double *WCET, double *deadline, double *period){
  unsigned int *temp_Tset;
  double *temp_WCET, *temp_deadline, *temp_period;

  unsigned int Num_of_tasks,ret,non_RM_flag;
  double Utilization=0,UB;
  Num_of_tasks = Tset[1];

  temp_Tset = (unsigned int *)malloc(sizeof(unsigned int)*2);
  temp_period = (double *)malloc(sizeof(double)*Num_of_tasks);
  temp_WCET = (double *)malloc(sizeof(double)*Num_of_tasks);
  temp_deadline = (double *)malloc(sizeof(double)*Num_of_tasks);

  temp_WCET = WCET;
  temp_Tset = Tset;
  temp_period = period;
  temp_deadline = deadline;

  UB = Num_of_tasks * (pow(2,1.0/Num_of_tasks) -1);
  density =Comparator(temp_Tset,temp_deadline,temp_period);
  Utilization=Schedulability(temp_Tset,temp_WCET,temp_deadline,temp_period);
  non_RM_flag =density_period_comparator(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
  if((Utilization <= UB) && (density==0)){
    return 0;
  }
  else if((Utilization <= UB) && (non_RM_flag==0)){
    return 0;
  }
  else if((Utilization <= UB) && (non_RM_flag==1)){
    ret =effective_util(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
    return 0;
  }
  else if((Utilization > UB) && (density==0)){
    ret =response_time_analysis(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
    return ret;
  }
  else if((Utilization > UB) && (non_RM_flag == 0)){
    ret =response_time_analysis(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
    return ret;
  }
  else if((Utilization > UB) && (non_RM_flag == 1)){
    ret =effective_util(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
    return ret;
  }
  return 0;

}



// Following code will perform DM analysis, if the results from the
// UB analysis come back as negative
// Ensure that this part of the code goes into a separate function
// for clarity purposes. Current implementation is not a separate
// function because of static data structures

int DM_schd(unsigned int *Tset, double *WCET, double *deadline, double *period){
  unsigned int *temp_Tset;
  double *temp_WCET, *temp_deadline, *temp_period;

  unsigned int Num_of_tasks,ret,non_RM_flag;
  double Utilization=0,UB;
  Num_of_tasks = Tset[1];

  temp_Tset = (unsigned int *)malloc(sizeof(unsigned int)*2);
  temp_period = (double *)malloc(sizeof(double)*Num_of_tasks);
  temp_WCET = (double *)malloc(sizeof(double)*Num_of_tasks);
  temp_deadline = (double *)malloc(sizeof(double)*Num_of_tasks);

  temp_WCET = WCET;
  temp_Tset = Tset;
  temp_period = period;
  temp_deadline = deadline;

  UB = Num_of_tasks * (pow(2,1.0/Num_of_tasks) -1);
  density =Comparator(temp_Tset,temp_deadline,temp_period);
  Utilization=Schedulability(temp_Tset,temp_WCET,temp_deadline,temp_period);
  non_RM_flag =density_period_comparator(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
  if((Utilization <= UB) && (density==0)){
    return 0;
  }
  else if((Utilization <= UB) && (non_RM_flag==0)){
    return 0;
  }
  else if((Utilization <= UB) && (non_RM_flag==1)){
    ret =effective_util(temp_Tset,temp_WCET,temp_deadline,temp_period,1);
    return 0;
  }
  else if((Utilization > UB) && (density==0)){
    ret =response_time_analysis(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
    return ret;
  }
  else if((Utilization > UB) && (non_RM_flag == 0)){
    ret =response_time_analysis(temp_Tset,temp_WCET,temp_deadline,temp_period,0);
    return ret;
  }
  else if((Utilization > UB) && (non_RM_flag == 1)){
    ret =effective_util(temp_Tset,temp_WCET,temp_deadline,temp_period,1);
    return ret;
  }
  return 0;

}
