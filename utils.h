#ifndef UTILS__H
#define UTILS__H

#include <string>

namespace utils
{
   std::string getcwd();

   enum LogType { LOG_INFO, LOG_ERROR };
   void log(LogType type, const char * format, ...);

   std::string read_file(std::string const & filename);
}

#endif // #ifndef UTILS__H