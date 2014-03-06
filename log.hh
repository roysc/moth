// -*- coding: utf-8 -*-
// log.hh
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
  ::logging::Log logpush_(&(logtop_)); (logtop_)

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
  Log* _base{};
  unsigned _indent;
  const string _base_prelude = "  ";
  // const string _suffix = "\n";

public:
  Log(ostream& o): _out(o) {}
  Log(Log* base): _out(base->_out), _base(base), _indent(base->_indent + 1) {}

  ostream& stream() {return _out;}
  // return indent depth for any stacked logs
  string prelude()
  {
    string rv;
    for (auto ct = _indent; ct; --ct) 
      rv += _base_prelude; 
    return rv;
  }

  template <class... Ts>
  void operator()(Ts&&... args) {util::print_to(_out, prelude(), std::forward<Ts>(args)..., '\n');}
};

// struct Chan: public Log {};

Log& get_global_log();
Log& get_thread_log();
}
