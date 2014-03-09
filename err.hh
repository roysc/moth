
// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include <stdexcept>
#include <string>

#include "typedefs.hh"
#include "util/io.hh"

#define USE_ERR_DIAGNOSTICS 1

#define ERR_THROWLINE_P 1
#define ERR_THROWFUNC_P 1
  
#ifdef ERR_THROWLINE_P
#define ERR_THROWFILE_ARG_ __FILE__
#define ERR_THROWLINE_ARG_ __LINE__
#else
#define ERR_THROWFILE_ARG_ ""
#define ERR_THROWLINE_ARG_ ""
#endif

#ifdef ERR_THROWFUNC_P
#define ERR_THROWFUNC_ARG_ __PRETTY_FUNCTION__
#else
#define ERR_THROWFUNC_ARG_ ""
#endif

// 
#define THROW_BASIC_(Type_, w_)                 \
  throw (err::Type_(w_))

#define THROW_DIAGS_(Type_, w_)                 \
  throw (err::WrapDiagnostics<err::Type_>(      \
           err::Type_{w_}                       \
         ).set_diagnostics(                     \
           ERR_THROWFILE_ARG_,                  \
           ERR_THROWLINE_ARG_,                  \
           ERR_THROWFUNC_ARG_                   \
         ))
  
// // throw with template arg
// #define THROW_BASIC_ARG_(Type_, w_, a_)         \
//   throw (err::WrapArg<err::Type_>(w_, a_))

#ifdef USE_ERR_DIAGNOSTICS
#define THROW_(Type_, w_) THROW_DIAGS_(Type_, w_)
#else
#define THROW_(Type_, w_) THROW_BASIC_(Type_, w_)
#endif

namespace err
{
// wrapper class including diagnostics
template <class Err>
struct WrapDiagnostics: public Err
{
  // to ensure validity of what() return value until destruction
  string _what;
  string _file; int _line; string _func;
  WrapDiagnostics(Err e):
    Err(e), _what(e.what()),
    _file{}, _line{}, _func{}
  {}
  WrapDiagnostics& set_diagnostics(string fl, int l, string fn)
  {
    _file = fl; _line = l; _func = fn;
    // set string
    _what.clear();
    if (!_file.empty())
      _what += util::concat("[", _file, _line? util::concat(":", _line) : "", "]");
    if (!_func.empty())
      _what += "(" + _func + "): ";
    _what += Err::what();
    return *this;
  }
  const char* what() const noexcept override {return _what.c_str();}
};

// runtime
struct Runtime: public std::runtime_error
{using runtime_error::runtime_error;};

struct Invalid: public Runtime {using Runtime::Runtime;};
template <class T>
struct Not_found_T: public Runtime
{
  T _value;
  // using Runtime::Runtime;
  Not_found_T(const T& x):
    Runtime(util::concat(x)),
    _value(x) {}
};
template <>
struct Not_found_T<void>: public Runtime {using Runtime::Runtime;};
using Not_found = Not_found_T<void>;

// Logic
struct Logic: public std::logic_error
{using logic_error::logic_error;};
struct Not_impl: public Logic {using Logic::Logic;};
struct Internal: public Logic {using Logic::Logic;};
struct Failed_assert: public Logic {using Logic::Logic;};

// template <class Err>
// struct WrapArg: public Err
// {
//   template <class T>
//   WrapArg(string w, const T& a):
//     Err(util::concat(w, ", arg: ", a)) 
//   {}
// };
}
