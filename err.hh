// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include <stdexcept>
// #include "util/demangle.hh"

namespace err
{
struct Not_found : public std::runtime_error
{
  Not_found(string s): 
    runtime_error("Not found"+ (s.empty()?"":": "+s)) {}
};

// template <class T>
// struct Not_found_ : public Not_found
// {
//   Not_found_(): Not_found(util::type_name<T>()) {}
// };

struct Invalid : public std::runtime_error {};

// Logic
struct Not_impl : public std::logic_error {};
struct Internal : public std::logic_error {};
}
