// -*- coding: utf-8 -*-
// parse.hh
#pragma once

#include <vector>
#include <string>
#include <functional>

#include <boost/spirit/include/qi.hpp>

inline
namespace parse
{
using std::ref;
using std::bind;

using std::vector;
using std::string;

namespace qi = boost::spirit::qi;
namespace ascii = boost::spirit::ascii;

// using ascii::char_;
using qi::lexeme;
namespace ql = qi::labels;

using ascii::alnum;
const qi::rule<std::string::iterator, std::string(), ascii::space_type>
r_sym = +alnum;

template <class It>
using Rule = qi::rule<It, void(), ascii::space_type>;

template <class It>
Rule<It> rule_realm(Data& out)
{
  using qi::int_;
  using qi::double_;
  using qi::lit;

  std::vector<double> vr;
  std::vector<int> vz;

  // "real [a, b]"
  auto vr_app = [&](double e){vr.push_back(e);};
  auto r_real = lit("real")
    >> '[' >> double_ [vr_app] >> ',' >> double_ [vr_app] >> ']';
  // "int [a, b]"
  auto vz_app = [&](int e){vz.push_back(e);};
  auto r_int = lit("int")
    >> '[' >> int_ [vz_app] >> ',' >> int_ [vz_app] >> ']';

  auto r_rlm =
    r_real [ ([&]{
        out = Data{dtype::ty_rcont};
        out.set(data::RlmCont{0, vr.at(0), vr.at(1)});
      }) ] |
    r_int  [ ([&]{
        out = Data{dtype::ty_rdisc};
        out.set(data::RlmDisc{0, vz.at(0), vz.at(1)});
      })];
  return r_rlm;
}

template <class It, class F>
Rule<It> rule_ref(Data& out, F&& fa)
{
  auto r_ref = qi::lit("ref") >> '(' >> r_sym[fa] >> ')';
  return r_ref;
}

template <class It>
Rule<It> rule_dtype(Data& out, std::string n, dtype::T dt)
{
  auto set_out = [&]{out = Data{dt};};
  return qi::lit(n) [set_out];
}

using RulePath = qi::rule<std::string::iterator, std::vector<std::string>(), ascii::space_type>;
inline
bool parse_cptpath(std::string in, std::vector<std::string>& out)
{
  auto set = [&](std::vector<std::string> v){out = v;};
  // auto r_path = qi::no_skip[(r_sym % '.')];
  RulePath r_path;
  r_path = (r_sym % '.') [set];
  // r_path %= (r_sym % '.');
  
  auto f = begin(in), l = end(in);
  auto r = phrase_parse(f, l, r_path, ascii::space);

  if (f != l) return false;
  return r;
}

}
