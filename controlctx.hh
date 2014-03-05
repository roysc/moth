// -*- coding: utf-8 -*-
// controlctx.hh
#pragma once

#include "event.hh"
#include "condition.hh"
#include "cptclass.hh"
#include "entity.hh"
#include "mgen.hh"

// "Contexts" for components needing centralized control
// eg. Location/coordinates
//     Physics
//     Information network

// TODO controller strategy
// Control context for sim. instance
struct ControlCtx 
{
protected:
  ModelGen* _modelgen;
  set<SystemHandle*> _systems;
  // entity and condition data
  set<Entity_ptr> _entities;
  set<Cond_ptr> _conditions;
  // control 
  uptr<Entity> _ctrl_ent;
  uptr<Condition> _end_cond;

public:
  ControlCtx(ModelGen* mg, set<SystemHandle*> shs):
    _modelgen(mg),
    _systems(shs),
    _end_cond(end_cond()),
    _ctrl_ent(ctrl_entity())
  { }

  set<Entity_ptr> entities() const {return _entities;}
  set<Cond_ptr> conditions() const {return _conditions;}
  set<SystemHandle*> systems() const {return _systems;}

  const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}
  
  // make an entity for control...
  Entity_ptr ctrl_entity()
  {
    // 
    m<Compt_id, CptClass*> cptmap;
    // system-handle
    for (auto&& sh: _systems)
    { // "get" SystemHandle
      auto cp = sh->cptclass();
      if (sh) cptmap.emplace(_modelgen->unget_class(cp), cp);
    }

    Entity_ptr ret = new Entity(this, cptmap);
    return ret;
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
};
