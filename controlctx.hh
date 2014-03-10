// -*- coding: utf-8 -*-
// controlctx.hh
#pragma once

// #include "cptclass.hh"
#include "log.hh"
#include "statement.hh"
#include "trigger.hh"
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
  // entity and trigger data
  set<uptr<Trigger> > _triggers;
  set<uptr<Entity> > _entities;
  // control 
  // uptr<Trigger> _end_cond;
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
  set<Trigger*> triggers() const
  {
    set<Trigger*> ret;
    for (auto&& t: _triggers) ret.insert(t.get());
    return ret;
  }
  // m<Compt_id, SystemHandle*> systems() const {return _systems;}
  // create ents., triggers
  Entity_ptr create_entity(v<Compt_id> cs)
  {
    auto insr = _entities.emplace(new Entity(this, cs));
    return insr.second? insr.first->get() : LOG_EVAL_(nullptr);
  }
  bool set_trigger(Trigger* t) {return _triggers.emplace(t).second;}

  // handle a Signal object throw from event loop
  // relies on dynamic type
  bool handle_signal(const stmt::Signal& sig) {return !sig.fatal();}
  
  // class lookup, fwd to modelgen
  const CptClass* get_class(Compt_id cpid) const {return _modelgen->get_class(cpid);}
  Compt_id find_class(string nm) {return _modelgen->find_class(nm);}
};
