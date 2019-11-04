#ifndef __logfile_hpp
#define __logfile_hpp


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOGFILEMSGLEN 300

class tp_LogFile {

   private:
      char
         _path[256]
        ;

      int
         _size
        ,_lf
        ;

      FILE
         *_file
        ;

      int  shrink   (void);
      long filesize (void);

   public:

      char
         _msg[LOGFILEMSGLEN]
        ;

      tp_LogFile (char *);
      tp_LogFile (      )        {_path[0] = 0; _size = 5000;}

     ~tp_LogFile (void  )       {;}


      int  clear   (void    );
      int  clear   (char *  );

      void lf      (int f   )    {_lf   = f;};

      void setsize (int i   )    {_size = i; shrink ();};

      void setpath (char *  );


      void write   (void    );
      void write   (char    );
      void write   (int     );
      void write   (long int);
      void write   (float   );
      void write   (double  );
      void write   (char *  );

      void writelf (char    );
      void writelf (int     );
      void writelf (long int);
      void writelf (float   );
      void writelf (double  );
      void writelf (char *  );

      void dlmwrite (char, double *, int, int);
} ;


#endif
