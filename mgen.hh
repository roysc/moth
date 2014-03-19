// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include "json.hh"
#include "err.hh"
#include "cptclass.hh" 
#include "entdef.hh"

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
using CptClasses = map<Compt_id, const CptClass>;
using EntDefs = map<string, set<Compt_id> >;
using CptIDs = map<const CptClass*, Compt_id>;
class ModelGen 
{
protected:
  Compt_id _next_id;
  // Types
  CptClasses _cptclasses;
  EntDefs _entdefs;
  CptIDs _cpt_ids;
  map<string, Compt_id> _ctrl_cpts;
  // const map<string, dtype::T> _builtins;
  // map<dtype::T, string> _dtype_defs;
  // vector<dtype::Meta> _dtype_meta;

  void read_cpts(Json);
  void read_ents(Json);
  
public:
  ModelGen(Json js);
  
  Compt_id fresh_id() {return ++_next_id;}
  map<Compt_id, const CptClass*> component_classes() 
  {
    map<Compt_id, const CptClass*> ret;
    for (auto&& c: _cptclasses) ret.emplace(c.first, &c.second);
    return ret;
  }
  Compt_id control_cpt(string nm) const {return _ctrl_cpts.at(nm);}

  const CptClass* get_cptclass(Compt_id cpid) 
  {
    auto it = _cptclasses.find(cpid);
    return it != end(_cptclasses) ? &(it->second) : nullptr;
  }

  Compt_id find_cptclass(string nm) 
  {
    auto it = find_if(
      begin(_cptclasses), end(_cptclasses),
      [=](CptClasses::value_type p) {return p.second.name() == nm;}
    );
    return it != end(_cptclasses) ? it->first : THROW_(Not_found, nm);
  }
  
  // stupid
  Compt_id unget_cptclass(const CptClass* cp)
  {
    auto it = _cpt_ids.find(cp);
    return it != end(_cpt_ids)? it->second : THROW_(Not_found, util::concat(cp));
  }

  EntDef get_entdef(string n) const
  {
    auto it = _entdefs.find(n);
    return it != end(_entdefs) ? EntDef{n, it->second} : THROW_(Not_found, n);
  }
};

