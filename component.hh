// -*- coding: utf-8 -*-
// component.hh
#pragma once

#include "basic.hh"
#include "cptclass.hh"

struct Component;
using Compt_id = unsigned;
using Compt_own = uptr<Component>;
using Compt_ptr = Component*;
using Compt_data = m<Compt_id, Compt_own>;
// the idea is that a component is only accessed through Compt_addr keys
using Compt_addr = pair<Entity_ptr, Compt_id>;

// struct Field;

// struct Field
// {
//   dtype::T _dtype;
//   uptr<uint8_t> _ptr;
  
//   template <class D>
//   Data get();
// };

// template<>
// Field::get<dtype::ty_int>() {return _ptr}

// """A trait of an entity"""
struct Component
{
  using Data = v<uint8_t>;
protected:
  dtype::T _dtype;
  Data _data;

public:
  // Component(Size bytes)
  Component(dtype::T dt):
    _dtype(dt), _data(dtype::size(dt)) {}

  dtype::T dtype() const {return _dtype;}
  Data::value_type* raw() 
  {
    assert(!_data.empty() && "data not initialized");
    return &_data[0];
  }
};
