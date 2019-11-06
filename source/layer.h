#pragma once

#include "uflow.h"
#include "lodepng.h"
#include <iostream>
#include <vector>
#include <string>
#include <stack>


#define M_data(x,y)     data[(x+(y*width))]
#define M_data2(p,x,y)  p[(x+((y)*width))]
#define M_idx(data,x,y) data[(x+(y*width))]

class Layer {

   public:

      std::vector<unsigned char> image; //the raw pixels
      double *data;

      unsigned int width, height;

      Layer () ;
//      Layer (const Layer &) ;
      ~Layer ();

      void copy (const Layer *);
      void copy_colored_pixels (Layer *);

      bool load  (const char *);
      bool save  (const char *);

      int  get (int, int);
      void set (int, int, int);

      int  countGreater (double);
      void find_range   (double &, double &);


      double *image2double (double);
      double *image2double ();
      void    normalize    ();
      void    double2image ();
      void    double2image (double);
      void    double2image (double *, double);
      void    double2tone  (unsigned char, unsigned char, unsigned char);

      void    distance_transform  (double);
      void    distance_transform2 (double);

      void    invertBW     ();

      void    BWthreshold  (double);
      void    clamp        (double, double);
      void    clamp_min    (double);
      void    Subtract     (Layer *);
      void    subtract     (Layer *);
      void    add          (Layer *);
      void    set_union    (Layer *);
      void    multiply     (Layer *);
      //void    set_difference(Layer *);
      void    multiply     (double );
      void    linear_regression (double &, double &);

      std::vector<int>  cluster_count ();

      Layer & operator = (const Layer &) ;

      void    adjacencies (Layer *, Layer *);

      void    set_every_pixel_to (double);

      void    floodFill (double *, int, int, double, double, int &) ;

      double  metricLeeSallee (const Layer &l, double v);
      double  metricMatthews   (const Layer &ref
                               , int &tp, int &tn
                               , int &fp, int &fn);

};
