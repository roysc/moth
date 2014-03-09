// -*- coding: utf-8 -*-
// controlctx.hh
#pragma once

// #include "cptclass.hh"
// #include "condition.hh"
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
  m<Compt_id, SystemHandle*> _systems;
  // entity and condition data
  set<Cond_ptr> _conditions;
  set<uptr<Entity> > _entities;
  // control 
  // uptr<Condition> _end_cond;
  uptr<Entity> _ctrl_ent;

public:
  ControlCtx(ModelGen* mg);
  virtual ~ControlCtx() {}

  set<Entity_ptr> entities() const
  {
    set<Entity_ptr> ret;
    for (auto&& e: _entities) ret.insert(e.get());
    return ret;
  }
  set<Cond_ptr> conditions() const {return _conditions;}
  m<Compt_id, SystemHandle*> systems() const {return _systems;}
  // The control entity. Possibly not useful directly
  Entity_ptr ctrl_entity() const {return _ctrl_ent.get();}
  Entity_ptr create_entity(v<Compt_id> cs)
  {
    auto insr = _entities.emplace(new Entity(this, cs));
    return insr.second? insr.first->get() : LOG_EVALN_(nullptr, __func__);
  }
  
  // class lookup, fwd to modelgen
  const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}
  Compt_id find_class(string nm) {return _modelgen->find_class(nm);}
};
