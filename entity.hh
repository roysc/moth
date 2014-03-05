// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include "basic.hh"
#include "component.hh"

// 
using Compt_data = m<Compt_id, Component>;
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
      _compts.emplace(c.first, c.second->create());
  }

  // O(log n)
  Compt_ptr get(const Compt_id id) {return &_compts.at(id);}
  // O(n)
  Compt_ptr find(const string& cn);

  // TODO
  Compt_ptr create(CptClass* cpc) 
  {
    return {};
  }
};
