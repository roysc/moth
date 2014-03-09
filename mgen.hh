// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include "json.hh"
#include "err.hh"
#include "cptclass.hh" 

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
class ModelGen 
{
  using CptClasses = m<Compt_id, const CptClass>;
  using CptIDs = m<const CptClass*, Compt_id>;
protected:
  Compt_id _next_id;
  // Types
  CptClasses _cptclasses;
  CptIDs _cpt_ids;
  m<string, Compt_id> _ctrl_cpts;
  static const m<string, dtype::T> builtins;
  
public:
  ModelGen(Json js);

  Compt_id fresh_id() {return ++_next_id;}
  m<Compt_id, const CptClass*> component_classes() 
  {
    m<Compt_id, const CptClass*> ret;
    for (auto&& c: _cptclasses) ret.emplace(c.first, &c.second);
    return ret;
  }
  Compt_id control_cpt(string nm) const {return _ctrl_cpts.at(nm);}

  const CptClass* get_class(Compt_id cpid) 
  {
    auto it = _cptclasses.find(cpid);
    return it != end(_cptclasses) ? &it->second : nullptr;
  }

  Compt_id find_class(string nm) 
  {
    auto it = find_if(
      begin(_cptclasses), end(_cptclasses),
      [=](CptClasses::value_type p) {return p.second.name() == nm;});
    return it != end(_cptclasses) ? it->first : THROW_(Not_found, nm);
  }
  
  // stupid
  Compt_id unget_class(const CptClass* cp)
  {
    auto it = _cpt_ids.find(cp);
    return it != end(_cpt_ids)? it->second : THROW_(Not_found, util::concat(cp));
  }
};

