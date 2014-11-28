#ifndef UTILS__H
#define UTILS__H

#include <string>

namespace utils
{
   std::string getcwd();
   std::string fmt(const char * format, ...);

   enum LogType { LOG_INFO, LOG_WARN, LOG_ERROR };
   void log(LogType type, const char * format, ...);

   std::string read_file(std::string const & filename);

   template <typename T>
   T lerp(T const & a, T const & b, float t) {
      return a * (1.f - t) + b * t;
   }
}

#endif // #ifndef UTILS__H