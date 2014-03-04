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

// TODO controller strategy
// Control context for sim. instance
struct ControlCtx 
{
  ControlCtx(ModelGen* mg, set<SystemHandle*> shs):
    _modelgen(mg),
    _end_cond(end_cond()),
    _ctrl_ent(ctrl_entity())
  {}
  
  // make an entity for control...
  static
  Entity_ptr ctrl_entity()
  {
    return {};
  }

  // The condition, which when evaluated as true, ends the game;
  // expressed as: 
  static
  Cond_ptr end_cond()
  {
    // eg. run ends after 4 "days"
    // Condition(= time.n_day 4)
    // -> Halt()
    return {};
  }
  set<Entity_ptr> entities() {return _entities;}
  set<Cond_ptr> conditions() {return _conditions;}

  const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}

protected:
  ModelGen* _modelgen;
  set<Entity_ptr> _entities;
  set<Cond_ptr> _conditions;
  uptr<Entity> _ctrl_ent;
  uptr<Condition> _end_cond;
};
