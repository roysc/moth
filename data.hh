// -*- coding: utf-8 -*-
// data.hh
#pragma once

#include "basic.hh"
#include "component.hh"

namespace dtype
{
using T = unsigned;
}

struct Entity;
struct Data;

struct Compt_addr: pair<Entity*, vector<Compt_id> >
{
  using pair::pair;
  Compt_addr(vector<Compt_id> cs): Compt_addr(0, cs) {}
  Compt_addr(Compt_id c): Compt_addr(0, {c}) {}
  Data* operator()(size_t ix) const;
};

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
  int32_t index;
  int32_t realm_max; int32_t offset;
  static dtype::T flag_type() {return dtype::ty_rdisc;}};
struct RlmCont {
  double index;
  double realm_max; double offset;
  static dtype::T flag_type() {return dtype::ty_rcont;}};
struct Str {
  string value;
  static dtype::T flag_type() {return dtype::ty_str;}
};
struct Ref {
  Compt_addr address;
  static dtype::T flag_type() {return dtype::ty_ref;}
};
}

// Tagged union-ish type
struct Data
{
protected:
  using Bytes = vector<uint8_t>;
  // using Bytes = uint8_t*;
  dtype::T _dtype;
  Bytes _bytes;

public:
  Data(dtype::T dt = dtype::ty_N): _dtype(dt), _bytes(dtype::size(dt)) {}
  // Data(dtype::T dt): _dtype(dt), _bytes(dtype::size(dt)) {}
  Data(const Data&) = default;
  // template <class D>
  // explicit Data(D&& dv): Data(typename std::decay<D>::type::flag_type()) {set(dv);}
    
  template <class D, class... Vs>
  static Data make(Vs&&... vs)
  {
    Data ret(D::flag_type());
    ret.set(D{vs...});
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

#define DATA_DEFINE_GET_SET_(Type_)                                     \
  template <>                                                           \
  inline data::Type_ Data::get<data::Type_>() const                     \
  {                                                                     \
    ASSERT_EQ_(                                                         \
      _dtype, (data::Type_::flag_type()),                               \
      "Data::get(" #Type_ "): wrong dtype"                              \
    );                                                                  \
    return *static_cast<const data::Type_*>(raw());                     \
  }                                                                     \
  template <>                                                           \
  inline void Data::set<data::Type_>(data::Type_ dat_in_)               \
  {                                                                     \
    ASSERT_EQ_(                                                         \
      _dtype, (data::Type_::flag_type()),                               \
      "Data::set(" #Type_ "): wrong dtype"                              \
    );                                                                  \
    *reinterpret_cast<data::Type_*>(&_bytes[0]) = dat_in_;              \
  }

// per-type getters
DATA_DEFINE_GET_SET_(Bool)
DATA_DEFINE_GET_SET_(Int)
DATA_DEFINE_GET_SET_(Float)
DATA_DEFINE_GET_SET_(RlmDisc)
DATA_DEFINE_GET_SET_(RlmCont)
DATA_DEFINE_GET_SET_(Str)
DATA_DEFINE_GET_SET_(Ref)

#undef DATA_DEFINE_GET_SET_
