// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include "cptclass.hh" 

#include "err.hh"
#include "json.hh"

namespace konst
{
const string end_cond_name = "_end_";
const string gameover_evtspec = "_game_over_";
}

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
class ModelGen 
{
  using CptClasses = m<Compt_id, const CptClass>;
  using CptIDs = m<const CptClass*, Compt_id>;
protected:
  Compt_id _next_id;
  // Types
  CptClasses _cptclasses;
  CptIDs _cpt_ids;
  Compt_id _switch_cptid;

public:
  ModelGen(Json js);

  Compt_id fresh_id() {return ++_next_id;}
  m<Compt_id, const CptClass*> component_classes() 
  {
    m<Compt_id, const CptClass*> ret;
    for (auto&& c: _cptclasses) ret.emplace(c.first, &c.second);
    return ret;
  }
  Compt_id switch_cpt() const {return _switch_cptid;}

  const CptClass* get_class(Compt_id cpid) 
  {
    auto it = _cptclasses.find(cpid);
    return it != end(_cptclasses) ? &it->second : nullptr;
  }

  Compt_id unget_class(const CptClass* cp)
  {
    // auto it = find_if(begin(_cptclasses), end(cp), [])
    auto it = _cpt_ids.find(cp);
    return it != end(_cpt_ids)? it->second : throw err::Not_found(util::concat(cp));
  }
};

