// -*- coding: utf-8 -*-
// controlctx.cc

#include "controlctx.hh"
#include "expression.hh"
#include "statement.hh"
#include "trigger.hh"

// "Contexts" for components needing centralized control
// eg. Location/coordinates
//     Physics
//     Information network

// Control context for sim. instance
struct ControlCtx;

ControlCtx::ControlCtx(ModelGen* mg):
  _modelgen(mg),
  _systems(),
  _triggers(),
  _entities()
{ 
  // _systems:
  auto cpks = _modelgen->component_classes();
  for (auto&& c : cpks) {
    auto sh = c.second->system_handle();
    if (sh) _systems.emplace(c.first, sh);
  }

  // The trigger, which when evaluated as true, ends the game;
  // eg. run ends after 4 "days"
  // Trigger(= time.n_day 4) -> Halt()
  // Cond_ptr end_cond() const {return _end_cond.get();}
  auto swid = _modelgen->control_cpt("_end_");
  auto tmid = _modelgen->control_cpt("_time_");
  _ctrl_ent.reset(new Entity(this, {swid, tmid}));
  
  // auto end_expr = new expr::EFun(
  //   "!", {new expr::ERef({_ctrl_ent.get(), swid})}
  // );
  // auto end_expr = new expr::ERef({_ctrl_ent.get(), swid});
  
  auto tm_expr = new expr::EFun("=="_op, {
      new expr::ERef({_ctrl_ent.get(), tmid}),
      new expr::ELit(Data::make<data::Int>(5))
    });
  auto end_stmt = new stmt::Halt;
  
  
  // auto tm_stmt = new stmt::Spawn("_tick_");

  // auto end_cond = new Trigger(end_expr, end_stmt);
  auto tm_cond = new Trigger(tm_expr, end_stmt);

  // _triggers.emplace(end_cond);
  _triggers.emplace(tm_cond);
}

// const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}

