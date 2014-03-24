// -*- coding: utf-8 -*-
// cptctx.hh
#pragma once

#include <algorithm>
#include <tuple>

#include "util/range/adapt.hh"
#include "util/range/algo.hh"

#include "log.hh"
#include "data.hh"
#include "component.hh"

// # each cpt type C
// # represents state that is shared across Control s
// # hence, instantiate once. no need for special ctor.
// # def __init__(self, Cs):
// struct Entity;
struct ModelGen;
struct ControlHandle;
using std::tuple;
using std::get;

namespace cpt
{
//
// Aggregate Specs
struct Specs
{
  using Members = vector<pair<string, string> >;
  // using Aliases = map<string, string>;
protected:
  string _name;
  Members _members;
  // Aliases _aliases;
  
public:
  Specs(string n, Members ms): 
    _name(n), _members(ms)
  {}

public:
  string name() const {return _name;}
  Members members() const {return _members;}
  // bool operator<(const CptClass& that) {return _id }
  // Data create()

  // create a control handle for components, if supported
  ControlHandle* control_handle() const {return {};}
};

// attempt at a well-behaved path key
struct CptPath
{
  using Parts = vector<string>;
  Parts _parts;
  
  CptPath() = default;
  CptPath(const Parts& ps): _parts(ps) {}
  CptPath(const string& s);
  
  // using RangeT = decltype(util::range::join(std::declval<Parts>(), char{}));
  // RangeT range() const { return util::range::join(_parts, '.'); }

  string str() const {return util::io::join(_parts, '.');}
    
  bool operator==(const CptPath& that) const {return _parts == that._parts;}
  bool operator<(const CptPath& that) const
  {
    auto ps = that._parts;
    auto it = begin(ps);
      
    for (auto&& e: _parts) {
      if (it == end(ps)) return false;
      if (e < *it) return true;
      if (e > *it) return false;
      ++it;
    }
    return false;
  }
  
  friend CptPath operator+(const CptPath& a, const CptPath& b)
  {
    auto ap = a._parts;
    if (ap.empty()) return b;
    if (b._parts.empty()) return a;
    
    ap.insert(end(ap), begin(b._parts), end(b._parts));
    return CptPath(ap);
  }
};

template <class Ch, class Tr>
std::basic_ostream<Ch,Tr>&
operator<<(std::basic_ostream<Ch,Tr>& out, const CptPath& p)
{ return out << '\'' << p.str() << '\''; }

using CptDefn =   tuple<Data, string>;

// struct CptType: public Data
// {
//   // Data _init;
//   // CptType(Data i): _init(i) {}
//   // Data init() const { return _init; }
//   using Data::Data;
//   // CptType(const CptType&) = default;
//   Data init() const { return *this; }
// };
using CptType = Data;

//! The "context" of a set of well-defined components
//
// distinction between dtypes, component types, and component data:
// dtype: represents data part of a basic component
// basic component: 1-tuple mapping to single dtype : (name, dtype)
// component type: tuple(of basic components) : (name, [basic])
struct Ctx
{
  using DtypeNames = map<string, dtype::T>;
  using CptTypes = map<Compt_id, CptType>;
  using CptNames = map<Compt_id, CptPath>;
  using CptAggrs = map<string, vector<Compt_id>>;
  
protected:
  static const dtype::Dtypes _dtypes;
  const DtypeNames _dtype_names;

  CptTypes _cpt_types;
  CptNames _cpt_names;
  CptAggrs _cpt_aggrs;
  // set<Operation> _operations;

public:
  Ctx(ModelGen& mg, vector<Specs> cts);

  bool parse_data(string s, CptDefn& out) const;  
  
  Data create_type(Compt_id c) const
  {
    auto cpt = get_type(c);
    if (!cpt) THROW_(Not_found_T<Compt_id>, c);
    return *cpt;
  }
  
  const CptType* get_type(Compt_id cpid) const
  {
    auto itc = _cpt_types.find(cpid);
    if (itc != end(_cpt_types)) {
      return &(itc->second);
    }
    // return nullptr;
    THROW_(Not_found_T<Compt_id>, cpid);
  }
  
  vector<Compt_id> find_type(string p) const
  {
    // CptPath cp(p);
    auto it = _cpt_aggrs.find(p);
    return it != end(_cpt_aggrs) ? it->second : THROW_(Not_found, p);
  }

  CptPath get_name(Compt_id cpid) const
  {
    // auto rg = filter(
    //   util::range::view(_cpt_names),
    //   [=](const CptNames::value_type& e){return cpid == e.second;}
    // );
    // ASSERT_EQ_(1, count(rg), "component name is not unique");
    // return rg.front().first;
    auto it = _cpt_names.find(cpid);
    return it != end(_cpt_names)? it->second : THROW_T_(Not_found, cpid);
  }
};

template <class Ch,class Tr>
std::basic_ostream<Ch,Tr>& 
operator<<(std::basic_ostream<Ch,Tr>& out, const Specs& cpc)
{
  util::print_to(out, "Specs(\"", cpc.name(), "\",", cpc.members(), ")");
  return out;
}
}
