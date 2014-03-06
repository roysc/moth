// -*- coding: utf-8 -*-
// log.cc

#include <thread>
#include "util/io.hh"

#include "log.hh"
#include "typedefs.hh"

inline
namespace logging
{
Log& get_global_log() 
{
  static uptr<Log> s_log(new Log("global", std::cout));
  assert(s_log && "global Log was deleted");
  return *s_log;
}

// Log& get_thread_log() 
// {
//   auto tid = std::this_thread::get_id();
//   thread_local uptr<Log> th_log(
//     new Log(util::concat("thread(", tid, ')'), std::cout)
//   );
//   assert(th_log && "thread-local Log was deleted");
//   return *th_log;
// }

}
