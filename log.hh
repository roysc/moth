// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include <functional>
#include <iostream>
#include "util/io.hh"

inline
namespace logging
{
using std::string;
using std::ostream;

struct Log;

struct Log
{
protected:
  ostream& _out;
  unsigned _refct;
  static const string s_prelude;

public:
  Log(ostream& o): _out(o) {}
  template <class... Ts>
  void operator()(Ts&&... args) 
  {
    util::print_to(_out, std::forward<Ts>(args)..., '\n');
  }
  ostream& stream() {return _out;}
  // current prelude for any stacked logs
  string prelude()
  {
    string rv;
    auto ct = _refct++;
    while (ct--) rv += s_prelude; 
    return rv;
  }
  // void refplus() {++_refct;}
};
const string Log::s_prelude = "  ";

// struct LogThr: public Log {};

// struct Chan: public Log {};

struct LogScoped: public Log
{
  string _prelude;
  LogScoped(Log& l):
    Log(l.stream()), _prelude(l.prelude())
  {}
};

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

// 
#define LOG_TO_(chan_, log_) (log_)

// static & thread macros
#define LOG_(chan_) LOG_TO_(chan_, ::logging::get_global_log())
// #define LOG_THR_(chan_) (log::get_thread_log()) // thr_?

// scope a log by "pushing" to (global, or specific) log
#define LOG_PUSH_TO_(logpush_, logtop_)                 \
  ::logging::LogScoped logpush_(logtop_); (logpush_)

#define LOG_PUSH_(logpush_)                         \
  LOG_PUSH_TO_(logpush_, ::logging::get_global_log())
