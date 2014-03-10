// -*- coding: utf-8 -*-
// component.hh
#pragma once

#include "err.hh"
#include "basic.hh"
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

// subtyping relationships on Tags
// "any term of type S can be safely used in a context where a
//  term of type T is expected" -WP
bool operator%(Tag s, Tag t);
bool operator%(dtype::T s, Tag t);

template <class Ch,class Tr>
std::basic_ostream<Ch,Tr>& 
operator<<(std::basic_ostream<Ch,Tr>& out, Tag tag)
{
  static const m<Tag, string> tbl = {
    {Tag::boolean, "Tag::boolean"},
    {Tag::number, "Tag::number"},
    {Tag::realm, "Tag::realm"},
    {Tag::expr, "Tag::expr"},
    {Tag::any, "Tag::any"},
  };
  auto it = tbl.find(tag);
  if (it != end(tbl))
    out << it->second;
  else 
    THROW_(Not_found, __PRETTY_FUNCTION__);
  return out;
}

unsigned size(T dt);
T from_string(string s);
string to_string(T dt);
}

namespace data
{
// template <dtype::T DT> 
// struct DT_label {static const dtype::T dt = DT;};

// Layout of data objects
struct Bool {
  int32_t value;
  static dtype::T flag_type() {return dtype::ty_bool;}
};
// struct Bool: DT_label<dtype::ty_bool> {int32_t value;};
struct Int {
  int32_t value;
  static dtype::T flag_type() {return dtype::ty_int;}
};
struct Float {
  float value;
  static dtype::T flag_type() {return dtype::ty_float;}
};
struct RlmDisc {
  uint32_t index;
  uint32_t realm_max; uint32_t offset; static dtype
  ::T flag_type() {return dtype::ty_rdisc;}};
struct RlmCont {
  float index;
  float realm_max; float offset; static dtype
  ::T flag_type() {return dtype::ty_rcont;}};
struct Str {
  int _;
  static dtype::T flag_type() {return dtype::ty_str;}
};
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
  // template <class D>
  // explicit Data(D&& dv): Data(typename std::decay<D>::type::flag_type()) {set(dv);}
    
  template <class D, class V>
  static Data make(V&& v)
  {
    Data ret(D::flag_type());
    ret.set(D{v});
    return ret;
  }
  
  void set(const Data& that)
  {
    // LOG_(debug)(__PRETTY_FUNCTION__);
    ASSERT_EQ_(_dtype, that._dtype, "Data::set(Data): wrong dtype");
    _bytes = that._bytes;
  }
  // Data& operator=(const Data& that) {set(that); return *this;}
  
  dtype::T dtype() const {return _dtype;}
  const void* raw() const
  {
    assert(!_bytes.empty() && "data not initialized");
    return static_cast<const void*>(&_bytes[0]);
  }
  string to_string() const;
  
  template <class D> D get() const;
  template <class D> void set(D);
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& 
  operator<<(std::basic_ostream<Ch,Tr>& out, const Data& e)
  {return out << e.to_string();}
}; // struct Data

#define DATA_DEFINE_GET_SET_(Type_, dt_)                                \
  template <>                                                           \
  inline data::Type_ Data::get<data::Type_>() const                     \
  {                                                                     \
    ASSERT_EQ_(_dtype, (dtype::dt_),                                    \
                "Data::get(" #Type_ "): wrong dtype");                  \
    return *static_cast<const data::Type_*>(raw());                     \
  }                                                                     \
  template <>                                                           \
  inline void Data::set<data::Type_>(data::Type_ dat_in_)               \
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
