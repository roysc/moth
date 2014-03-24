// -*- coding: utf-8 -*-
// cptctx.hh
#pragma once

#include <algorithm>

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
  
  CptPath(const Parts& ps): _parts(ps) {}
  CptPath(const string& s);
    
  // using RangeT = decltype(util::range::join(std::declval<Parts>(), char{}));
  // RangeT range() const { return util::range::join(_parts, '.'); }

  string str() const {return util::io::join(_parts, "");}
    
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
};

template <class Ch, class Tr>
std::basic_ostream<Ch,Tr>&
operator<<(std::basic_ostream<Ch,Tr>& out, const CptPath& p)
{ return out << '"' << p.str() << '"'; }

//! The "context" of a set of well-defined components
//
// distinction between dtypes, component types, and component data:
// dtype: represents data part of a basic component
// basic component: 1-tuple mapping to single dtype : (name, dtype)
// component type: tuple(of basic components) : (name, [basic])
struct Ctx
{
  struct CptType
  {
    enum Kind { basic, named };
    Kind _kind;
    dtype::T _dtype;
    
    CptType(bool k, dtype::T dt):
      _kind(k? basic : named), _dtype(dt) {}
    
    bool is_basic() const {return _kind == Kind::basic;}
    Data init() const
    {
      Data ret{_dtype};
      // 
      return ret;
    }
  };

  using DtypeNames = map<string, dtype::T>;
  // using Dtype_ids = map<dtype::T, const dtype::Meta*>;
  using CptTypes = map<Compt_id, CptType>;
  // using CptNames = map<string, Compt_id>;
  using CptNames = map<CptPath, Compt_id>;
  using CptAggrs = map<string, vector<Compt_id>>;
  // using CptNames = CptAggrs;
  
protected:
  static const dtype::Dtypes _dtypes;
  const DtypeNames _dtype_names;
  // Dtype_ids _dtype_ids;

  CptTypes _cpt_types;
  CptNames _cpt_names;
  CptAggrs _cpt_aggrs;
  // set<Operation> _operations;

public:
  Ctx(ModelGen& mg, vector<Specs> cts);

  bool parse_data(string s, Data& out) const;  
  // Data parse_data(string s) const
  // {
  //   Data ret;
  //   if (!parse_data(begin(s), end(s), ret)) THROW_(Invalid, "parsing failed");
  //   return ret;
  // }
  
  Data create_type(Compt_id c) const
  {
    auto cpt = get_type(c);
    if (!cpt) THROW_(Not_found_T<Compt_id>, c);

    return cpt->init();
    // THROW_(Unimplemented, __func__);
    // return Data{};
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
  
  Compt_id find_type(CptPath p) const
  {
    // CptPath cp(p);
    auto it = _cpt_names.find(p);
    return it != end(_cpt_names) ? it->second : THROW_(Not_found, p.str());
  }

  CptPath get_name(Compt_id cpid) const
  {
    auto rg = filter(
      util::range::view(_cpt_names),
      [=](const CptNames::value_type& e){return cpid == e.second;}
    );
    ASSERT_EQ_(1, count(rg), "component name is not unique");
    return rg.front().first;
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
