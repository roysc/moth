// -*- coding: utf-8 -*-
// cptclass.hh
#pragma once

#include "basic.hh"

// # each CptClass C
// # represents state that is shared across System s
// # hence, instantiate once. no need for special ctor.
// # def __init__(self, Cs):
// struct Entity;

struct SystemHandle {};
using SystemHandle_opt = uptr<SystemHandle>;

struct CptClass
{
protected:
  ModelGen* _modelgen;
  // Compt_id _id;
  string _name;
  // DataType _dtype;

public:
  CptClass(ModelGen* mg, string name): 
    _modelgen(mg), _name(name) {}
  
  // meta data
  string name() const {return _name;}
  // Compt_id id() const {return _id;}
  // components needed to impl. this one
  set<CptClass*> dependencies() const {return {};}
  // create a control context for components of this type
  SystemHandle* system_handle() {return new SystemHandle;}

  // bool operator<(const CptClass& that) {return _id }
};
