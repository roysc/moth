// -*- coding: utf-8 -*-
// controlctx.hh
#pragma once

#include "log.hh"
#include "expression.hh"
#include "statement.hh"
#include "trigger.hh"
#include "entity.hh"

// "Contexts" for components needing centralized control
// eg. Location/coordinates
//     Physics
//     Information network

// TODO controller strategy
// Control context for sim. instance
struct ControlCtx 
{
protected:
  // ModelGen* _modelgen;
  cpt::Ctx _cpt_ctx;
  // EntCtx _ent_ctx;
  map<Compt_id, ControlHandle*> _controls;
  // entity and trigger data
  set<uptr<Entity> > _entities;
  set<uptr<Trigger> > _triggers;
  uptr<Entity> _ctrl_ent;

public:
  ControlCtx(ModelGen& mg);
  virtual ~ControlCtx() {}

  set<Entity*> entities() const
  {
    set<Entity*> ret;
    for (auto&& e: _entities) ret.insert(e.get());
    return ret;
  }
  set<Trigger*> triggers() const
  {
    set<Trigger*> ret;
    for (auto&& t: _triggers) ret.insert(t.get());
    return ret;
  }
  // map<Compt_id, ControlHandle*> controls() const {return controls;}
  // create ents., triggers
  Entity* create_entity(vector<Compt_id> cs)
  {
    auto insr = _entities.emplace(new Entity(_cpt_ctx, cs));
    return insr.second? insr.first->get() : LOG_EVAL_(nullptr);
  }

  // set a (condition, statement)
  Trigger* set_trigger(const expr::Expr* e, stmt::Statement* s)
  {
    if (!e)
    // for nil expression, set const Pred = 1
      e = new expr::ELit(Data::make<data::Bool>(true));
    auto tr = new Trigger{e, s};
    auto r = _triggers.emplace(tr);
    return r.second? r.first->get() : THROW_T_(Insertion, *tr);
  }
  bool remove_trigger(const Trigger* tr)
  {
    auto it = find_if(
      begin(_triggers), end(_triggers),
      [=](const decltype(_triggers)::value_type& tp){return tr == tp.get();}
    );
    if (it == end(_triggers))
      return false;
    _triggers.erase(it);
    return true;
  }
  
  // handle a Signal object throw from event loop
  // relies on dynamic type
  bool handle_signal(const stmt::Statement& sig)
  {
    // small set of important events handled directly
    return !sig.fatal();
  }
  
  // class lookup, fwd to modelgen
  // const cpt::Type* get_type(Compt_id cpid) const {return _cpt_ctx.get_type(cpid);}
  vector<Compt_id> find_type(string nm) const {return _cpt_ctx.find_type(nm);}
};
