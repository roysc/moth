// -*- coding: utf-8 -*-
// statement.cc

#include "statement.hh"
#include "controlctx.hh"

namespace stmt
{
void Update::execute(ControlCtx& ctx, Entity* ent) const 
{
  LOG_(debug)(__PRETTY_FUNCTION__);
  // set target data
  auto res = _expr->eval();
  ent->ref(_cpid)()->set(res);
}

void Halt::execute(ControlCtx& ctx, Entity_ptr) const
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
