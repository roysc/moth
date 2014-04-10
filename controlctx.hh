// -*- coding: utf-8 -*-
// controlctx.hh
#pragma once

#include <functional>

#include "log.hh"
#include "expression.hh"
#include "statement.hh"
#include "trigger.hh"
#include "entity.hh"

// "Contexts" for components needing centralized control
// eg. Location/coordinates
//     Physics
//     Information network

struct PtrCompare: std::less<uptr>
{
  // using is_transparent = int;

  // template <class A, class B>
  // operator()(const A& a, const B& b)
  // {
  //   return a < b;
  // }
};

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
  using Entity_id = size_t;
  using Trigger_id = size_t;

  using Entities = set<uptr<Entity>, PtrCompare>;
  using Triggers = set<uptr<Trigger>, PtrCompare>;
  Entities _entities;
  Triggers _triggers;
  
  Entity* _ctrl_ent;

public:
  ControlCtx(ModelGen& mg);
  ~ControlCtx() {}

  const Entities& entities() const { return _entities; }
  const Triggers& triggers() const { return _triggers; }

  // map<Compt_id, ControlHandle*> controls() const {return controls;}
  // create ents., triggers
  Entity* create_entity(vector<Compt_id> cs);  
  // set a (condition, statement) pair
  Trigger* create_trigger(const expr::Expr* e, stmt::Statement* s);
  
  bool delete_trigger(const Trigger* tr)
  {
    auto it = _triggers.find(tr);
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
