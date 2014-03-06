// -*- coding: utf-8 -*-
// component.hh
#pragma once

#include "log.hh"
#include "basic.hh"

// the idea is that a component is only accessed through Compt_addr keys
// using Compt_addr = pair<Entity_ptr, Compt_id>;

// Kind of like a "a trait of a (specific) entity"
struct Data;
using Data_ptr = Data*;

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

enum class Tag: unsigned
{ boolean, number, realm, expr, any, N };
}

namespace data
{
template <dtype::T DT> 
struct DT_label {static const dtype::T dt = DT;};

// Layout of data objects
struct Bool {int32_t value;};
// struct Bool: DT_label<dtype::ty_bool> {int32_t value;};
struct Int {int32_t value;};
struct Float {float value;};
struct RlmDisc {uint32_t index; uint32_t realm_max; uint32_t offset;};
struct RlmCont {float index; float realm_max; float offset;};
struct Str {int _;};
}

namespace dtype 
{
Tag tag_of(T dt);
unsigned size(T dt);
T from_string(string s);
string to_string(T dt);
}

// Tagged union-ish type
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
    assert((_dtype == (dtype::dt_)) && ("Data::get(" #Type_ "): wrong dtype")); \
    dat_out_ = *static_cast<const data::Type_*>(raw());                 \
  }                                                                     \
  void set(data::Type_ dat_in_)                                         \
  {                                                                     \
    assert((_dtype == (dtype::dt_)) && ("Data::set(" #Type_ "): wrong dtype")); \
    *static_cast<data::Type_*>(const_cast<void*>(raw())) = dat_in_;     \
  }
  
  // per-type getters
  DATA_DEFINE_GET_SET_(Bool, ty_bool)
  DATA_DEFINE_GET_SET_(Int, ty_int)
  DATA_DEFINE_GET_SET_(Float, ty_float)
  DATA_DEFINE_GET_SET_(RlmDisc, ty_rdisc)
  DATA_DEFINE_GET_SET_(RlmCont, ty_rcont)
  DATA_DEFINE_GET_SET_(Str, ty_str)
  
#undef DATA_DEFINE_GET_SET_
};

