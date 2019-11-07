#include "uflow.h"
#include "layer.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <random>
#include <vector>


//---------------------------------------------------
/*!
   Counts pixels
   \return number of pixels above threshold, <br>
   -1 if no data available.
*/
int countGreater (double *d, int k, double threashold) {

   if (!k) return -1;

   int n = 0;

   while (k--)
      if (threashold < d[k])
         ++n;

   return n;
};



//---------------------------------------------------
/*!
   Multiply array by double.
*/
void multiply (double *d, int k, double v) {

   if (!k) return;

   int n = 0;

   while (k--)
      d[k] *= v;

};


//-----------------------------------------------------------------------------
/*!
  While Heat Equation is computed, hot zones cool down as they <br>
    loose energy to neighbor cells. <br>
  This function 're-kindles' an image.
*/
void HeatSources (double *plate, double *heatsources, int len) {

   if (NULL == plate      ) M_FullStop ("null plate");
   if (NULL == heatsources) M_FullStop ("null heatsources");

   while (--len) {
    
      if (*plate < *heatsources) 
         *plate = *heatsources;
      
      plate++;
      heatsources++;
   }
}

//-----------------------------------------------------------------------------
/*!
  Computes one step of heat equation. Explicit scheme.
*/
void OldStepHeat (int **x, int **xnew, int **k,
                  int nx, int ny) {

/*
  k = thermal conductivity.
  dt < h*h / (4*k);
  if k = 1, dt < (h*h)/4
*/
   const double h2x = 1.0;
   const double h2y = 1.0;
   const double dt  = 0.03125;

   // remove borders from calculation
   --nx; --ny;

   // go for it
   for (int i = 1; i <= nx; i++)
      for (int j = 1; j <= ny; j++)
         xnew[i][j] = x[i][j] +
                      k[i][j] * dt * (
                        ((x[i+1][j  ] - 2*x[i][j] + x[i-1][j  ]) / h2x)
                        +
                        ((x[i  ][j+1] - 2*x[i][j] + x[i  ][j-1]) / h2y)
                      );
}


/**
 *****************************************************************************************
 *  @brief      Computes one step of heat equation. Explicit scheme.
 *
 *  @usage      This API can be called at any time
 *
 *  @param      m  heat field
 *  @param      nm new heat field
 *  @parm       k  thermal conductivity
 *  @param      nx,ny size
 ****************************************************************************************/

void StepHeat (double *m, double *nm, double *k,
               int nx, int ny) {

/*
  k = thermal conductivity.
  dt < h*h / (4*k);
  if k = 1, dt < (h*h)/4
*/
   const double h2x = 1.0;
   const double h2y = 1.0;
   const double dt  = 0.0625;

   double *pl, *pw, *pn, *ps;
   double *npl, *npw, *npn, *nps;

   double *pk    = &k [1 + nx];
   double *avant = &m [1 + nx];
   double *apres = &nm[1 + nx];

   pw = avant-1 ; pl = avant+1;
   pn = avant-nx; ps = avant+nx;

   // remove borders from calculation
   //nx; ny;

   // go for it
   for (int y = 1; y < ny-1; y++) {
      for (int x = 1; x < nx-1; x++) {

         *apres = *avant +
                      *pk * dt * (
                        ((*pl + *pw - 2*(*avant)) / h2x)
                        +
                        ((*pn + *ps - 2*(*avant)) / h2y)
                      );

        ++pl; ++pw; ++pn; ++ps; ++pk; ++apres;++avant;
      }

      ++pl; ++pw; ++pn; ++ps; ++pk; ++apres;++avant;
      ++pl; ++pw; ++pn; ++ps; ++pk; ++apres;++avant;
   }
}



//-----------------------------------------------------------------------------
/*!
  Computes one step of the heat equation.
  Adapted for parallelization
*/
void StepHeat2 (double *m, double *nm, double *k,
               int nx, int ny, int of) {

/*
  k = thermal conductivity.
  dt < h*h / (4*k);
  if k = 1, dt < (h*h)/4
*/
   const double h2x = 1.0;
   const double h2y = 1.0;
//   const double dt  = 0.03125;
   const double dt  = 0.0625;
//   const double dt  = 0.25;


   double *pl, *pw, *pn, *ps;
   double *npl, *npw, *npn, *nps;

   double *pk    = &k [1 + nx + of];
   double *avant = &m [1 + nx + of];
   double *apres = &nm[1 + nx + of];

   pw = avant-1 ; pl = avant+1;
   pn = avant-nx; ps = avant+nx;

   // remove borders from calculation
   //nx; ny;

   // go for it
   int y,x;
   for (y = 1; y < ny-1; y+=1) {
      for (x = 1; x < nx-1; x+=2) {

         *apres = *avant +
                      *pk * dt * (
                        ((*pl + *pw - 2*(*avant)) / h2x)
                        +
                        ((*pn + *ps - 2*(*avant)) / h2y)
                      );
        ++pl; ++pw; ++pn; ++ps; ++pk; ++apres;++avant;
        ++pl; ++pw; ++pn; ++ps; ++pk; ++apres;++avant;
      }

      ++pl; ++pw; ++pn; ++ps; ++pk; ++apres;++avant;

      if (x == (nx-1)) {
         ++pl; ++pw; ++pn; ++ps; ++pk; ++apres;++avant;
      }
   }
}


//-----------------------------------------------------------------------------
/*!
  Computes several steps of the heat equation. Results in x and xnew
  (with one deltaT of difference)
*/
void CalcHeat (double *m, double *nm, double *k, double *s,
               int nx, int ny, int nsteps) {

   if (NULL == m ) M_FullStop("Null m");
   if (NULL == nm) M_FullStop("Null nm");
   if (NULL == k ) M_FullStop("Null k");

   int i;

   // Round to an even number; this may add an extra calculation step;
   //   this does not really interfere with the results, but
   //   makes the algorithm much simpler.
   if (nsteps % 2)
      ++nsteps;

   int len = nx*ny;


//#define noparallel 1
#ifdef noparallel
   while (nsteps--) {

      HeatSources (m, s, len);
      StepHeat    (m, nm, k, nx, ny);

      nsteps--;
      HeatSources (nm, s, len);
      StepHeat    (nm, m, k, nx, ny);
   }

#else

   std::thread t1, t2;
   while (nsteps--) {

      HeatSources (m, s , len);
      t1 = std::thread(StepHeat2, m, nm, k, nx, ny, 0);
      t2 = std::thread(StepHeat2, m, nm, k, nx, ny, 1);

      t1.join();
      t2.join();

      nsteps--;
      HeatSources (nm, s, len);
      t1 = std::thread(StepHeat2, nm, m, k, nx, ny, 0);
      t2 = std::thread(StepHeat2, nm, m, k, nx, ny, 1);

      t1.join();
      t2.join();
   }
#endif
}



//-----------------------------------------------------------------------------
/*!
  Drives the heat calculation routine.
*/
int LoopCalcHeat ( const Layer &im1
                  ,const Layer &imhot
                  ,Layer &imk
                  ,Layer &imresult
                  ,int   nmaxloops
                  ,int   expected
                  ,int   &dif
                 ) {

   int nloops;


   //-----------------------
   // Auxiliary arrays.
   // m1 and m2 are for results (one time step difference);
   //  we assume that the time step DeltaT is small enough so that
   //  getting m1 or m2 as result does not matter.
   //  k stands for the heat coefficient and is set arbitrarily to 1.
   //-----------------------
   int nn = im1.width * im1.height;

   Layer aux1, auxr, auxx, hot2;

   aux1 .copy (&im1);
   auxx .copy (&im1);

   int nold = aux1.countGreater (URBANTHRESHOLD);

   int delta;
   int nnow;

   //-----------------------
   // gradually heats up
   //-----------------------
   for (nloops = 0; nloops < nmaxloops; nloops++) {
   
      // aux1 is both input and result
      CalcHeat (aux1.data, auxx.data, imk.data, imhot.data, im1.width, im1.height, 4);
      
      nnow  = aux1.countGreater (URBANTHRESHOLD);

      delta = nnow - nold;
       
      nold  = nnow;

      if (delta < 1) break;

      if (nnow >= expected) break;

   }


   imresult.copy (&aux1);


   return nloops;
}



//-----------------------------------------------------------------------------
/*!
  Enlarge spots in an image, using.. the heat equation!

  \param m     image array
  \param nx,ny image size
  \param t     number of time steps
  \return m    the input parameter is changed

*/
void Enlarge (double *m, int nx, int ny, int nsteps) {

   if (NULL == m ) M_FullStop("Null m");

   int nn = nx * ny;

   double *m2, *k, *s;

   m2 = new double[nn]; if (NULL == m2) M_FullStop ("No memory for m2\n");
   k  = new double[nn]; if (NULL == k ) M_FullStop ("No memory for k\n");
   s  = new double[nn]; if (NULL == s ) M_FullStop ("No memory for s\n");

   for (int x = 0; x < nn; x++) {
      s[x]  = m[x];
      k [x] = 1;
      m2[x] = 0;
   }

   int i;

   // Round to an even number; this may add an extra calculation step;
   //   this does not really interfere with the results, but
   //   makes the algorithm much simpler.
   if (nsteps % 2)
      ++nsteps;

   while (nsteps--) {

      HeatSources (m, s , nn);
      StepHeat    (m, m2, k, nx, ny);

      nsteps--;
      HeatSources (m2, s, nn);
      StepHeat    (m2, m, k, nx, ny);

   }
}





void Enlarge2 (double *m, int nx, int ny, int nsteps) {

   if (NULL == m ) M_FullStop("Null m");

   int nn = nx * ny;

   double *m2, *k, *s;

   m2 = new double[nn]; if (NULL == m2) M_FullStop ("No memory for m2\n");
   k  = new double[nn]; if (NULL == k ) M_FullStop ("No memory for k\n");
   s  = new double[nn]; if (NULL == s ) M_FullStop ("No memory for s\n");

   for (int x = 0; x < nn; x++) {
      s[x]  = m[x];
      k [x] = 1;
      m2[x] = 0;
   }

   int i;

   // Round to an even number; this may add an extra calculation step;
   //   this does not really interfere with the results, but
   //   makes the algorithm much simpler.
   if (nsteps % 2)
      ++nsteps;

   std::thread t1, t2;
   while (nsteps--) {

      HeatSources (m, s , nn);
      t1 = std::thread(StepHeat2, m, m2, k, nx, ny, 0);
      t2 = std::thread(StepHeat2, m, m2, k, nx, ny, 1);

      t1.join();
      t2.join();

      nsteps--;
      HeatSources (m2, s, nn);
      t1 = std::thread(StepHeat2, m2, m, k, nx, ny, 0);
      t2 = std::thread(StepHeat2, m2, m, k, nx, ny, 1);

      t1.join();
      t2.join();

   }
}



void SprayUrban (Layer &imgR, const Layer &imgPossible, 
                 double temperature, unsigned int q, unsigned int area) {


   std::mt19937 g (time (NULL));
   std::uniform_real_distribution<double> ud (0.1, 0.91);
   std::uniform_real_distribution<double> ud2(0.0, 1.0);
   //std::normal_distribution<double>       nd (0.0, 0.25);

   int nn = imgR.width * imgR.height;

   area = sqrt (area);

   int x, x2, y, y2;

   double v;

   int idx;

   while (q--) {

      int k = 100;

      while (k--) {

         idx  = (int) (nn * ud2(g));
 
         if (imgPossible.data[idx] > v)
            break;
      }

      if (!k) continue;


      x = idx % imgR.width;
      y = idx / imgR.width;

      for (x2 = 0; (x2 < area) && (x < imgR.width); x2++, x++)
         for (y2 = 0; (y2 < area) && ((y + area) < imgR.height); y2++) {

            idx = x+((y+y2)*imgR.width);

            imgR       .data[idx] = temperature;
            imgPossible.data[idx] = 0.0;
         }
    }
}


void SprayUrban_trial (Layer &imgR, const Layer &imgPossible, 
                 double temperature, unsigned int q, unsigned int area) {


   std::mt19937 g (time (NULL));
   std::uniform_real_distribution<double> ud (0.1, 0.91);
   std::uniform_real_distribution<double> ud2(0.0, 1.0);
   //std::normal_distribution<double>       nd (0.0, 0.25);

   int nn = imgR.width * imgR.height;

   area = sqrt (area);

   int x, x2, y, y2;

   double v;

   int direcao = imgR.width - 1;

   while (q-- > 0) {

      v = ud(g);

      // select a random direction to scan the image
      if (ud2(g) >= 0.5) direcao = -direcao;

      int xx  = (int) (nn * ud2(g));
      int idx = (xx + direcao + nn) % nn;

      while (idx != xx) {

         idx = (idx + direcao + nn) % nn;

         if (imgPossible.data[idx] > v)
            break;
      }

      // could not find point, advance q
      if (idx == xx)
         continue;

      x = idx % imgR.width;
      y = idx / imgR.width;

      for (x2 = 0; (x2 < area) && (x < imgR.width); x2++, x++)
         for (y2 = 0; (y2 < area) && ((y + area) < imgR.height); y2++) {

            idx = x+((y+y2)*imgR.width);

            imgR       .data[idx] = temperature;
            imgPossible.data[idx] = 0.0;
         }
    }
}
