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
  _caddr().set(res);
}

// void Incr::execute(ControlCtx& ctx) const 
// {
//   LOG_(debug)(__PRETTY_FUNCTION__);
//   auto v = _caddr().get_at<data::Int, 0>();
//   // _caddr(0)->set_at<data::Int, 0>(v + _amt);
//   _caddr().set<data::Int>(v + _amt);
// }

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

namespace builder
{
namespace exb = expr::builder;

St operator<<(Compt_addr a, Ex v)
{
  return {new Update(r.address, v)};
}

// St operator+=(Compt_addr a, int v)
// {
//   return {new Update(a, exb::lit<int>(a().get_at<data::))};
// }
}
}
