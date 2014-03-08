// -*- coding: utf-8 -*-
// component.cc

#include "component.hh"

namespace dtype
{

Tag tag_of(T dt)
{
  const m<T, Tag> map = {
    // boolean
    {dtype::ty_bool, Tag::boolean},
    // special tag for now, but maybe 
    // {dtype::ty_bool, Tag::realm},

    // numeric types
    {dtype::ty_int, Tag::number},
    {dtype::ty_float, Tag::number},

    // a pair (i, r, scale) Realm defn and index, scale
    {dtype::ty_rdisc, Tag::realm},
    {dtype::ty_rcont, Tag::realm},

    // --not sure how to impl. or if necessary
    {dtype::ty_str, Tag::N},

    // {dtype::ty_tuple, ...},
    // {dtype::ty_expr, sizeof(unsigned*)},

    // {dtype::ty_N, Tag::N}
  };
  return map.at(dt);
}

unsigned size(T dt) 
{
  const m<T, unsigned> map = {
    {dtype::ty_bool, sizeof(data::Bool)},
    {dtype::ty_int, sizeof(data::Int)},
    {dtype::ty_float, sizeof(data::Float)},
    {dtype::ty_rdisc, sizeof(data::RlmDisc)},
    {dtype::ty_rcont, sizeof(data::RlmCont)},
    {dtype::ty_str, sizeof(data::Str)},
    {dtype::ty_N, 0}
  };
  return map.at(dt);
}

T from_string(string s)
{
  const m<string, T> map = {
    {"bool", dtype::ty_bool},
    {"int", dtype::ty_int},
    {"float", dtype::ty_float},
    {"rdisc", dtype::ty_rdisc},
    {"rcont", dtype::ty_rcont},
    {"str", dtype::ty_str},
    {"()", dtype::ty_N}
  };
  auto it = map.find(s);
  return it != end(map)? it->second : (
    LOG_(warning)(__func__, ": no dtype for \"", s, '"'),
    ty_N);
}

string to_string(T dt)
{
  const m<T, string> map = {
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
} // namespace dtype

string Data::to_string() const
{
  string ret;
    
  // `val_' for value
#define DATA_TO_STRING_CASE_(dt_, Type_, str_expr_) \
  case dtype::dt_: {                                \
    data::Type_ val_ = get<data::Type_>();          \
    ret = ("{" #Type_ ", ") + (str_expr_) + '}';    \
  } break
    
  switch (_dtype)
  {
    DATA_TO_STRING_CASE_(ty_bool, Bool, util::concat(val_.value));
    DATA_TO_STRING_CASE_(ty_int, Int, util::concat(val_.value));
    DATA_TO_STRING_CASE_(ty_float, Float, util::concat(val_.value));
    DATA_TO_STRING_CASE_(ty_rdisc, RlmDisc, util::concat(
                           '(', val_.index,
                           ',', val_.realm_max, 
                           ',', val_.offset, 
                           ')'));
    DATA_TO_STRING_CASE_(ty_rcont, RlmCont, util::concat(
                           '(', val_.index,
                           ',', val_.realm_max, 
                           ',', val_.offset, 
                           ')'));
    DATA_TO_STRING_CASE_(ty_str, Str, string("\"\""));
#undef DATA_TO_STRING_CASE_
      
  default:
    THROW_(Invalid, __PRETTY_FUNCTION__);
  }
  return ret;
}
