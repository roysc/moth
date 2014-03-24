// -*- coding: utf-8 -*-
// entity.hh
#pragma once

#include "data.hh"
#include "component.hh"
#include "cptctx.hh"

// An object instance in the running model
//
using Compts = map<Compt_id, Data>;
// using Compts = map<Compt_id, Data
struct Entity
{
protected:
  const cpt::Ctx& _ctx;
  Compts _compts;
  
public:
  // constructed with set of classes
  Entity(cpt::Ctx& cx, vector<Compt_id> cpts);  

  // O(log n)
  Data* get(Compt_id id)
  {
    auto it = _compts.find(id);
    return it != end(_compts) ? &(it->second) : THROW_(Not_found_T<Compt_id>, id);
  }
  bool has(Compt_id id) const {return _compts.find(id) != end(_compts);}
  Compt_addr ref(Compt_id id) {return ref({id});}
  Compt_addr ref(vector<Compt_id> id) {return {this, id};}
  // string name of component
  string get_name(Compt_id id) const;
  
  // O(n)
  // Compt_ptr find(const string& cn);
};
