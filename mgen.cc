// -*- coding: utf-8 -*-
// mgen.cc

#include "err.hh"
#include "log.hh"
#include "assert.hh"

#include "mgen.hh"

ModelGen::ModelGen(Json js):
  _next_id{},
  _builtins{
    {"_end_", dtype::ty_bool},
    {"_time_", dtype::ty_int}
  }
{
  LOG_PUSH_(lctor)(__PRETTY_FUNCTION__);

  // js = json.load(fp)
  LOG_TO_(info, lctor)("loading JSON");
    
  auto cpts_js = js.get_child_optional("components");
  auto ents_js = js.get_child_optional("entities");

  // Create CptClass
  auto make_cpt = [&lctor, this](string n, dtype::T dt) -> Compt_id
    {
    auto cpid = fresh_id();
    LOG_PUSH_TO_(lmk, lctor)(
      "creating: ", n,
      " (id: ", cpid, ", type: ", dtype::to_string(dt), ')'
    );
    
    auto insr = _cptclasses.emplace(cpid, CptClass(this, n, dt));
    if (!insr.second)
      LOG_TO_(error, lmk)("CptClass insertion failed");
    else
    {
      auto cptr = &(insr.first->second);
      auto insr2 = _cpt_ids.emplace(cptr, cpid);
      if (!insr2.second)
        LOG_TO_(error, lmk)("Compt_id insertion failed");
    }
    // THROW_(Internal, "CptClass insertion failed");
    return cpid;
  };

  // make builtins:
  //  switch component (end condition result)
  //  time counter
  for (auto&& b: _builtins) {
    _ctrl_cpts.emplace(b.first, make_cpt(b.first, b.second));
  }

  // process components
  for (auto&& c: cpts_js? *cpts_js : Json()) {
    // name. easy.
    auto name = c.second.get<string>("name");
    { // check for confusion
      auto itc = _ctrl_cpts.find(name);
      if (itc != end(_ctrl_cpts))
        LOG_(warning)("builtin \"", name, "\" redefined (ignoring)");
    }

    // get the system specs for the component
    // not sure how this works yet
    auto sys = c.second.get_optional<string>("system");
    // if (!sys) THROW_(Invalid, "system");

    { // get types
      auto tp = c.second.get_child("type");
      // type could be value, or dict.
      // for value, define component
      auto val = tp.get_value<string>();
      if (!val.empty()) {
        // LOG_TO_(trivial, lctor)("single-type");

        auto dt = dtype::from_string(val);
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
          
        for (auto&& child: tp)
        {
          auto ch_name = child.first;
          // ptree sets this to "" for json arrays
          // handle using the type name as data name
          
          auto ch_val = child.second.get_value<string>();
          // LOG_TO_(info, lctor)("type node: \"", ch_val, '"');
          if (ch_name.empty())
          {
            // LOG_(debug)("");
            // needs to be unique this way
            ch_name = ch_val;
          }
          
          auto dt = dtype::from_string(ch_val);
          // LOG_TO_(info, lctor)(name, " child: ", ch_name);

          auto mkname = util::concat(name, '.', ch_name);
          make_cpt(mkname, dt);
        }
      }
    } // get types
  } // process components

  // entities
  for (auto&& c: ents_js? *ents_js : Json()) {
    // TODO
  }
}

