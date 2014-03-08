// -*- coding: utf-8 -*-
// controlctx.cc

#include "controlctx.hh"

// "Contexts" for components needing centralized control
// eg. Location/coordinates
//     Physics
//     Information network

// Control context for sim. instance
struct ControlCtx;

ControlCtx::ControlCtx(ModelGen* mg):
  _modelgen(mg),
  _systems(),
  _conditions(),
  _entities()
{ 
  // _systems:
  auto cpks = _modelgen->component_classes();
  for (auto&& c : cpks)
  {
    auto sh = c.second->system_handle();
    if (sh) _systems.emplace(c.first, sh);
  }

  auto swid = _modelgen->switch_cpt();
  auto tmid = _modelgen->time_cpt();
  _ctrl_ent.reset(new Entity(this, {swid, tmid}));
  
  // auto end_expr = new expr::EFun(
  //   "!", {new expr::ERef({_ctrl_ent.get(), swid})}
  // );
  auto end_expr = new expr::ERef({_ctrl_ent.get(), swid});
  
  Compt_addr cent_tm;
  auto tm_expr = new expr::EFun("==", {
      new expr::ERef({_ctrl_ent.get(), tmid}),
      new expr::ELit(Data::make<data::Int>(5))
    });

  // auto end_stmt = stmt::Statement(
  //   konst::gameover_evtspec,    // name "game over"
  //   EventKind::destroy,         // destroy condition entity
  //   _ctrl_ent.get());
  auto end_stmt = new stmt::EndGame;
  auto tm_stmt = new stmt::Spawn("_tick_");

  auto end_cond = new Condition(end_expr, end_stmt);
  auto tm_cond = new Condition(tm_expr, tm_stmt);

  _conditions.emplace(end_cond);
  _conditions.emplace(tm_cond);
}

// const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}

