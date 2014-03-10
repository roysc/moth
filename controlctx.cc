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
}

// const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}

