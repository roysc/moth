// -*- coding: utf-8 -*-
// component.hh
#pragma once

#include "basic.hh"

// """A trait of an entity"""
struct Component
{
public:
  Component(string e)
  {
    // ?
  }
};
using Compt_id = uint32_t;
using Compt_own = uptr<Component>;
using Compt_ptr = ptr<Component>;
using Compt_data = m<Compt_id, Compt_own>;
using Compt_addr = pair<Entity_ptr, Compt_id>;
// using Compt_set = set<Compt_ptr>;
