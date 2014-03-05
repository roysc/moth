// -*- coding: utf-8 -*-
// mgen.hh
#pragma once

#include "basic.hh"
#include "cptclass.hh" 
#include "component.hh"

#include "err.hh"
#include "log.hh"
#include "json.hh"

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
class ModelGen 
{
protected:
  Compt_id _next_id;
  // Component types
  // m<Compt_id, CptClass> _cptclasses;
  m<Compt_id, CptClass> _cptclasses;
  
public:
  ModelGen(Json js):
    _next_id{}
  {
    LOG_PUSH_(lctor)(__func__);

    // js = json.load(fp)
    LOG_(info)("loading JSON");
    
    auto cpts_js = js.get_child_optional("components");
    auto ents_js = js.get_child_optional("entities");

    auto make = [&lctor, this](string n, dtype::T dt) {
      LOG_(info)("creating: ", n, ": ", dtype::to_string(dt));

      auto res = _cptclasses.emplace(fresh_id(), CptClass(this, n, dt));
      if (!res.second)
        LOG_(error)("CptClass insertion failed");
      // throw err::Internal("CptClass insertion failed");
    };

    for (auto&& c: cpts_js? *cpts_js : Json()) {
      // name. easy.
      auto name = c.second.get<string>("name");
      
      { // get types

        auto tp = c.second.get_child("type");
        // type could be value, or dict.
        // for value, define component
        auto val = tp.get_value<string>();
        if (!val.empty()) {
          LOG_(info)("single-type");
          auto dt = dtype::from_string(val);
          make(name, dt);
        }
        // no children; it's an empty {}
        else if (tp.empty()) {
          LOG_(info)("empty-type");
          make(name, dtype::ty_N);
        }
        // has multiple child components
        else {
          LOG_(info)("multi-type");
          
          for (auto&& child: tp)
          {
            auto ch_name = child.first;
            auto ch_val = child.second.get_value<string>();
            // LOG_(info)("type node: \"", ch_val, '"');

            auto dt = dtype::from_string(ch_val);
            // LOG_(info)(name, " child: ", ch_name);

            auto mkname = util::concat(name, '.', ch_name);
            make(mkname, dt);
          }
        }
      }
    }
  }

  Compt_id fresh_id() {return ++_next_id;}
  set<CptClass*> component_types() 
  {
    set<CptClass*> ret;
    for (auto&& c: _cptclasses) ret.emplace(&c.second);
    return ret;
  }

  const CptClass* get_class(Compt_id cpid) 
  {
    auto it = _cptclasses.find(cpid);
    return it != end(_cptclasses) ? &it->second : nullptr;
  }
};

