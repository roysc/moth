// -*- coding: utf-8 -*-
// component.cc

#include <algorithm>
#include "component.hh"
#include "data.hh"

namespace dtype
{
namespace
{
Tag tag_of(dtype::T dt)
{
  auto it = _meta_info.find(dt);
  return it != end(_meta_info)? it->second.tag : THROW_(Not_found_T<dtype::T>, dt);
}
}

unsigned size(dtype::T dt) 
{
  auto it = _meta_info.find(dt);
  return it != end(_meta_info)? it->second.size : THROW_(Not_found_T<dtype::T>, dt);
}

string to_string(T dt)
{
  const map<T, string> map = {
    {dtype::ty_bool, "bool"},
    {dtype::ty_int, "int"},
    {dtype::ty_float, "float"},
    {dtype::ty_rdisc, "rdisc"},
    {dtype::ty_rcont, "rcont"},
    {dtype::ty_str, "str"},
    {dtype::ty_N, "()"}
  };
  return map.at(dt);
}

bool operator%(Tag s, Tag t)
{
  switch (t) {
  case Tag::N: return false;
  case Tag::any: return true;
  case Tag::number:
    return s == Tag::number
      // s realm can be converted to a real number
      || s == Tag::realm;
  case Tag::boolean:
    return s == Tag::boolean
      || s == Tag::number;
      // || s == Tag::realm;
  case Tag::realm:
    // actually realms only make sense if they are dealt with separately
    return s == Tag::realm;
  default:
    LOG_(debug)(__PRETTY_FUNCTION__, ": default case; t = ", t);
    return false;
  }
}

bool operator%(dtype::T s, Tag t) {return tag_of(s) % t;};

Dtypes _meta_info {
  // boolean
  {dtype::ty_bool, {Tag::boolean, sizeof(data::Bool)}},
  // numeric types
  {dtype::ty_int, {Tag::number, sizeof(data::Int)}},
  {dtype::ty_float, {Tag::number, sizeof(data::Float)}},
  // a pair (i, r, scale) Realm defn and index, scale
  {dtype::ty_rdisc, {Tag::realm, sizeof(data::RlmDisc)}},
  {dtype::ty_rcont, {Tag::realm, sizeof(data::RlmCont)}},
  // --not sure how to impl. or if necessary
  {dtype::ty_str, {Tag::N, sizeof(data::Str)}},
  {dtype::ty_N, {Tag::N, 0}}
  // {dtype::ty_tuple, ...},
  // {dtype::ty_expr, sizeof(unsigned*)},
};

DtypeNames _meta_names = {
  {"bool", dtype::ty_bool},
  {"int", dtype::ty_int},
  {"real", dtype::ty_float},
  {"realm(int)", dtype::ty_rdisc},
  {"realm(real)", dtype::ty_rcont},
  {"str", dtype::ty_str},
  {"nil", dtype::ty_N}
};
} // namespace dtype

