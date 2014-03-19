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
  _controls(),
  _triggers(),
  _entities()
{ 
  // controls:
  auto cpks = _modelgen->component_classes();
  for (auto&& c : cpks) {
    auto sh = c.second->control_handle();
    if (sh) _controls.emplace(c.first, sh);
  }
}

// const CptClass* get_cptclass(Compt_id cpid) {return _modelgen->get_cptclass(cpid);}

