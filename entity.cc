// -*- coding: utf-8 -*-
// entity.cc

#include "entity.hh"

// constructed with set of classes
Entity::Entity(ControlCtx* cx, v<Compt_id> cpts):
  _ctx(cx)
{
  // "Each C in Cpts represents a component meta-type"
  for (auto&& c : cpts)
    _compts.emplace(c, _ctx->get_class(c)->create());
}

// O(n)
Compt_ptr Entity::find(const string& cn) 
{
  for (auto&& c: _compts)
    if (_ctx->get_class(c.first)->name() == cn)
      return &c.second;
  return {};
}
