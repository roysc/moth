// -*- coding: utf-8 -*-
// data.hh
#pragma once

#include "log.hh"
#include "basic.hh"

namespace data
{
// Layout of data objects
struct Bool {int32_t value;};
struct Int {int32_t value;};
struct Float {float value;};
struct RlmDisc {uint32_t index; uint32_t realm_max; uint32_t offset;};
struct RlmCont {uint32_t index; uint32_t realm_max; uint32_t offset;};
struct Str {int _;};
}

namespace dtype
{
// essentially, built-in types
enum T: unsigned 
{ 
  // indentation sucks
  ty_bool,                 // could be impl'd as discrete realm of 0|1
  ty_int,
    ty_float,
    // Realm-values (of a range)
    ty_rdisc, // discrete (int)
    ty_rcont, // continuous (float)
    // ?
    ty_str,   // string, for what?
    // ty_tuple, // tuple rather than child grouping?
    // ty_calc,  // compt. calculated on reference?..
    ty_N // the nil-type
    };

inline
unsigned size(T dt) 
{
  const m<T, unsigned> map = {
    // numeric types
    {dtype::ty_bool, sizeof(data::Bool)},
    {dtype::ty_int, sizeof(data::Int)},
    {dtype::ty_float, sizeof(data::Float)},
    // a pair (i, r, scale) Realm defn and index, scale
    {dtype::ty_rdisc, sizeof(data::RlmDisc)},
    {dtype::ty_rcont, sizeof(data::RlmCont)},
    // --not sure how to impl. or if necessary
    {dtype::ty_str, sizeof(data::Str)},
    // {dtype::ty_tuple, ...},
    // {dtype::ty_calc, sizeof(unsigned*)},
    {dtype::ty_N, 0}
  };
  return map.at(dt);
}

inline
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

inline
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
}

// Tagged union type
struct Data
{
protected:
  using Bytes = v<uint8_t>;
  // using Bytes = uint8_t*;
  const dtype::T _dtype;
  Bytes _bytes;
public:
  Data(dtype::T dt): _dtype(dt), _bytes(dtype::size(dt)) {}
  dtype::T dtype() const {return _dtype;}
  const void* raw() const
  {
    assert(!_bytes.empty() && "data not initialized");
    return static_cast<const void*>(&_bytes[0]);
  }
  
#define DATA_DEFINE_GET_SET_(Type_, dt_)                                \
  void get(data::Type_& dat_out_) const                                 \
  {                                                                     \
    assert((_dtype == (dtype::dt_)) && ("Data::set(" #Type_ "): wrong dtype")); \
    dat_out_ = *static_cast<const data::Type_*>(raw());                 \
  }                                                                     \
  void set(data::Type_ dat_in_)                                         \
  {                                                                     \
    assert((_dtype == (dtype::dt_)) && ("Data::set(" #Type_ "): wrong dtype")); \
    *static_cast<data::Type_*>(const_cast<void*>(raw())) = dat_in_;     \
  }
  
  // per-type getters
  DATA_DEFINE_GET_SET_(Int, ty_int)
  DATA_DEFINE_GET_SET_(Float, ty_float)
  DATA_DEFINE_GET_SET_(RlmDisc, ty_rdisc)
  DATA_DEFINE_GET_SET_(RlmCont, ty_rcont)
  DATA_DEFINE_GET_SET_(Str, ty_str)
  
#undef DATA_DEFINE_GET_SET_
};
