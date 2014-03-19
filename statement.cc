// -*- coding: utf-8 -*-
// statement.cc

#include "statement.hh"
#include "controlctx.hh"

namespace stmt
{
void Halt::execute(ControlCtx& ctx, Entity_ptr) const
{
  LOG_(debug)(__PRETTY_FUNCTION__);
  // create warning entity?
  // delete everything?

  // Compt_addr end_flag = ctx->ctrl_component("_end_");
  // Compt_addr end_flag = ctx->ctrl_component("_end_");
  // end_flag()->set(data::Bool{true});

  // auto alert = ctx.create_entity({ctx.get_cptclass("_msg_")});
  
  LOG_(info)("Halt was triggered");

  // hacky, works for now
  throw *this;
}
}
