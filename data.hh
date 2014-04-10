// -*- coding: utf-8 -*-
// data.hh
#pragma once

#include "util/std/type_traits.hh"
#include "util/range/adapt.hh"
#include "util/range/algo.hh"
#include "util/traits.hh"
#include "util/mp.hh"
#include "basic.hh"
#include "component.hh"

namespace dtype
{
using T = unsigned;
}

struct Entity;
struct Data;

// fixme: clean up
struct Compt_addr: pair<Entity*, Compt_id >
{
  using pair::pair;
  Compt_addr(Compt_id c): Compt_addr{0, c} {}
  Data& operator()() const;
};
struct Compt_mult: Compt_addr
{
  using Compt_addr::Compt_addr;
  vector<Compt_id> third;
  Compt_mult(Entity* e, vector<Compt_id> cs):
    Compt_addr{0, cs.at(0)}// ,
  // third{util::transform<vector<Compt_id>>(util::slice(cs, 1))} {}
  {
    ASSERT_BINOP_(1, cs.size(), <=, "fixme");
    third.assign(begin(cs) + 1, end(cs));
  }
  Data& operator()(size_t ix) const;
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

template <class D>
constexpr dtype::T d_type();

template <> constexpr dtype::T d_type<Bool>() {return dtype::ty_bool;}
template <> constexpr dtype::T d_type<Int>() {return dtype::ty_int;}
template <> constexpr dtype::T d_type<Real>() {return dtype::ty_float;}
template <> constexpr dtype::T d_type<Int_rlm>() {return dtype::ty_rdisc;}
template <> constexpr dtype::T d_type<Real_rlm>() {return dtype::ty_rcont;}
template <> constexpr dtype::T d_type<Str>() {return dtype::ty_str;}
template <> constexpr dtype::T d_type<Ref>() {return dtype::ty_ref;}

//! Deduce a data Type
template <class... Ts>
struct compat_type_
{
  static constexpr bool num = sizeof...(Ts);
  using all_int = util::all_satisfy<std::is_integral, Ts...>;
  using all_float = util::all_satisfy<std::is_floating_point, Ts...>;
  using is_str = util::is_member<util::Head<Ts...>, std::string, const char*>;
  
  using type = util::Cond<
    util::Case<(all_int::value && num == 1), data::Int>,
    util::Case<(all_int::value && num == 3), data::Int_rlm>,
    util::Case<(all_float::value && num == 1), data::Real>,
    util::Case<(all_float::value && num == 3), data::Real_rlm>,
    util::Case<is_str::value, data::Str>// ,
    // util::Case<std::is_base_of<Compt_addr, Head<Ts...> >::value, data::Ref>
    // ...
    >;
};
template <class... Ts>
using compat_type = typename compat_type_<Ts...>::type;
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
  Data(const Data&) = default;
  // template <class D>
  // Data(D&& dv): Data(data::d_type<decay_t<D> >()) {}
    
  template <class D, class... Vs>
  static Data make(Vs&&... vs)
  {
    Data ret(data::d_type<D>());
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
    const auto dt = data::d_type<D>();    
    ASSERT_EQ_(
      _dtype, dt,
      "Data::get(", dtype::to_string(dt), "): wrong dtype"
    );
    return D(*static_cast<const D*>(raw()));
  }

  template <class D, size_t n>
  util::type_at<D, n> get_at() const
  {
    const auto dt = data::d_type<D>();    
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
  //   const auto dt = data::d_type<D>();
  //   ASSERT_EQ_(
  //     _dtype, dt,
  //     "Data::set_at(", dtype::to_string(dt), "): wrong dtype"
  //   );
  //   *reinterpret_cast<util::type_at<D,n>*>(&_bytes[0]) = v;
  // }

  template <class D, class... Ts>
  void set(Ts&&... vs)
  {
    const auto dt = data::d_type<D>();
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
};
