// -*- coding: utf-8 -*-
// parse.hh
#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>

namespace qi = boost::spirit::qi;

template <class R>
bool parse_ref(R rg, Data& out)
{
  DTYPE_CHECK_(dtype::Tag::ref, out.dtype());

}

template <class R, class N>
bool parse_realm(R rg, Data& out)
{
  DTYPE_CHECK_(dtype::Tag::realm, out.dtype());

  using boost::phoenix::ref;
  // using boost::
  
  
}
