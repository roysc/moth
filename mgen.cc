// -*- coding: utf-8 -*-
// mgen.cc

#include "err.hh"
#include "log.hh"
#include "assert.hh"

#include "mgen.hh"

ModelGen::ModelGen(Json js):
  _next_id{}
{
  LOG_PUSH_(lctor)(__PRETTY_FUNCTION__);

  // js = json.load(fp)
  LOG_TO_(info, lctor)("loading JSON");
    
  auto cpts_js = js.get_child_optional("components");
  auto ents_js = js.get_child_optional("entities");

  // Create CptClass
  auto make_cpt = [this, &lctor](string n, dtype::T dt) -> Compt_id
    {
      auto cpid = fresh_id();
      LOG_PUSH_TO_(lmk, lctor)(
        "creating: ", n,
        " (id: ", cpid, ", type: ", dtype::to_string(dt), ')'
      );
    
      auto insr = _cptclasses.emplace(cpid, CptClass(this, n, dt));
      if (!insr.second)
        // THROW_(Runtime, "CptClass insertion failed");
        LOG_TO_(error, lmk)("CptClass insertion failed");
      else {
        auto cptr = &(insr.first->second);
        // double-index
        auto insr2 = _cpt_ids.emplace(cptr, cpid);
        if (!insr2.second)
          LOG_TO_(error, lmk)("Compt_id insertion failed");
      }
      // THROW_(Internal, "CptClass insertion failed");
      return cpid;
    };

  // (deprecate)
  // make builtins:
  //  switch component (end condition result)
  //  time counter
  // for (auto&& b: _builtins) {
  //   _ctrl_cpts.emplace(b.first, make_cpt(b.first, b.second));
  // }

  // process components
  for (auto&& c: cpts_js? *cpts_js : Json{}) {
    // name. easy.
    auto name = c.second.get<string>("name");
    { // check for confusion
      auto itc = _ctrl_cpts.find(name);
      if (itc != end(_ctrl_cpts))
        LOG_(warning)("builtin \"", name, "\" redefined (ignoring)");
    }

    // get the control specs for the component
    // not sure how this works yet
    auto sys = c.second.get_optional<string>("control");
    // if (!sys) THROW_(Invalid, "control");

    // get types
    // ----
    auto tp = c.second.get_child("type");

    // type could be value, or dict.
    // for value, define component
    auto val = tp.get_value<string>();
    if (!val.empty()) {
      // LOG_TO_(trivial, lctor)("single-type");

      dtype::T dt = dtype::from_string(val);
      make_cpt(name, dt);
    }
      
    // no children; it's an empty {}
    else if (tp.empty()) {
      // LOG_TO_(trivial, lctor)("empty-type");
      make_cpt(name, dtype::ty_N);
    }
      
    // has multiple child components
    else {
      // LOG_TO_(trivial, lctor)("multi-type");
      
      size_t i{};
      for (auto&& child: tp) {
        auto ch_name = child.first;
        // ptree sets key to "" for json arrays
        // in that case, treat it as a tuple type, name with int. index
        if (ch_name.empty()) {
          ch_name = std::to_string(i);
        }
          
        auto ch_val = child.second.get_value<string>();
        auto dt = dtype::from_string(ch_val);
        // LOG_TO_(info, lctor)(name, " child: ", ch_name);

        auto mkname = util::concat(name, '.', ch_name);
        make_cpt(mkname, dt);
      }
    }
    // get types
  } // process components

  // entities
  for (auto&& c: ents_js? *ents_js : Json{}) {
    // TODO
  }
}

