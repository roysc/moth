// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include "basic.hh"
#include "cptclass.hh" 
#include "component.hh"

#include "json.hh"

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
class ModelGen 
{
protected:
  Compt_id _next_id;
  // Component types
  // m<Compt_id, CptClass> _cptclasses;
  m<Compt_id, CptClass> _cptclasses;
  
public:
  ModelGen(Json js):
    _next_id{}
  {
    // js = json.load(fp)
    auto cs = js.get_child("components");
    for (auto&& c : cs)
      _cptclasses.emplace(
        fresh_id(), CptClass(this, c.second.get<string>("name")));
    
  }

  Compt_id fresh_id() {return ++_next_id;}
  set<CptClass*> component_types() 
  {
    set<CptClass*> ret;
    for (auto&& c: _cptclasses) ret.emplace(&c.second);
    return ret;
  }

  const CptClass* get_class(Compt_id cpid) 
  {
    auto it = _cptclasses.find(cpid);
    return it != end(_cptclasses) ? &it->second : nullptr;
  }
};

