// -*- coding: utf-8 -*-
// statement.cc

#include "statement.hh"
#include "controlctx.hh"

namespace stmt
{
// set target data
void Update::execute(ControlCtx& ctx) const 
{
  LOG_(debug)(__PRETTY_FUNCTION__);
  auto res = _expr->eval();
  _caddr(0)->set(res);
}

void Incr::execute(ControlCtx& ctx) const 
{
  LOG_(debug)(__PRETTY_FUNCTION__);
  auto v = _caddr(0)->get<data::Int>();
  _caddr(0)->set(data::Int{v.value + _amt});
}

void Halt::execute(ControlCtx& ctx) const
{
  LOG_(debug)(__PRETTY_FUNCTION__);
  // create warning entity?
  // delete everything?

  // Compt_addr end_flag = ctx->ctrl_component("_end_");
  // Compt_addr end_flag = ctx->ctrl_component("_end_");
  // end_flag()->set(data::Bool{true});

  // auto alert = ctx.create_entity({ctx.get_type("_msg_")});
  
  LOG_(info)("Halt was triggered");

  // hacky, works for now
  throw *this;
}
}
