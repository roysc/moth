// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include "util/range/lazy.hh"

#include "json.hh"
#include "err.hh"

#include "entdef.hh"
#include "cptctx.hh" 

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
using CptTypes = map<Compt_id, const cpt::Specs>;
using EntDefs = map<string, set<Compt_id> >;
// using CptIDs = map<const cpt::Specs*, Compt_id>;
class ModelGen 
{
protected:
  Json _json;
  Compt_id _next_id;
  // Types
  // CptTypes _cptclasses;
  EntDefs _entdefs;
  // CptIDs _cpt_ids;
  map<string, Compt_id> _ctrl_cpts;
  // const map<string, dtype::T> _builtins;
  // map<dtype::T, string> _dtype_defs;
  // vector<dtype::Meta> _dtype_meta;

  void read_ents();
  
public:
  ModelGen(Json js): _json(js) {}
  
  cpt::Ctx make_cpts();
  
  Compt_id fresh_id() {return ++_next_id;}
  // Compt_id control_cpt(string nm) const {return _ctrl_cpts.at(nm);}

  EntDef get_entdef(string n) const
  {
    auto it = _entdefs.find(n);
    return it != end(_entdefs) ? EntDef{n, it->second} : THROW_(Not_found, n);
  }
};

