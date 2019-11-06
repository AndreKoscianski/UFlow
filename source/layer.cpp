#include "uflow.h"
#include "lodepng.h"
#include "layer.h"

#include <math.h>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <algorithm>

Layer::Layer () {width = height = 0; data = NULL;};

//Layer::Layer (const Layer &l) { copy (&l);};

Layer::~Layer () {

//   printf ("\nI am %p \n", this);

   image.clear(); 

   if (NULL != data) {
      delete[] data; data = NULL;
   }
};


//---------------------------------------------------
/*!
   Loads a PNG file to memory
     \param filename path
     \return true if ok, abort program otherwise
*/
bool Layer::load (const char *filename) {

   width = height = 0;

   image.clear(); 

   if (NULL != data) {
      delete[] data; data = NULL;
   }

   unsigned error = lodepng::decode (image, width, height, filename);
/*
   if (error) {
      char *s = (char*) lodepng_error_text (error);
      std::cout << "\n" << filename << "\n";
      M_FullStop(s);
   }
*/

//   printf ("\nI am %p %s \n", this, filename);

   if (0 == (width + height)) return false;

   return true;
}


//---------------------------------------------------
/*!
   Saves a PNG file to file
     \param filename path
     \return true if ok, abort program otherwise
*/
bool Layer::save (const char *filename) {

   if (!*filename)
      M_FullStop ("Tried to save a layer with a NULL filname!");


   unsigned error = lodepng::encode(filename, image, width, height);

   if (error) {
      char *s = (char*) lodepng_error_text (error);
      char str[400];
      sprintf (str,"%s\nFilename %s", s, filename);
      M_FullStop(str);
   }

   return true;
}

//---------------------------------------------------
/*!
   Copy object, including malloc'd memory
     \param origin
     \return copy
*/
void Layer::copy (const Layer *origin) {

   if (NULL != data) {
      delete[] data;
      data = NULL;
   }

   image.clear();

   image  = origin->image;
   width  = origin->width;
   height = origin->height;

   if (NULL != origin->data) {
      data = new double[width * height];
      memcpy (data, origin->data, width*height * sizeof (double));
   }
}

//---------------------------------------------------
/*!
   Copy pixels, if not black
     \param origin
     \return copy
*/
void Layer::copy_colored_pixels (Layer *origin) {

   if (NULL == origin)           M_FullStop("Shouldnt be null image here.");
   if (width  != origin->width)  M_FullStop("width should be ok.");
   if (height != origin->height) M_FullStop("height should be ok.");

   for (int k = 0; k < image.size(); k+=4) {

      unsigned char r = origin->image[k    ];
      unsigned char g = origin->image[k + 1];
      unsigned char b = origin->image[k + 2];

      if ((r+g+b) > 1) {
         image[k    ] = origin->image[k    ];
         image[k + 1] = origin->image[k + 1];
         image[k + 2] = origin->image[k + 2];
      }
   }
}

//---------------------------------------------------
/*!
   Reads one pixel from image
     \param x,y coordinates
     \return pixel RGB
*/
int Layer::get (int x, int y) {

   if (!(width+height))
      return 0;

   int k = (4 * (width*y + x));

   return    image[k    ] << 24
           + image[k + 1] << 16
           + image[k + 2];
}


//---------------------------------------------------
/*!
   Sets one pixel from image
     \param x,y coordinates
*/
void Layer::set (int x, int y, int n) {

   if (!(width+height))
      M_FullStop("Shouldnt be null image here.");

   int k = (4 * (width*y + x));

   image[k    ] = (char) n;//((n & 0xFF0000) >> 16);
   image[k + 1] = (char) n;//((n & 0x00FF00) >>  8);
   image[k + 2] = (char) n;//((n & 0x0000FF)      );
   image[k + 3] = 0xff;
};


//---------------------------------------------------
/*!
   Converts pixels to double [0..1]
     \return pointer to internal vector
*/
double *Layer::image2double (double threashold) {

   if (NULL != data) delete[] data;
   data = new double[width*height];

   int k = 0;
   while (k < image.size()) {
      if (threashold < (double) image[k    ] / 765. +
                       (double) image[k + 1] / 765. +
                       (double) image[k + 2] / 765.
         )
         data[k/4] = 1;
      else
         data[k/4] = 0;

      k += 4;
   }

   return data;
};



//---------------------------------------------------
/*!
   Converts pixels to double [0..1]
     \return pointer to internal vector
*/
double *Layer::image2double () {

   if (NULL != data) delete[] data;
   data = new double[width*height];

   int k = 0;
   while (k < image.size()) {
      data[k/4] = (double) image[k    ] / 765. +
                  (double) image[k + 1] / 765. +
                  (double) image[k + 2] / 765.;

      k += 4;
   }

   return data;
};







void Layer::find_range (double &min, double &max) {

   int k;
   int n = image.size() / 4;
   min = 1e6;
   max = -min;

   for (k = 0; k < n; k++) {
      if (min > data[k]) min = data[k];
      if (max < data[k]) max = data[k];
   }
}

//---------------------------------------------------
/*!
   Converts from array 'data' to 'image'
*/
//double lmax,lmin;
void Layer::normalize () {

   int k;
   int n = image.size() / 4;
   double min = 1e6;
   double max = -min;

   for (k = 0; k < n; k++) {
      if (min > data[k]) min = data[k];
      if (max < data[k]) max = data[k];
   }

   double range = max-min;
//lmax=max;
//lmin=min;
   k = 0;
   while (k < n) {

      data[k] = (data[k] - min) / range;

      ++k;
   }
};



// https://stackoverflow.com/questions/7706339/grayscale-to-red-green-blue-matlab-jet-color-scale

void GetColour(double v,double vmin,double vmax,
               double &r, double &g, double &b) {

   double dv;

   r = g = b = 1.0;

   if (v < vmin)
      v = vmin;
   if (v > vmax)
      v = vmax;
   dv = vmax - vmin;

   if (v < (vmin + 0.25 * dv)) {

      r = 0;
      g = 4 * (v - vmin) / dv;

   } else if (v < (vmin + 0.5 * dv)) {
      r = 0;
      b = 1 + 4 * (vmin + 0.25 * dv - v) / dv;
   } else if (v < (vmin + 0.75 * dv)) {
      r = 4 * (v - vmin - 0.5 * dv) / dv;
      b = 0;
   } else {
      g = 1 + 4 * (vmin + 0.75 * dv - v) / dv;
      b = 0;
   }
}



//---------------------------------------------------
/*!
   Converts from array 'data' to 'image',
     using a 'heat' color map
*/
void Layer::double2image () {

   int k;
   double min = 1e6;
   double max = -min;

   for (k = 0; k < (image.size()/4); k++) {
      if (min > data[k]) min = data[k];
      if (max < data[k]) max = data[k];
   }

   double range = max-min;

   k = 0;
   while (k < image.size()) {

      //unsigned char c = (unsigned char) (255. * data[k/4]);

      double q = data[k/4];// - min) / range) - 1;

      double r,g,b;

      GetColour (q, min, max, r, g, b);

      image[k    ] = (unsigned int)(r*255);
      image[k + 1] = (unsigned int)(g*255);
      image[k + 2] = (unsigned int)(b*255);
      image[k + 3] = 255;

      k += 4;
   }
};


//---------------------------------------------------
/*!
   Converts from array 'data' to 'image',
     monochromatic, obeying a given cut value.
*/
void Layer::double2image (double threashold) {

   int k = 0;
   while (k < image.size()) {

      //unsigned char c = (unsigned char) (255. * data[k/4]);

      unsigned char c = (threashold < data[k/4]) ? 255 : 0;

      image[k    ] = c ;
      image[k + 1] = c ;
      image[k + 2] = c ;
      image[k + 3] = 255;

      k += 4;
   }
};


//---------------------------------------------------
/*!
   Converts from array 'data' to 'image',
      in shades of the chosen color.
*/
void Layer::double2tone (unsigned char r
                        ,unsigned char g
                        ,unsigned char b) {

   int k = 0;
   while (k < image.size()) {

      double q = data[k/4];

      image[k    ] = (unsigned int) (r*q);
      image[k + 1] = (unsigned int) (g*q);
      image[k + 2] = (unsigned int) (b*q);
      image[k + 3] = 255;

      k += 4;
   }
};


//---------------------------------------------------
/*!
   Converts from external array 'source' to 'image'
*/
void Layer::double2image (double *source, double threashold) {

   int k = 0;
   while (k < image.size()) {

      data[k/4] = source[k/4];

      unsigned char c = (threashold < source[k/4])
                        ? 255
                        : 0;

      image[k    ] = c ;
      image[k + 1] = c ;
      image[k + 2] = c ;
      image[k + 3] = 255;

      k += 4;
   }
};


//---------------------------------------------------
/*!
   As the name says. Calculations using 'data' ()= double)
     \return true if ok, abort program otherwise
*/
void Layer::BWthreshold (double threshold) {

   if (NULL == data)    return;

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
      data[k] = (data[k] >= threshold) ? 1.0 : 0.0;

};


//---------------------------------------------------
/*!
   As the name says. Calculations using 'data' ()= double)
     \return true if ok, abort program otherwise
*/
void Layer::clamp (double min, double max) {

   if (NULL == data)    return;

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
      if (data[k] < min)
            data[k] = min;
      else if (data[k] > max)
            data[k] = max;
};




//---------------------------------------------------
/*!
   As the name says. Calculations using 'data' ()= double)
     \return true if ok, abort program otherwise
*/
void Layer::clamp_min (double min) {

   if (NULL == data)    return;

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
      if (data[k] < min)
            data[k] = 0;
};


//---------------------------------------------------
/*!
   As the name says. Calculations using 'data' ()= double)
   Note that this is a 'special' kind of subtraction,
     keeping information of pixels that are under a given threshold.
     \return true if ok, abort program otherwise
*/
void Layer::subtract (Layer *l) {

   if (l->width  != width ) return;
   if (l->height != height) return;
   if (NULL == data)    return;
   if (NULL == l->data) return;

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
      if ((data[k] > 0.5) && (((l->data)[k]) > 0.5))
            data[k] = 0;
};


//---------------------------------------------------
/*!
   As the name says. Calculations using 'data' ()= double)
     \return true if ok, abort program otherwise
*/
void Layer::Subtract (Layer *l) {

   if (l->width  != width ) return;
   if (l->height != height) return;
   if (NULL == data)    return;
   if (NULL == l->data) return;

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
//      if (data[k] > 0.1) data[k] -= ((l->data)[k]);
//            data[k] -= ((l->data)[k]);
      if ((((l->data)[k]) > 0.5))
            data[k] = 0;
};

//---------------------------------------------------
/*!
   Computes set union

*/
void Layer::set_union (Layer *l) {

   if (l->width  != width ) return;
   if (l->height != height) return;
   if (NULL == data)    return;
   if (NULL == l->data) return;

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
      if (((l->data)[k]) > 0.5)
         data[k] = ((l->data)[k]);
};



//---------------------------------------------------
/*!
   Computes per-element multiplication

*/
void Layer::multiply (Layer *l) {

   if (l->width  != width ) return;
   if (l->height != height) return;
   if (NULL == data)    return;
   if (NULL == l->data) return;

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
         data[k] *= ((l->data)[k]);
};

//---------------------------------------------------
/*!
   As the name says. Calculations using 'data' ()= double)
     \return true if ok, abort program otherwise
*/
void Layer::add (Layer *l) {

   if ((l->width  != width ) ||
       (l->height != height) ||
       (NULL == data)        ||
       (NULL == l->data))
      M_FullStop("Bad Layer sum");

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
      data[k] += ((l->data)[k]);
};



//---------------------------------------------------
/*!
   As the name says. Calculations using 'data' ()= double)
     \return true if ok, abort program otherwise
*/
void Layer::multiply (double v) {

   int k = (image.size() / 4) - 1;

   for (; k >= 0; k--)
      data[k] *= v;

  // double2image (URBANTHRESHOLD);
};




std::vector<int> Layer::cluster_count () {

   int i, j, n, area, npaintedpixels;

   std::vector<int> vc;

   double *aux;

   aux = new double[width * height];

   memcpy (aux, data, width*height * sizeof (double));

   n = 2;

   // WARNING.
   // If a city cluster touches the image boundary, we will run in trouble.
   // Because of this, put a frame around the image.
   // Sure, we're modifying input data. But there are 2 points to consider.
   //  1st, Be honest with me, who he hell would run this kind of simulation (urban expansion)
   //       using an image that does not have room for that process?!
   //       So, yes, let's modify the image here - input data is crap anyway. 
   //  2nd, During reverse simulation (shrinkage using inverted BW maps),
   //       we do need to get sure about black borders.
   for (i = 0; i < width; i++) {
      M_data2(aux,i,0)        = 0;
      M_data2(aux,i,height-1) = 0;
   }
   for (i = 0; i < height; i++) {
      M_data2(aux,0,i)       = 0;
      M_data2(aux,width-1,i) = 0;
   }


   for (i = 1; i < width-1; i++)
      for (j = 1; j < height-1; j++)
         if (1 == M_data2(aux,i,j)) {

            floodFill (aux, i, j, 1.0, (double) n, npaintedpixels);
            if (npaintedpixels > 0)
               vc.push_back (npaintedpixels);

            n++;
         }

//   memcpy (data, aux, width*height * sizeof (double));

   delete[] aux;

   std::sort (vc.begin(), vc.end());

   return vc;//n-2;
}


//---------------------------------------------------
/*!
   Receives two images: grow and original.<br>
   Grow contains only cells that were urbanized.<br>
   This routine finds points in 'original' that touch points<br>
   in 'grow'. These points are hot spots for growing.<br>
     \return true if ok, abort program otherwise
*/
void Layer::adjacencies (Layer *grow, Layer *origin) {

   // check if everything ok.
   if (grow->width  != origin->width ) return;
   if (grow->height != origin->height) return;
   if (NULL == grow->  data)           return;
   if (NULL == origin->data)           return;

   // This object will hold result; adjust size.
   width  = grow->width;
   height = grow->height;

   image.clear ();
   image.insert (image.begin(), 4*width*height, 0);

   image2double (.5);


   // Everything has been set.
   // Now, let's check what's going on in the neighbourhood.
   // The following array saves the C++ code the burden of
   //   computing indices [x-1][y-1], [x][y-1]...
   int neighbours[] = {
      -(signed)width-1, -(signed)width, -(signed)width+1, -1, +1,
       (signed)width-1, (signed)width, (signed)width+1
   };

   // Limit the loop by discarding the borders (variable n);
   int k = width + 1;
   int n = width * (height-1) - 2;

   // for every active grow point,
   //   for every one of its active neighbours, mark 1 in the map
   int j;
   for (; k < n; k++)
      if (0 != grow->data[k])
         for (j = 0; j < 8; j++)
            if (origin->data[k+neighbours[j]] > .5)
               data[k+neighbours[j]] = 1.0;

    //for (j = 1; j < 1000)
      // data[j] = 1.0;

   // And that's all for today, boys.
   // Beautiful!

};


Layer& Layer::operator = (const Layer &other) {

    if (this != &other)
        copy ((Layer *) &other);

    return *this;
}


void Layer::set_every_pixel_to (double v) {

   for (int k = 0; k < width*height; k++)
      data[k] = v;

   double2image (URBANTHRESHOLD);
}





void Layer::invertBW () {

   int k = 0;
   while (k < image.size()) {

      data[k/4] = (int) (1.0 - data[k/4]);

      k += 4;
   }
};



//---------------------------------------------------
/*!
   paints spaces by flooding them with colour.
     \param x,y initial position
     \param color a double (the filling value)
*/

void Layer::floodFill (double *dd, int x, int y, double antes, double depois,
                       int &npaintedpixels) {


   std::stack < std::pair <int,int> > q;
   std::pair<int,int> aux;


   if (M_data2(dd,x,y) != antes) return;

   npaintedpixels = 0;

   q.push (std::make_pair(x,y));

   while (!q.empty()) {

     aux = q.top ();

     q.pop ();

     M_data2(dd,aux.first,aux.second) = depois;

     ++npaintedpixels;

      if (antes ==   M_data2(dd,(aux.first-1),aux.second))
         q.push (std::make_pair((aux.first-1),aux.second));

      if (antes ==   M_data2(dd,(aux.first+1),aux.second))
         q.push (std::make_pair((aux.first+1),aux.second));

      if (antes ==   M_data2(dd,aux.first,(aux.second-1)))
         q.push (std::make_pair(aux.first,(aux.second-1)));

      if (antes ==   M_data2(dd,aux.first,(aux.second+1)))
         q.push (std::make_pair(aux.first,(aux.second+1)));
   }
}



//---------------------------------------------------
/*!
   Simple linear regression
     \param a,b (references) to Ax+B coefficients.
*/
void  Layer::linear_regression (double &a, double &b) {

   std::vector<double> x, y;

   int ix, iy;

   for (ix = 0; ix < width; ix++)
      for (iy = 0; iy < height; iy++) {
         if (M_data(ix,iy) > 0.5) {
            x.push_back((double) ix);
            y.push_back((double) iy);
         }
      }


      const auto n    = x.size();
      const auto s_x  = std::accumulate(x.begin(), x.end(), 0.0);
      const auto s_y  = std::accumulate(y.begin(), y.end(), 0.0);
      const auto s_xx = std::inner_product(x.begin(), x.end(), x.begin(), 0.0);
      const auto s_xy = std::inner_product(x.begin(), x.end(), y.begin(), 0.0);
      const auto aa   = (n * s_xy - s_x * s_y) / (n * s_xx - s_x * s_x);
      const auto bb   = (s_y / n) - (aa * (s_x / n));

      a = (double) aa;
      b = (double) bb;

}




//---------------------------------------------------
/*!
   Count pixels (data) with value > p
     \param p value for comparison.
     \return number of pixels
*/
int Layer::countGreater (double threashold) {

   int k = width * height;

   if (!k) return -1;

   int n = 0;

   while (k--)
      if (threashold < data[k])
         ++n;

   return n;
};



//---------------------------------------------------
/*!
   Count pixels (data) with value > p
     \param p value for comparison.
     \return number of pixels
*/
double Layer::metricLeeSallee (const Layer &l, double v) {

   if (l.width  != width ) return 0.0;
   if (l.height != height) return 0.0;
   if (NULL == data)       return 0.0;
   if (NULL == l.data)     return 0.0;

   int k = (image.size() / 4) - 1;

   int u = 0;
   int i = 0;

   for (; k >= 0; k--) {
      if ((data[k] > v) || ((l.data[k]) > v)) ++u;
      if ((data[k] > v) && ((l.data[k]) > v)) ++i;
   }
   return (double) i / (double) u;
};





//---------------------------------------------------
/*!
   Count false/true positive/negative results;<br>
 then returns Matthews correlation coefficient.
     \param r reference layer
     \param tp true positive
     \param tn true negative
     \param fp false positive
     \param fn false negative
     \return Matthews correlation coefficient.
*/
double Layer::metricMatthews (const Layer &ref
    , int &tp, int &tn
    , int &fp, int &fn
    ) {

   if (ref.width  != width ) return -2.0;
   if (ref.height != height) return -2.0;
   if (NULL == data)         return -2.0;
   if (NULL == ref.data)     return -2.0;

   int k = (image.size() / 4) - 1;

   tp = tn = fp = fn = 0;
/*
   for (; k >= 0; k--)
      if (ref.data[k] > 0.5) {
         if (data[k] > 0.5) ++tp; else ++fn;
      } else
         if (data[k] > 0.5) ++fp; else ++tn;
*/

   for (; k >= 0; k--)
      if (data[k] > 0.5) {
         if (ref.data[k] > 0.5) ++tp; else ++fp;
      } else
         if (ref.data[k] > 0.5) ++fn; else ++tn;


   return (((double)tp*tn)-(fp*fn)) /
          sqrt ((((double)tp+fp)*(tp+fn)*(tn+fp)*(tn+fn)));
};











void Layer::distance_transform (double stddev) {

   double *pdata = new double[width * height];
   memcpy (pdata, data, width*height * sizeof (double));


   int x, y, k;

   double d, ds, dm = -1;

   struct s_xy {int x,y; s_xy(int a, int b) {x=a;y=b;}};

   std::list<struct s_xy> L;

   for (y = 0; y < height; y++)
      for (x = 0; x < width; x++)
         if (M_data(x,y) ==  1)
            L.push_back (s_xy(x,y));          

   for (y = 1; y < height-1; y++) {
      for (x = 1; x < width-1; x++) {

         if (M_idx(pdata,x,y) == 0) {

            M_data(x,y) = d = 1e6;

            for (auto & it : L) {

               int x1 = it.x;
               int y1 = it.y;

               ds = sqrt ((x-x1)*(x-x1) + (y-y1)*(y-y1));

               if (ds < d)
                  d = ds;
            }
         }
         M_data(x,y) = d;

         if (dm < d) dm = d;
      }
   }

   L.clear();

   delete[] pdata;



   for (y = 1; y < height-1; y++)
      for (x = 1; x < width-1; x++) {

//either this
//       M_data(x,y) = (ds - M_data(x,y)) / ds;
//or this

         double v = stddev * (1.0 - ((ds - M_data(x,y)) / ds));
         M_data(x,y) = exp (- (v*v));

       }

   normalize ();
}


/*


#include "kdtreei.h"



// Example test:
//
//   Gaux.copy (&Groads);
//   Gaux.distance_transform2 (10.);
//   Gaux.Subtract (&Groads);
//   Gaux.double2tone (255,255,255);
//   Gaux.save ("DistanceTransform-kd.png");



void Layer::distance_transform2 (double stddev) {

   double *pdata = new double[width * height];
   memcpy (pdata, data, width*height * sizeof (double));


   int x, y;

   int xy_in[2], *p;
   double d, ds = -1;

   std::vector<int> L;

   for (y = 0; y < height; y++)
      for (x = 0; x < width; x++)
         if (M_data(x,y) ==  1) {
            L.push_back (x);
            L.push_back (y);
         }

   CKDTree<2> kt;
   kt.make_tree (L.data(), L.size() / 2);


   for (y = 1; y < height-1; y++) {
      for (x = 1; x < width-1; x++) {

         if (M_idx(pdata,x,y) == 0) {

            xy_in[0] = x;
            xy_in[1] = y;

            kt.nearestL (xy_in, &p, &d);

            M_data(x,y) = d;

            if (ds < d) ds = d;
         }
      }
   }

   L.clear();

   delete[] pdata;


//   find_range (d, ds);

   for (y = 1; y < height-1; y++)
      for (x = 1; x < width-1; x++) {
//either this
       M_data(x,y) = (ds - M_data(x,y)) / ds;
//or this
  //       double v = stddev * (1.0 - ((ds - M_data(x,y)) / ds));
  //       M_data(x,y) = exp (- (v*v));
       }

   normalize ();
}

*/