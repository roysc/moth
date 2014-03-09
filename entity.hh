// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include "component.hh"
struct ControlCtx;

// struct ControlCtx;
struct Compt_addr: pair<Entity_ptr, Compt_id>
{
  using pair::pair;
  Data_ptr operator()() const;
};

// An object instance in the running model
struct Entity;

// 
using Compt_data = m<Compt_id, Data>;
struct Entity
{
protected:
  ControlCtx* _ctx;
  Compt_data _compts;
  
public:
  // constructed with set of classes
  Entity(ControlCtx* cx, v<Compt_id> cpts);  

  // O(log n)
  Compt_ptr get(Compt_id id)
  {
    auto it = _compts.find(id);
    return it != end(_compts) ? &it->second : THROW_(Not_found_T<Compt_id>, id);
  }
  // O(n)
  Compt_ptr find(const string& cn);
};
