// -*- coding: utf-8 -*-
// entity.cc

#include "entity.hh"
#include "controlctx.hh"

// dereference
Data_ptr Compt_addr::operator()() const {return first->get(second);}

// constructed with set of classes
Entity::Entity(ControlCtx* cx, v<Compt_id> cpts):
  _ctx(cx)
{
  LOG_PUSH_(lctor)(__PRETTY_FUNCTION__);
  // "Each C in Cpts represents a component meta-type"
  for (auto&& c : cpts)
    _compts.emplace(c, _ctx->get_class(c)->create());
  LOG_SHOW_TO_(_compts, lctor);
}

string Entity::get_name(Compt_id id) const {return _ctx->get_class(id)->name();}

// O(n)
Compt_ptr Entity::find(const string& cn) 
{
  for (auto&& c: _compts)
    if (_ctx->get_class(c.first)->name() == cn)
      return &c.second;
  return {};
}
