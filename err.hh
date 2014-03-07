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
           w_,                                  \
           ERR_THROWFILE_ARG_,                  \
           ERR_THROWLINE_ARG_,                  \
           ERR_THROWFUNC_ARG_                   \
         ))                                     \
  
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
  // keep these around for later
  string _file; int _line; string _func;
  WrapDiagnostics(string w, string fl, int l, string fn):
    Err(util::concat("[", fl, ":", l, "] (", fn, "): ", w)),
    _file(fl), _line(l), _func(fn) 
  {}
};

// runtime
struct Runtime: public std::runtime_error
{using runtime_error::runtime_error;};

struct Invalid: public Runtime {using Runtime::Runtime;};
struct Not_found: public Runtime {using Runtime::Runtime;};

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
