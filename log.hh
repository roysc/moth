// -*- coding: utf-8 -*-
// log.hh
#pragma once

#include <functional>
#include <iostream>
#include <cassert>
#include "util/io.hh"

// 
#define LOG_TO_(chan_, log_) (log_)

// static & thread macros
#define LOG_(chan_) LOG_TO_(chan_, ::logging::get_global_log())
// #define LOG_THR_(chan_) (log::get_thread_log()) // thr_?

// scope a log by "pushing" to (global, or specific) log
#define LOG_PUSH_TO_(logpush_, logtop_)             \
  ::logging::Log logpush_(#logpush_, &(logtop_)); (logpush_)

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
  string _name;
  ostream& _out;
  unsigned _indent;
  Log* _base;
  const string _base_prelude = "  ";
  // const string _suffix = "\n";

public:
  Log(string nm, ostream& o):
    _name(nm), _out(o), _indent(0), _base(0) {}
  Log(string nm, Log* l):
    _name(nm), _out(l->_out),
    _indent(++l->_indent), _base(l) {}
  ~Log() 
  {
    if (!_base) return;
    assert(_base->_indent > 0);
    --_base->_indent;
  }

  string name() const {return _name;}
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
  void operator()(Ts&&... args) 
  {
    util::print_to(_out, prelude(), std::forward<Ts>(args)..., '\n');
  }
};

// struct Chan: public Log {};

Log& get_global_log();
Log& get_thread_log();
}
