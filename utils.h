#ifndef UTILS__H
#define UTILS__H

#include <string>

namespace utils
{
   std::string getcwd();

   enum LogType { LOG_INFO, LOG_ERROR };
   void log(LogType type, const char * format, ...);

}

#endif // #ifndef UTILS__H