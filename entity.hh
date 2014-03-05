// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include "basic.hh"
#include "mrun.hh"
#include "component.hh"

// 
struct Entity
{
protected:
  ControlCtx* _ctx;
  Compt_data _compts;
  
public:
  // constructed with set of classes
  Entity(ControlCtx* cx, m<Compt_id, CptClass*> cpts):
    _ctx(cx)
  {
    // "Each C in Cpts represents a component meta-type"
    for (auto&& c : cpts)
      _compts.emplace(c.first, Compt_own{create(c.second)});
  }

  // O(log n)
  Compt_ptr get(const Compt_id id) {return _compts.at(id).get();}
  // O(n)
  Compt_ptr find(const string& cn) 
  {
    for (auto&& c: _compts)
      if (_ctx->get_class(c.first)->name() == cn)
        return c.second.get();
    return {};
  }

  // TODO
  Compt_ptr create(CptClass* cpc) 
  {
    
    return {};
  }
};
