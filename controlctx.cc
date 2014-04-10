// -*- coding: utf-8 -*-
// controlctx.cc

#include "controlctx.hh"
#include "mgen.hh"

// "Contexts" for components needing centralized control
// eg. Location/coordinates
//     Physics
//     Information network

// Control context for sim. instance
struct ControlCtx;

ControlCtx::ControlCtx(ModelGen& mg):
  _controls(),
  _triggers(),
  _entities(),
  _cpt_ctx(mg.make_cpts())
{ 
  // controls:
  // for (auto&& c : _cpt_ctx.cpt_types()) {
  //   auto sh = c.second.control_handle();
  //   if (sh) _controls.emplace(c.first, sh);
  // }
}

// map<Compt_id, ControlHandle*> controls() const {return controls;}
// create ents., triggers
Entity* ControlCtx::create_entity(vector<Compt_id> cs)
{
  auto insr = _entities.emplace(new Entity(_cpt_ctx, cs));
  return insr.second? insr.first->get() : THROW_(Insertion, "Entity");
}

// set a (condition, statement)
Trigger* ControlCtx::create_trigger(const expr::Expr* e, stmt::Statement* s)
{
  if (!e)
    // for nil expression, set const Pred = 1
    e = new expr::ELit(Data::make<data::Bool>(true));
  auto tr = new Trigger{e, s};
  auto r = _triggers.emplace(tr);
  return r.second? r.first->get() : THROW_T_(Insertion, *tr);
}

// const cpt::Type* get_type(Compt_id cpid) {return _modelgen->get_type(cpid);}

