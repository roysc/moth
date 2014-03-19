// -*- coding: utf-8 -*-
// cptclass.hh
#pragma once

#include "log.hh"
// #include "basic.hh"
#include "control.hh"
#include "component.hh"

// # each CptClass C
// # represents state that is shared across Control s
// # hence, instantiate once. no need for special ctor.
// # def __init__(self, Cs):
// struct Entity;
struct ModelGen;

struct CptClass
{
protected:
  ModelGen* _modelgen;
  // handled as map key for now
  // Compt_id _id;
  string _name;
  dtype::T _dtype;

public:
  CptClass(ModelGen* mg, string name, dtype::T dt): 
    _modelgen(mg), _name(name), _dtype(dt)
  {}
  
  // meta data
  string name() const {return _name;}
  dtype::T data_type() const {return _dtype;}
  // Compt_id id() const {return _id;}
  
  // components needed to impl. this one
  set<CptClass*> dependencies() const {return {};}

  // user control integration:
  // create a control context for components of this type
  // control_handle() => Handle(_) -- supports user control
  //                  | nullptr   -- no
  ControlHandle* control_handle() const {return {};}

  Data create() const {return {_dtype};}
  // bool operator<(const CptClass& that) {return _id }
};

template <class Ch,class Tr>
std::basic_ostream<Ch,Tr>& 
operator<<(std::basic_ostream<Ch,Tr>& out, const CptClass& cpc)
{
  util::print_to(out, "CptClass(\"", cpc.name(), "\",", cpc.data_type(), ")");
  return out;
}
