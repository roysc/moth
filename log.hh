// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include <iostream>
#include "util/io.hh"

struct Log;

struct Log
{
  template <class... Ts>
  void operator()(Ts&&... args) 
  {
    util::println(std::forward<Ts>(args)...);
  }
};

Log& get_global_log() 
{
  static Log s_log;
  return s_log;
}
Log& get_thread_log() 
{
  thread_local Log th_log;
  return th_log;
}

#define LOG_(level) (get_global_log())
#define LOG_THR_(level) (get_thread_log())
