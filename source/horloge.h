#include <chrono>
#include <typeinfo>

double Horloge () {

   static bool done    = false;
   static decltype(std::chrono::system_clock::now()) t_start, t_end;

   if (!done) {
      done = true;
      t_start = std::chrono::system_clock::now();
   } else {
      done  = false;
      t_end = std::chrono::system_clock::now();
      std::chrono::duration<double> d = (t_end - t_start);
      return d.count();
//      return std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start);
      //return (t_end - t_start);
   }

   return 0.;
}
