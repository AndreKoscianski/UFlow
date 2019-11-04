#pragma once

#include <cstdlib>
#include <iostream>
#include <fstream>
#include <map>
#include <string>


/* -----------------------------------------------

key = value

arquivo = 1.png

valor = 1.1324

outrakey = outrovalor

# Vamos ver se funciona esta bodega!
arquivos {
 ./ueba la la/file1.png
 ./ueba la la/file2.png
 ./ueba la la/file3.png
}

// Très bien!
finalmente = YES
----------------------------------------------- */


std::map<std::string,std::string> Gmapa;


void read_config_file (const char *filename) {

  std::ifstream thefile;

  thefile.open (filename);


  if (!thefile.is_open()) {
    std::cout << "\nCannot open file\n <" << filename << ">\n";
    exit (0);
  }

  std::string parm_name, parm_equal , parm_val;

  while (thefile >> parm_name ) {

     if ((parm_name[0] == '#') || (parm_name == "#") || (parm_name == "//")) {
        std::getline (thefile, parm_val);
        continue;
     }

     thefile >> parm_equal;

     if (parm_equal == "=") {
       thefile >> parm_val;
       Gmapa.insert (std::pair<std::string,std::string>(parm_name,parm_val));
       //std::cout << parm_name << " = " << parm_val << "\n";
     }

     if (parm_equal == "{") {
        std::getline (thefile, parm_val);
        int k = 0;
        do {
           std::getline (thefile, parm_val);
           if (parm_val == "}")
              break;

           Gmapa.insert (std::pair<std::string,std::string>
                         (parm_name+std::to_string(k++), parm_val)
                        );

           //std::cout << parm_val << std::endl;
        } while (parm_val != "}");
     }
  }

  //thefile << "Writing this to a file.\n";

  thefile.close();

  // list content:
//  for (std::map<std::string,std::string>::iterator it = Gmapa.begin();
//       it != Gmapa.end();
//       ++it)
//    std::cout << it->first << " => " << it->second << '\n';
}


const char *config_gets (const char *key) {

   auto it = Gmapa.find (key);

   if (it == Gmapa.end()) return "";

   return (const char *) ((it->second).c_str());

}


bool config_getb (const char *key) {

   auto it = Gmapa.find (key);

   if (it == Gmapa.end()) return "";

   return ((it->second) == "YES");

}

double config_getd (const char *key) {

   auto it = Gmapa.find (key);

   if (it == Gmapa.end()) return 0;

   return strtod ((const char *) ((it->second).c_str()), NULL);
}



/*

void read_config_file_2 (const char *filename) {

  std::ifstream thefile;

  thefile.open (filename);

  std::string parm_name, parm_equal , parm_val;

   std::string line;
   while (std::getline (thefile, line)) {

       std::istringstream iss(line);

       iss >> parm_name;

       if (parm_name == "") continue;

       iss >> parm_equal;

*/
