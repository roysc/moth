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
  _ctrl_ent.reset(new Entity(this, {swid}));
  
  Compt_addr cent_ref{_ctrl_ent.get(), swid};
  auto end_expr = new expr::EFun("!", {
      new expr::ERef(cent_ref)
    });
  // auto end_ekl = EvtSpec(konst::gameover_evtspec, EventKind::destroy);

  // auto end_stmt = stmt::Statement(
  //   konst::gameover_evtspec,    // name "game over"
  //   EventKind::destroy,         // destroy condition entity
  //   _ctrl_ent.get());
  auto end_stmt = new stmt::EndGame();

  _end_cond.reset(new Condition(end_expr, end_stmt));

  _conditions.emplace(_end_cond.get());
}

// const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}

