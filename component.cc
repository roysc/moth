// -*- coding: utf-8 -*-
// component.cc

#include <algorithm>
#include "component.hh"
#include "data.hh"

namespace dtype
{
namespace
{
Tag tag_of(T dt)
{
  auto it = _meta_info.find(dt);
  return it != end(_meta_info)? it->second.tag : THROW_(Not_found_T<dtype::T>, dt);
}
}

unsigned size(T dt) 
{
  auto it = _meta_info.find(dt);
  return it != end(_meta_info)? it->second.size : THROW_(Not_found_T<dtype::T>, dt);
}

string to_string(T dt)
{
  auto it = _meta_names.find(dt);
  return it != end(_meta_names)? it->second : THROW_(Not_found_T<dtype::T>, dt);
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

Dtypes _meta_info = {
  // boolean
  {dtype::ty_bool, {Tag::boolean, sizeof(data::Bool)}},
  // numeric types
  {dtype::ty_int, {Tag::number, sizeof(data::Int)}},
  {dtype::ty_float, {Tag::number, sizeof(data::Real)}},
  // a pair (i, r, scale) Realm defn and index, scale
  {dtype::ty_rdisc, {Tag::realm, sizeof(data::Int_rlm)}},
  {dtype::ty_rcont, {Tag::realm, sizeof(data::Real_rlm)}},
  {dtype::ty_ref, {Tag::N, sizeof(data::Ref)}},
  // --not sure how to impl. or if necessary
  {dtype::ty_str, {Tag::N, sizeof(data::Str)}},
  {dtype::ty_N, {Tag::N, 0}}
  // {dtype::ty_tuple, ...},
  // {dtype::ty_expr, sizeof(unsigned*)},
};

DtypeNames _meta_names = {
  {dtype::ty_bool, "bool"},
  {dtype::ty_int, "int"},
  {dtype::ty_float, "real"},
  {dtype::ty_rdisc, "realm(int)"},
  {dtype::ty_rcont, "realm(real)"},
  {dtype::ty_ref, "ref"},
  {dtype::ty_str, "str"},
  {dtype::ty_N, "nil"}
};
} // namespace dtype

