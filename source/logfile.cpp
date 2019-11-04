#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "logfile.hpp"


void tp_LogFile::write (char c) {

   sprintf (_msg, "%c", c);
   write   ();
}

void tp_LogFile::write (int i) {

   sprintf (_msg, "%i", i);
   write   ();
}

void tp_LogFile::write (long int i) {

   sprintf (_msg, "%li", i);
   write   ();
}

void tp_LogFile::write (float n) {

   sprintf (_msg, "%f", n);
   write   ();
}

void tp_LogFile::write (double n) {

   sprintf (_msg, "%lf", n);
   write   ();
}

void tp_LogFile::write (char *s) {

   _file = fopen (_path, "a+t");

   if (NULL == _file) return;

   fprintf (_file, "%s", s);

   fclose (_file);
}



void tp_LogFile::writelf (char c) {
   int aux = _lf; _lf = 1;
   write (c);
   _lf = aux;
   _lf = aux;
}

void tp_LogFile::writelf (int i) {
   int aux = _lf; _lf = 1;
   write   (i);
   _lf = aux;
}

void tp_LogFile::writelf (long int i) {
   int aux = _lf; _lf = 1;
   write   (i);
   _lf = aux;
}

void tp_LogFile::writelf (float n) {
   int aux = _lf; _lf = 1;
   write   (n);
   _lf = aux;
}

void tp_LogFile::writelf (double n) {
   int aux = _lf; _lf = 1;
   write   (n);
   _lf = aux;
}

void tp_LogFile::writelf (char *s) {

   _file = fopen (_path, "a+t");

   if (NULL == _file) return;

   fprintf (_file, "%s\n", s);

   fclose (_file);

}


long tp_LogFile::filesize (void) {

   long curpos, length;

   curpos = ftell(_file);

   fseek(_file, 0L, SEEK_END);
   length = ftell(_file);

   fseek(_file, curpos, SEEK_SET);
   return length;
}



int tp_LogFile::shrink (void) {

   long int
      size
     ;

   char
      *p
     ;


   if ((!_path) || (!_path[0]))
      return 0;

   if (NULL != _file)
      fclose (_file);

   _file = fopen (_path, "r+t");

   if (NULL == _file) {
      _file = fopen (_path, "wt");
      fclose (_file);
      return 1;
   }


   size = filesize ();

   if ((_size > 0) && (_size < size)) {

      fseek (_file, size - _size - 100, SEEK_SET);

      
      auto nada = fgets (_msg, 150, _file);
           nada = fgets (_msg, 150, _file);

      p = new char[_size + 100];

      if (NULL == p) {

         fclose (_file);

         _file = fopen (_path, "wt");
         fclose (_file);

         write ((char *) "NO MEMORY error in log");
         return 1;
      }

      size = (long) fread (p, 1, _size + 90, _file);

      fclose (_file);

      _file = fopen (_path, "wt");
      fwrite (p, size, 1, _file);

      delete [] p;


   }

   fclose (_file);


   return 1;

}


int tp_LogFile::clear (void) {

   if (! _path)
      return 0;

   return clear (_path);
}


int tp_LogFile::clear (char *s) {

   if ((!s) || (!s[0]))
      return 0;

   if (NULL == (_file = fopen (s, "w+t")))
      return 0;

   strcpy (_path, s);

   fclose (_file);

   return 1;
}



void tp_LogFile::write (void) {

   _file = fopen (_path, "a+t");

   if (NULL == _file) return;

   fprintf (_file, _lf ? "%s\n" : "%s" , _msg);

   fclose (_file);

   _msg[0] = 0;
}



void tp_LogFile::setpath (char *s) {

   strcpy (_path, s);
};


tp_LogFile::tp_LogFile (char *s) {

   _size = -1;

   setpath (s);

   _lf = 0;

   //shrink ();
}

void tp_LogFile::dlmwrite (char c, double *p, int ni, int nj) {

   int i, j, k;

   char aux[22];

   if (nj * 21 > LOGFILEMSGLEN)
      throw ("logfile::dlmwrite -> too long");

   _lf = 0;

   for (i = 0; i < ni; i++) {

      k = i * nj;

      sprintf (_msg, "%19.18f", p[k]);

      for (j = 1; j < nj; j++) {
         sprintf (aux, "%c%19.18f", c, p[k+j]);
         strcat  (_msg, aux);
      }
      strcat (_msg, "\n");
      write ();
   }
}


