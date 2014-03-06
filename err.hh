// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include "typedefs.hh"

#include <stdexcept>
// #include "util/io.hh"

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

// messy. meh.
#define THROW_BASIC_(Type_, a_)                                         \
  throw (err::Type_(a_))

#define THROW_(Type_, a_) THROW_BASIC_(Type_, a_)

namespace err
{
struct ErrDiag
{
//   string _file;
//   int _line;
//   string _func;
}
;
// // setters...
// ErrDiag& file(string f, ErrDiag& e) {e._file = f; return e;}
// ErrDiag& line(int ln, ErrDiag& e) {e._line = ln; return e;}
// ErrDiag& func(string f, ErrDiag& e) {e._func = f; return e;}

// runtime
struct Runtime: public std::runtime_error, public ErrDiag 
{using runtime_error::runtime_error;};

struct Invalid: public Runtime {using Runtime::Runtime;};
struct Not_found: public Runtime {using Runtime::Runtime;};
// {Not_found(string s): Runtime("Not found"+ (s.empty()?"":": "+s)) {}};

// template <class K>
// struct Not_found: public Runtime
// {
//   K _key;
//   Not_found(string s, ): Runtime("Not found"+ (s.empty()?"":": "+s)) {}
// };

// template <class T>
// struct Not_found_ : public Not_found
// {
//   Not_found_(): Not_found(util::type_name<T>()) {}
// };

// Logic
struct Logic: public std::logic_error, public ErrDiag {};
struct Not_impl: public Logic {using Logic::Logic;};
struct Internal: public Logic {using Logic::Logic;};
struct Failed_assert: public Logic {using Logic::Logic;};
}
