#include "log.hh"

inline
namespace logging
{
const string Log::s_prelude = "  ";

Log& get_global_log() 
{
  static Log s_log(std::cout);
  return s_log;
}
Log& get_thread_log() 
{
  thread_local Log th_log(std::cout);
  return th_log;
}
}
