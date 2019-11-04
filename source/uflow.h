#ifndef UFLOW_HPP
#define UFLOW_HPP

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <thread>



#define URBANTHRESHOLD   0.5
#define URBANTEMPERATURE 5.0

#include "layer.h"

#define M_FullStop(s) {printf("\n\nFile = %s, line %d\nMsg = %s\n", \
                       __FILE__,__LINE__,s); \
                       exit(0);}


struct S_heat_source {
   int idx;
   double value;
} ;

int countGreater (double *d, int k, double threashold) ;

void CalcHeat ( double *m
              , double *k
              , double *s
              , int nx, int ny, int nsteps);

int LoopCalcHeat (  const Layer &
                   ,const Layer &
                   ,Layer &
                   ,Layer &
                   ,int
                   ,int
                   ,int   &
                  );

void Enlarge  (double *m, int nx, int ny, int nsteps);
void Enlarge2 (double *m, int nx, int ny, int nsteps);

void SprayUrban (Layer &imgR, const Layer &imgPossible, double temperature, unsigned int, unsigned int);


#endif
