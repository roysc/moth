// -*- coding: utf-8 -*-
// log.hh
#pragma once

#include <functional>
#include <iostream>
#include <cassert>
#include "util/std/type_traits"

#include "util/io.hh"
#include "util/demangle.hh"

#define LOG_USE_FILE_INFO 0

// 
#if LOG_USE_FILE_INF0 > 0
#define LOG_TO_(chan_, log_) ((log_).file(__FILE__).line(__LINE__))
#else
#define LOG_TO_(chan_, log_) (log_)
#endif

// static & thread macros
#define LOG_(chan_) LOG_TO_(chan_, ::logging::get_global_log())
// #define LOG_THR_(chan_) (log::get_thread_log()) // thr_?

// scope a log by "pushing" to (global, or specific) log
#define LOG_PUSH_TO_(logpush_, logtop_)             \
  ::logging::Log logpush_(#logpush_, &(logtop_)); LOG_TO_(info, logpush_)

#define LOG_PUSH_(logpush_)                         \
  LOG_PUSH_TO_(logpush_, ::logging::get_global_log())

#define LOG_SHOW_TO_(x_, log_) LOG_TO_(debug, log_)("(" #x_ ") = ", (x_))
#define LOG_SHOW_(x_) LOG_(debug)("(" #x_ ") = ", (x_))

// log, eval; like
//  x = LOG_EVAL_(z + 1); LOG_SHOW_(x);
//  => "(z + 1) = 4", "(x) = 4"
// for generic lambda captures
#ifdef USE_STDCPP14
#define LOG_EVALN_(x_, ...) (([res_(x_)] {                             \
        LOG_(debug)("(eval): (" #x_ ") = ", res_, "; ", __VA_ARGS__);  \
        return res_;                                                   \
      })()                                                             \
  )
#define LOG_EVAL_(x_) (([res_(x_)] {                                   \
        LOG_(debug)("(eval): (" #x_ ") = ", res_);                     \
        return res_;                                                   \
      })()                                                             \
  )

#define LOG_TYPE_(x_) (([tnm_(::util::type_name<decltype(x_)>())] {    \
        LOG_(debug)("(type): (" #x_ ") : ", tnm_);                     \
      })()                                                             \
  )

#else
#define LOG_EVAL_(x_) (x_)
#define LOG_TYPE_(x_) (x_)
#endif

namespace logging
{
using std::string;
using std::ostream;

const string _base_prelude = "  ";

struct Log
{
protected:
  string _name;
  ostream& _out;
  unsigned _indent = 0;
  // "parent" log
  Log* _base;
  // diagnostics
  string _file;
  unsigned _line = 0;
  // options
  // truncate long [file:num] sections in prelude. TODO
  unsigned _truncate_file;
  // string _suffix = "\n";

public:
  Log(string nm, ostream& o):
    _name(nm), _out(o), _indent{}, _base{} {}
  Log(string nm, Log* l):
    _name(nm), _out(l->_out), _indent(++l->_indent), _base(l) {}
  ~Log() 
  {
    if (!_base) return;
    assert(_base->_indent > 0);
    --_base->_indent;
  }

  string name() const {return _name;}
  ostream& stream() {return _out;}
  Log& file(string f) {_file = f; return *this;}
  string file() {return _file;}
  Log& line(unsigned l) {_line = l; return *this;}
  unsigned line() {return _line;}
  
  // return indent depth for any stacked logs, and diagnostics
  void prelude()
  {
#if LOG_USE_FILE_INFO > 0
    _out << "[" << _file << ":" << _line << "] ";
#endif
    for (auto ct = _indent; ct; --ct) 
      _out << _base_prelude;
  }
  
  template <class... Ts>
  Log& operator()(Ts&&... args) 
  {
    prelude();
    util::print_to(_out, std::forward<Ts>(args)..., '\n');
    return *this;
  }
};

// struct Chan: public Log {};

Log& get_global_log();
Log& get_thread_log();
}

// needed for ambiguity of nullptr
namespace std
{
template <class Ch, class Tr>
std::basic_ostream<Ch, Tr>&
operator<<(std::basic_ostream<Ch, Tr>& out, const std::nullptr_t)
{return out << "<null>";}
}
