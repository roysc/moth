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
  _modelgen(mg)
  // _end_cond(end_cond()),
  // _ctrl_ent(ctrl_entity())
{ 
  for (auto&& c : _modelgen->component_classes())
  {
    auto sh = c.second->system_handle();
    if (sh) _systems.emplace(c.first, sh);
  }
  // auto ctrlsys =
  // control CptClass
  auto swid = _modelgen->switch_cpt();
  auto& swsys = _systems.at(swid);
  // 
  _ctrl_ent.reset(new Entity(this, {swid}));
  _end_cond.reset(new Condition("=", {{_ctrl_ent.get(), swid}}));
}

// const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}

