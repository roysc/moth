// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include <functional>
#include <iostream>
#include "util/io.hh"

// 
#define LOG_TO_(chan_, log_) (log_)

// static & thread macros
#define LOG_(chan_) LOG_TO_(chan_, ::logging::get_global_log())
// #define LOG_THR_(chan_) (log::get_thread_log()) // thr_?

// scope a log by "pushing" to (global, or specific) log
#define LOG_PUSH_TO_(logpush_, logtop_)                 \
  ::logging::LogScoped logpush_(logtop_); (logtop_)

#define LOG_PUSH_(logpush_)                         \
  LOG_PUSH_TO_(logpush_, ::logging::get_global_log())

inline
namespace logging
{
using std::string;
using std::ostream;

struct Log
{
protected:
  ostream& _out;
  unsigned _refct;
  const string _base_prelude = "  ";
  // const string _suffix = "\n";

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
    auto ct = ++_refct;
    while (ct--) rv += _base_prelude; 
    return rv;
  }
  // void refplus() {++_refct;}
};

// struct LogThr: public Log {};

// struct Chan: public Log {};

struct LogScoped: public Log
{
  string _prelude;
  LogScoped(Log& l):
    Log(l.stream()), _prelude(l.prelude())
  {}

  template <class... Ts>
  void operator()(Ts&&... args) 
  {
    Log::operator()(_prelude, std::forward<Ts>(args)...);
  }

};

Log& get_global_log();
Log& get_thread_log();
}
