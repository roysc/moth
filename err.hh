// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include <stdexcept>

namespace err
{
using Error = std::runtime_error;

struct Not_found : public Error
{
  string _what;
  Not_found(string s): 
    Error("Not found: "+ (s.empty()?"":": "+s)) 
    // _what("Not found: "+ (s.empty()?"":": "+s)) 
  {}
};

struct Not_impl : public std::logic_error
{

};

}
