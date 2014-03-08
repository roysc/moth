// -*- coding: utf-8 -*-
// controlctx.hh
#pragma once

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
  m<Compt_id, SystemHandle*> _systems;
  // entity and condition data
  set<Cond_ptr> _conditions;
  set<Entity_ptr> _entities;
  // control 
  // uptr<Condition> _end_cond;
  uptr<Entity> _ctrl_ent;

public:
  ControlCtx(ModelGen* mg);
  virtual ~ControlCtx() {}

  set<Entity_ptr> entities() const {return _entities;}
  set<Cond_ptr> conditions() const {return _conditions;}
  m<Compt_id, SystemHandle*> systems() const {return _systems;}
  // The control entity. Possibly not useful directly
  Entity_ptr ctrl_entity() const {return _ctrl_ent.get();}

  // class lookup, fwd to modelgen
  const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}
};
