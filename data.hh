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

using std::tuple;

using Bool = tuple<bool>;
using Int = tuple<int32_t>;
using Int_rlm = tuple<int32_t, int32_t, int32_t>;
// using Int_rlm = arr<int32_t, 3>;
using Real = tuple<double>;
using Real_rlm = tuple<double, double, double>;
// using Real_rlm = arr<double, 3>;
using Str = tuple<string>;
using Ref = tuple<Compt_addr>;

template <class T> struct Id_ { using type = T; };
template <class T> using Id = Id_<T>;

template <dtype::T dtype = dtype::ty_N>
struct flag_type_
{
  using type = void;
};

template <class D>
constexpr dtype::T flag_type();

template <>
constexpr dtype::T flag_type<Bool>() {return dtype::ty_bool;}
template <>
constexpr dtype::T flag_type<Int>() {return dtype::ty_int;}
template <>
constexpr dtype::T flag_type<Real>() {return dtype::ty_float;}
template <>
constexpr dtype::T flag_type<Int_rlm>() {return dtype::ty_rdisc;}
template <>
constexpr dtype::T flag_type<Real_rlm>() {return dtype::ty_rcont;}
template <>
constexpr dtype::T flag_type<Str>() {return dtype::ty_str;}
template <>
constexpr dtype::T flag_type<Ref>() {return dtype::ty_ref;}

// struct Bool {
//   int32_t value;
//   static dtype::T flag_type() {return dtype::ty_bool;}
// };
// // struct Bool: DT_label<dtype::ty_bool> {int32_t value;};
// struct Int {
//   int32_t value;
//   static dtype::T flag_type() {return dtype::ty_int;}
// };
// struct Float {
//   float value;
//   static dtype::T flag_type() {return dtype::ty_float;}
// };
// struct RlmDisc {
//   int32_t index;
//   int32_t realm_max; int32_t offset;
//   static dtype::T flag_type() {return dtype::ty_rdisc;}};
// struct RlmCont {
//   double index;
//   double realm_max; double offset;
//   static dtype::T flag_type() {return dtype::ty_rcont;}};
// struct Str {
//   string value;
//   static dtype::T flag_type() {return dtype::ty_str;}
// };
// struct Ref {
//   Compt_addr address;
//   static dtype::T flag_type() {return dtype::ty_ref;}
// };
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
    Data ret(data::flag_type<D>());
    ret.set<D>(D{std::forward<Vs>(vs)...});
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
  
  template <class D>
  D get() const
  {
    const auto dt = data::flag_type<D>();    
    ASSERT_EQ_(
      _dtype, dt,
      "Data::get(", dtype::to_string(dt), "): wrong dtype"
    );
    return D(*static_cast<const D*>(raw()));
  }

  template <class D, size_t n>
  util::type_at<D, n> get_at() const
  {
    const auto dt = data::flag_type<D>();    
    ASSERT_EQ_(
      _dtype, dt,
      "Data::get(", dtype::to_string(dt), "): wrong dtype"
    );
    // util::type_at<D,n> ret = std::get<n>(get<D>());
    util::type_at<D,n> ret = std::get<n>(*static_cast<const D*>(raw()));
    return ret;
  }

  // template <class D, size_t n, class V>
  // void set_at(V&& v)
  // {
  //   const auto dt = data::flag_type<D>();
  //   ASSERT_EQ_(
  //     _dtype, dt,
  //     "Data::set_at(", dtype::to_string(dt), "): wrong dtype"
  //   );
  //   *reinterpret_cast<util::type_at<D,n>*>(&_bytes[0]) = v;
  // }

  template <class D, class... Ts>
  void set(Ts&&... vs)
  {
    const auto dt = data::flag_type<D>();
    ASSERT_EQ_(
      _dtype, dt,
      "Data::set(", dtype::to_string(dt), "): wrong dtype"
    );
    *reinterpret_cast<D*>(&_bytes[0]) = D{std::forward<Ts>(vs)...};
  }
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& 
  operator<<(std::basic_ostream<Ch,Tr>& out, const Data& e)
  {return out << e.to_string();}
}; // struct Data

// #define DATA_DEFINE_GET_SET_(Type_)                                     \
//   template <>                                                           \
//   inline data::Type_ Data::get<data::Type_>() const                     \
//   {                                                                     \
//     ASSERT_EQ_(                                                         \
//       _dtype, (data::flag_type<data::Type_>()),                         \
//       "Data::get(" #Type_ "): wrong dtype"                              \
//     );                                                                  \
//     return *static_cast<const data::Type_*>(raw());                     \
//   }                                                                     \
//   template <>                                                           \
//   inline void Data::set<data::Type_>(data::Type_ dat_in_)               \
//   {                                                                     \
//     ASSERT_EQ_(                                                         \
//       _dtype, (data::flag_type<data::Type_>()),                         \
//       "Data::set(" #Type_ "): wrong dtype"                              \
//     );                                                                  \
//     *reinterpret_cast<data::Type_*>(&_bytes[0]) = dat_in_;              \
//   }

// // per-type getters
// DATA_DEFINE_GET_SET_(Bool)
// DATA_DEFINE_GET_SET_(Int)
// DATA_DEFINE_GET_SET_(Real)
// DATA_DEFINE_GET_SET_(Int_rlm)
// DATA_DEFINE_GET_SET_(Real_rlm)
// DATA_DEFINE_GET_SET_(Str)
// DATA_DEFINE_GET_SET_(Ref)

// #undef DATA_DEFINE_GET_SET_
