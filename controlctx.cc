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

ControlCtx::ControlCtx(ModelGen* mg):
  _controls(),
  _triggers(),
  _entities(),
  _cpt_ctx(mg->make_cpts())
{ 
  // controls:
  // for (auto&& c : _cpt_ctx.cpt_types()) {
  //   auto sh = c.second.control_handle();
  //   if (sh) _controls.emplace(c.first, sh);
  // }
}

// const cpt::Type* get_type(Compt_id cpid) {return _modelgen->get_type(cpid);}

