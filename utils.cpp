#ifdef WIN32
#  include <windows.h>
#  include <direct.h>
#else  // LINUX, OSX
#  include <unistd.h>
#  include <limits.h> // for MAX_PATH
#endif

#include "utils.h"

#include <iostream>
#include <fstream>
#include <cstdarg>

namespace utils
{
   std::string getcwd() {
#ifdef WIN32
         char cwd[MAX_PATH]; // not thread safe!
         return ::_getcwd(cwd, MAX_PATH);
#else  // LINUX, OSX
         char cwd[PATH_MAX];
         return ::getcwd(cwd, PATH_MAX);
#endif
   }

   void log(LogType type, const char * format, ...) {
      va_list args;
      va_start(args, format);
      char buffer[1024];
#pragma warning( push )
#pragma warning( disable : 4996 )
      std::vsprintf(buffer, format, args);
#pragma warning( pop )
      va_end(args);

#ifdef WIN32
      ::OutputDebugString(buffer);
#endif

      if (type == LOG_ERROR)
         std::cerr << buffer;
      else 
         std::cout << buffer;
   }

   std::string read_file(std::string const & filename) {
      std::ifstream infile(filename);
      std::string tmpStr;
      std::string data;

      getline(infile, tmpStr);
      while (infile.good()) {
         data += tmpStr;
         data += '\n';

         getline(infile, tmpStr);
      }

      data += '\n';
      return data;
   }

}