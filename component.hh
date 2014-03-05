// -*- coding: utf-8 -*-
// component.hh
#pragma once

#include "basic.hh"
#include "data.hh"

// the idea is that a component is only accessed through Compt_addr keys
// using Compt_addr = pair<Entity_ptr, Compt_id>;

// """A trait of an entity"""
struct Component
{
protected:
  Data _data;

public:
  // Component(Size bytes)
  Component(dtype::T dt):
    _data(dt) {}
  
};
