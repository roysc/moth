// -*- coding: utf-8 -*-
// controlctx.hh
#pragma once

// #include "cptclass.hh"
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
  m<Compt_id, SystemHandle*> systems() const {return _systems;}
  // The control entity. Possibly not useful directly
  Entity_ptr ctrl_entity() const {return _ctrl_ent.get();}
  Entity_ptr create_entity(v<Compt_id> cs)
  {
    auto insr = _entities.emplace(new Entity(this, cs));
    return insr.second? insr.first->get() : LOG_EVALN_(nullptr, __func__);
  }
  
  // class lookup, fwd to modelgen
  const CptClass* get_class(Compt_id cpid) const {return _modelgen->get_class(cpid);}
  Compt_id find_class(string nm) {return _modelgen->find_class(nm);}

  Compt_addr ctrl_component(string nm)
  {
    // check name is actual control component
    auto cpid = find_class(nm);
    ASSERT_(_ctrl_ent->has(cpid), "Control entity has no component for id = ", cpid);
    return {_ctrl_ent.get(), cpid};
  }
};
