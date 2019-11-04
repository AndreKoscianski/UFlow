#include <string>


char *str_trick (const char *add, const char *str) {

  static char buf[1000]; 

  strcpy (buf, str);

  std::string str1 (str);

  std::size_t found = str1.find_last_of(".");

  if (std::string::npos == found)
     return buf;
  
  
  std::string str2 =   str1.substr(0,found) 
                     + std::string (add)
                     + str1.substr(found);

  strcpy (buf, str2.c_str());

  return buf;
}
