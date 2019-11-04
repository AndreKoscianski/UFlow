/*




*/
#include "uflow.h"
#include "layer.h"
#include "config_handler.h"

#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
#include <string>


using namespace std;


void Usage () {
   std::cout << "\n"
                "add im1 im2 imResult\n"
                "sub im1 im2 imResult\n"
                "Matthews imRef imSim\n"
                "errImg imRef imSim imResult\n"
                "countGreater imRef 0.123\n"
                "invert im1 imResult\n";
   exit (0);
}


Layer im1, im2, ima, ima2;


//----------------------------------------------
int main (int argc, char **argv) {


   int x,y,nn,nx,ny,tp,tn,fp,fn;
   double q;


   if (argc < 2)
      Usage ();

   else if (std::string("sub") == argv[1]) {
      im1.load ((char *)argv[2]); im1.image2double (.5);
      im2.load ((char *)argv[3]); im2.image2double (.5);
      im1.subtract (&im2);
      im1.double2image (.5);
      im1.save ((char*) argv[4]);
   }


   else if (std::string("add") == argv[1]) {
      im1.load ((char *)argv[2]); im1.image2double ();
      im2.load ((char *)argv[3]); im2.image2double ();
      im1.add  (&im2);
      im1.double2tone (255,255,255);
      im1.save ((char*) argv[4]);
   }

   else if (std::string("Matthews") == argv[1]) {
      im1.load ((char *)argv[2]); im1.image2double ();
      im2.load ((char *)argv[3]); im2.image2double ();

      q = im1.metricMatthews   (im2
                               , tp, tn
                               , fp, fn);

      std::cout << "\nMatthews = " << q 
                << "\ntp tn fp fn " << tp << " " << tn << " " << fp << " " << fn << "\n";

   }


   else if (std::string("errImg") == argv[1]) {


       im1.load ((char *)argv[2]); im1.image2double ();
       im2.load ((char *)argv[3]); im2.image2double ();

	   // get rid of shades
	   im1.double2image (0.5);
	   im1.image2double (0.5);

	   // get rid of shades
	   im2.double2image (0.5);
	   im2.image2double (0.5);

	   // False Positive
	   ima.copy        (&im1);      // copy data
	   ima.subtract    (&im2);    // subtract according to '0.5 rule'
	   ima.double2tone (0,0,255); // image = f(double), paint in red

	   // False Negative
	   ima2.copy         (&im2);
	   ima2.subtract     (&im1);	   
	   ima2.double2tone  (255,0,0); // paint in red

 	   ima.copy_colored_pixels (&ima2);
	   ima.save ((char *)argv[4]);

	}

   else if (std::string("countGreater") == argv[1]) {
      im1.load ((char *)argv[2]);
      im1.image2double ();

      std::string s;
      s = std::string (argv[3]);

//      std::cout << "\nRead image " << argv[2];
//      std::cout << "\nParameter " << argv[3];

      std::cout << "\nCount = "
                << im1.countGreater (std::stod(s))
                << "\n";
   }

   else if (std::string("invert") == argv[1]) {
      im1.load ((char *)argv[2]);
      im1.image2double ();
      im1.invertBW     ();
      im1.double2tone  (255,255,255);
      im1.save ((char *)argv[3]);
   }

   else {
   	   Usage ();
   }


   std::cout << "Done.\n";

   return 0;
}
