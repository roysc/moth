// -*- coding: utf-8 -*-
// cptclass.hh
#pragma once

#include "basic.hh"
#include "log.hh"

// # each CptClass C
// # represents state that is shared across System s
// # hence, instantiate once. no need for special ctor.
// # def __init__(self, Cs):
// struct Entity;

struct SystemHandle {};
using SystemHandle_opt = uptr<SystemHandle>;

// Class interface
namespace data
{
struct Int {int32_t value;};
struct Float {float value;};
struct RlmDisc {uint32_t index; uint32_t realm_max; uint32_t offset;};
struct RlmCont {uint32_t index; uint32_t realm_max; uint32_t offset;};
struct Str {int _;};
}

namespace dtype
{
// essentially, built-in types
enum T: unsigned 
{ 
  // indentation sucks
  ty_int,
    ty_float,
    // Realm-values (of a range)
    ty_rdisc, // discrete (int)
    ty_rcont, // continuous (float)
    // ?
    ty_str,   // string, for what?
    // ty_tuple, // tuple rather than child grouping?
    // ty_calc,  // compt. calculated on reference?..
    ty_N // the nil-type
    };


unsigned size(T dt) 
{
  const m<T, unsigned> map = {
    // numeric types
    {dtype::ty_int, sizeof(data::Int)},
    {dtype::ty_float, sizeof(data::Float)},
    // a pair (i, r, scale) Realm defn and index, scale
    {dtype::ty_rdisc, sizeof(data::RlmDisc)},
    {dtype::ty_rcont, sizeof(data::RlmCont)},
    // --not sure how to impl. or if necessary
    {dtype::ty_str, sizeof(data::Str)},
    // {dtype::ty_tuple, ...},
    // {dtype::ty_calc, sizeof(unsigned*)},
    {dtype::ty_N, 0}
  };
  return map.at(dt);
}

T from_string(string s)
{
  const m<string, T> map = {
    {"int", dtype::ty_int},
    {"float", dtype::ty_float},
    {"rdisc", dtype::ty_rdisc},
    {"rcont", dtype::ty_rcont},
    {"str", dtype::ty_str},
    {"()", dtype::ty_N}
  };
  auto it = map.find(s);
  return it != end(map)? it->second : (
    LOG_(warning)(__func__, ": no dtype for \"", s, '"'),
    ty_N);
}

string to_string(T dt)
{
  const m<T, string> map = {
    {dtype::ty_int, "int"},
    {dtype::ty_float, "float"},
    {dtype::ty_rdisc, "rdisc"},
    {dtype::ty_rcont, "rcont"},
    {dtype::ty_str, "str"},
    {dtype::ty_N, "()"}
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
  CptClass(ModelGen* mg, string name, dtype::T dt): 
    _modelgen(mg), _name(name), _dtype(dt)
  {}
  
  // meta data
  string name() const {return _name;}
  // Compt_id id() const {return _id;}
  // components needed to impl. this one
  set<CptClass*> dependencies() const {return {};}
  // create a control context for components of this type
  SystemHandle* system_handle() {return {};}

  // bool operator<(const CptClass& that) {return _id }
};
