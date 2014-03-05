// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include <stdexcept>

namespace err
{
struct Not_found : public std::runtime_error
{
  string _what;
  Not_found(string s): 
    runtime_error("Not found: "+ (s.empty()?"":": "+s)) 
    // _what("Not found: "+ (s.empty()?"":": "+s)) 
  {}
};

struct Not_impl : public std::logic_error {};
struct Internal : public std::logic_error {};

}
