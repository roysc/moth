// -*- coding: utf-8 -*-

#include "controlctx.hh"
#include "entity.hh"

// O(n)
Compt_ptr Entity::find(const string& cn) 
{
  for (auto&& c: _compts)
    if (_ctx->get_class(c.first)->name() == cn)
      return &c.second;
  return {};
}
