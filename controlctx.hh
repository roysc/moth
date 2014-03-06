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
  m<Compt_id, SystemHandle*> _systems;
  // entity and condition data
  set<Entity_ptr> _entities;
  set<Cond_ptr> _conditions;
  // control 
  uptr<Entity> _ctrl_ent;
  uptr<Condition> _end_cond;

public:
  ControlCtx(ModelGen* mg);
  virtual ~ControlCtx() {};

  set<Entity_ptr> entities() const {return _entities;}
  set<Cond_ptr> conditions() const {return _conditions;}
  m<Compt_id, SystemHandle*> systems() const {return _systems;}

  const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}
  
  // The control entity.
  // Possibly not useful directly
  Entity_ptr ctrl_entity() const {return _ctrl_ent.get();}
  
  // The condition, which when evaluated as true, ends the game;
  // expressed as: 
  // eg. run ends after 4 "days"
  // Condition(= time.n_day 4)
  // -> Halt()
  Cond_ptr end_cond() const {return _end_cond.get();}
};
