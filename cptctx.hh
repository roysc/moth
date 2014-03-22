// -*- coding: utf-8 -*-
// cptctx.hh
#pragma once

#include <algorithm>
#include "util/range/algo.hh"

#include "log.hh"
// #include "basic.hh"
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
  using Members = vector<string>;
  using Aliases = map<string, string>;
protected:
  string _name;
  Members _members;
  // Aliases _aliases;
  
public:
  Specs(string n, Members ms, Aliases as = {}): 
    _name(n), _members(ms)// , _aliases(as)
  {}

public:
  string name() const {return _name;}
  Members members() const {return _members;}
  // bool operator<(const CptClass& that) {return _id }
  // Data create()

  // create a control handle for components, if supported
  ControlHandle* control_handle() const {return {};}
};

//! The "context" of a set of well-defined components
//
// distinction between dtypes, component types, and component data:
// dtype: represents data part of a basic component
// basic component: 1-tuple mapping to single dtype : (name, dtype)
// component type: tuple(of basic components) : (name, [basic])
struct Ctx
{
  struct CptType;

  struct CptType
  {
    enum Kind { basic, named };
    Kind _kind;
    dtype::T _dtype;
    CptType(bool k, dtype::T dt):
      _kind(k? basic : named), _dtype(dt) {}
    // bool is_basic() const {return 1 == _mems.size();}
    bool is_basic() const {return _kind == Kind::basic;}
  };
  
  using CptTypes = map<Compt_id, CptType>;
  using CptNames = map<string, Compt_id>;
  using CptAggrs = map<string, vector<Compt_id>>;
  // using CptNames = CptAggrs;
  
protected:
  using DtypeNames = map<string, dtype::T>;
  // using Dtype_ids = map<dtype::T, const dtype::Meta*>;

  static const dtype::Dtypes _dtypes;
  const DtypeNames _dtype_names;
  // Dtype_ids _dtype_ids;

  CptTypes _cpt_types;
  CptNames _cpt_names;
  CptAggrs _cpt_aggrs;
  // set<Operation> _operations;

public:
  Ctx(ModelGen& mg, vector<Specs> cts);
  
  Data create_type(Compt_id c) const
  {
    auto cpt = get_type(c);
    if (!cpt) THROW_(Not_found_T<Compt_id>, c);

    THROW_(Unimplemented, __func__);
    return Data{};
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
  
  Compt_id find_type(string nm) const
  {
    auto it = _cpt_names.find(nm);
    return it != end(_cpt_names) ? it->second : THROW_(Not_found, nm);
  }

  string get_name(Compt_id cpid) const
  {
    auto rg = filter(
      util::view(_cpt_names),
      [=](CptNames::value_type e){return cpid == e.second;}
    );
    ASSERT_EQ_(1, count(rg), "component name is not unique");
    return rg.front().first;
  }
};

template <class Ch,class Tr>
std::basic_ostream<Ch,Tr>& 
operator<<(std::basic_ostream<Ch,Tr>& out, const Specs& cpc)
{
  util::print_to(out, "CptClass(\"", cpc.name(), "\",", cpc.members(), ")");
  return out;
}
}
