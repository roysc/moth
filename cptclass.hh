// -*- coding: utf-8 -*-
// cptclass.hh
#pragma once

#include "basic.hh"
#include "system.hh"
#include "log.hh"
#include "component.hh"
#include "data.hh"

// # each CptClass C
// # represents state that is shared across System s
// # hence, instantiate once. no need for special ctor.
// # def __init__(self, Cs):
// struct Entity;

struct CptClass
{
protected:
  ModelGen* _modelgen;
  // Compt_id _id;
  string _name;
  dtype::T _dtype;

public:
  CptClass(ModelGen* mg, string name, dtype::T dt): 
    _modelgen(mg), _name(name), _dtype(dt)
  {}
  
  // meta data
  string name() const {return _name;}
  // Compt_id id() const {return _id;}
  // components needed to impl. this one
  set<CptClass*> dependencies() const {return {};}

  // user control integration:
  // create a control context for components of this type
  // system_handle() => Handle(_) -- supports user control
  //                  | nullptr   -- no
  SystemHandle* system_handle() {return {};}

  Component create() {return Component(_dtype);}
  // bool operator<(const CptClass& that) {return _id }
};
