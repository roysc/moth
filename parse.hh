// -*- coding: utf-8 -*-
// parse.hh
#pragma once

#include <vector>
#include <string>
#include <functional>

#include <boost/spirit/include/qi.hpp>

#include "data.hh"

inline
namespace parse
{
// using std::ref;
// using std::bind;

// using std::vector;
// using std::string;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

// using ascii::char_;
using qi::lexeme;
namespace ql = qi::labels;

// using ascii::alnum;
const qi::rule<std::string::iterator, std::string(), ascii::space_type>
// r_sym = +alnum;
r_sym = +(ascii::char_("a-zA-Z0-9_"));

template <class It>
using Rule = qi::rule<It, void(), ascii::space_type>;

using RulePath = qi::rule<
  std::string::iterator, std::vector<std::string>(), ascii::space_type
  >;
inline
bool parse_cptpath(std::string in, std::vector<std::string>& out)
{
  auto set = [&](std::vector<std::string> v){out = v;};
  RulePath r_path = (r_sym % '.') [set];
  // r_path = qi::no_skip[(r_sym % '.') [set]];
  
  auto f = begin(in), l = end(in);
  auto r = phrase_parse(f, l, r_path, ascii::space);

  if (f != l) return false;
  return r;
}

}
