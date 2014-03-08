// -*- coding: utf-8 -*-
// component.hh
#pragma once

#include "basic.hh"
#include "log.hh"
#include "err.hh"
#include "assert.hh"

// the idea is that a component is only accessed through Compt_addr keys
// using Compt_addr = pair<Entity_ptr, Compt_id>;

// Kind of like a "a trait of a (specific) entity"
// a data instance, or Value
struct Data;
using Data_ptr = Data*;

using Compt_ptr = Data*;
using Compt_ptr_k = Data const*;

namespace dtype
{
// essentially, built-in types
enum T: unsigned 
{ 
  // emacs indentation sucks
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
// template <dtype::T DT> 
// struct DT_label {static const dtype::T dt = DT;};

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
  dtype::T _dtype;
  Bytes _bytes;

public:
  Data(dtype::T dt = dtype::ty_N): _dtype(dt), _bytes(dtype::size(dt)) {}
  // Data(dtype::T dt): _dtype(dt), _bytes(dtype::size(dt)) {}
  Data(const Data&) = default;

  void set(const Data& that)
  {
    ASSERT_EQ_(_dtype, that._dtype, "Data::set(Data): wrong dtype");
    _bytes = that._bytes;
  }

  dtype::T dtype() const {return _dtype;}
  const void* raw() const
  {
    assert(!_bytes.empty() && "data not initialized");
    return static_cast<const void*>(&_bytes[0]);
  }

  string to_string() const
  {
    string ret;

    // `val_' for value
#define DATA_TO_STRING_CASE_(dt_, Type_, str_expr_) \
    case dtype::dt_: {                              \
      data::Type_ val_; get(val_);                  \
      ret = ("{" #Type_ ", ") + (str_expr_) + '}';  \
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
    default:
      THROW_(Invalid, __PRETTY_FUNCTION__);
    }
    return ret;
  }

  // Data& operator=(Data const& that) 
  // {
  //   _dtype = that._dtype; 
  //   _bytes = that._bytes;
  //   return *this;
  // }
  
#define DATA_DEFINE_GET_SET_(Type_, dt_)                                \
  void get(data::Type_& dat_out_) const                                 \
  {                                                                     \
    ASSERT_EQ_(_dtype, (dtype::dt_),                                    \
                "Data::get(" #Type_ "): wrong dtype");                  \
    dat_out_ = *static_cast<const data::Type_*>(raw());                 \
  }                                                                     \
  void set(data::Type_ dat_in_)                                         \
  {                                                                     \
    ASSERT_EQ_(_dtype, (dtype::dt_),                                    \
               "Data::set(" #Type_ "): wrong dtype");                   \
    *reinterpret_cast<data::Type_*>(&_bytes[0]) = dat_in_;              \
  }
  
  // per-type getters
  DATA_DEFINE_GET_SET_(Bool, ty_bool)
  DATA_DEFINE_GET_SET_(Int, ty_int)
  DATA_DEFINE_GET_SET_(Float, ty_float)
  DATA_DEFINE_GET_SET_(RlmDisc, ty_rdisc)
  DATA_DEFINE_GET_SET_(RlmCont, ty_rcont)
  DATA_DEFINE_GET_SET_(Str, ty_str)
#undef DATA_DEFINE_GET_SET_
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& 
  operator<<(std::basic_ostream<Ch,Tr>& out, const Data& e)
  {return out << e.to_string();}
}; // struct Data

