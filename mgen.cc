// -*- coding: utf-8 -*-
// mgen.cc

#include "util/range/map.hh"

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
  read_cpts(cpts_js? *cpts_js : THROW_(Invalid, "components node"));
  read_ents(ents_js? *ents_js : THROW_(Invalid, "entities node"));
}

void
ModelGen::read_cpts(Json cpts_js)
{
  LOG_PUSH_(lcpts)(__func__);
  
  // Create CptClass
  auto make_cpt = [this, &lcpts]
    (string n, dtype::T dt) -> Compt_id {
    
    auto cpid = fresh_id();
    // LOG_PUSH_(info)(
    LOG_PUSH_TO_(lmk, lcpts)(
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
    return cpid;
  };

  // process components
  for (auto&& c: cpts_js) {

    auto name = c.second.get<string>("name");

    { // check for confusion
      auto itc = _ctrl_cpts.find(name);
      if (itc != end(_ctrl_cpts))
        LOG_(warning)("builtin \"", name, "\" redefined (ignoring)");
    }

    // get the control specs for the component
    auto ctrl = c.second.get_optional<string>("control");
    if (ctrl) {
      // specifies some kind of input
      // {target: "compt. changed", kind: "type of input"}
    }
    
    // get type(s)
    auto tp = c.second.get_child("type");

    // type could be value, or dict.
    // for value, define component
    // boost treats {} as ("", ...)
    auto val = tp.get_value<string>();
    if (!val.empty()) {
      dtype::T dt = dtype::from_string(val);
      make_cpt(name, dt);
    }
      
    // no children; it's an empty {}
    else if (tp.empty()) {
      make_cpt(name, dtype::ty_N);
    }
      
    // has multiple child components
    else {
      size_t i{};
      for (auto&& child: tp) {
        auto ch_name = child.first;
        // boost sets key to "" for json arrays
        // in that case, treat it as a tuple type, name with int. index
        if (ch_name.empty()) {
          ch_name = std::to_string(i);
        }
          
        auto ch_val = child.second.get_value<string>();
        auto dt = dtype::from_string(ch_val);
        // LOG_TO_(info, lcpts)(name, " child: ", ch_name);

        make_cpt(util::concat(name, '.', ch_name), dt);
      }
    }
  } // processed components
}

void
ModelGen::read_ents(Json ents_js)
{
  // auto make_ent = [this](string nm, vector<Compt_id> cnts) {};
  
  // process entities
  for (auto&& e: ents_js) {
    auto name = e.second.get<string>("name");
    auto cpts = e.second.get_child("contains");
    for (auto&& cpt: cpts) {
      auto contents = util::range::map<vector<Compt_id> >(
        cpts, [&](Json::value_type& c) -> Compt_id {
          return find_class(cpt.second.get_value<string>());
        }
      );
      // make_ent(name, contents);
    }
  }
}
