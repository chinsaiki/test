#ifndef SCHD_ALGORITHMS_H_
#define SCHD_ALGORITHMS_H_

#include <stdio.h>
#include <math.h>

double Schedulability(unsigned int *Tset, double *WCET, double *deadline, double *period);
int Comparator(unsigned int *Tset, double *deadline, double *period);
int loading_factor_analysis(unsigned int *Tset, double *WCET, double *deadline, double *period);
int EDF_schd(unsigned int *Tset, double *WCET, double *deadline, double *period);
int RM_schd(unsigned int *Tset, double *WCET, double *deadline, double *period);
int DM_schd(unsigned int *Tset, double *WCET, double *deadline, double *period);

#endif
