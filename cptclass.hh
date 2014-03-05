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


namespace dtype
{
enum T: unsigned { ty_int, ty_float, ty_rdisc, ty_rcont, ty_str, ty_N };

unsigned size(T dt) 
{
  const m<T, unsigned> map = {
    // numeric types
    {dtype::ty_int, sizeof(int)},
    {dtype::ty_float, sizeof(float)},
    // a pair (r, i) Range realm and index
    {dtype::ty_rdisc, sizeof(unsigned*) + sizeof(unsigned)},
    {dtype::ty_rcont, sizeof(unsigned*) + sizeof(unsigned)},
    // --not sure how to impl. or if necessary
    {dtype::ty_str, sizeof(int)},
    // the nil type placeholder
    // {dtype::ty_N, 0}
  };
  return map.at(dt);
}

};

struct CptClass
{
protected:
  ModelGen* _modelgen;
  // Compt_id _id;
  string _name;
  dtype::T _dtype;

public:
  CptClass(ModelGen* mg, string name): 
    _modelgen(mg), _name(name), _dtype(dtype::ty_N)
  {}
  
  // meta data
  string name() const {return _name;}
  // Compt_id id() const {return _id;}
  // components needed to impl. this one
  set<CptClass*> dependencies() const {return {};}
  // create a control context for components of this type
  SystemHandle* system_handle() {return new SystemHandle;}

  // bool operator<(const CptClass& that) {return _id }
};
