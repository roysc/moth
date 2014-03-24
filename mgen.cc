// -*- coding: utf-8 -*-
// mgen.cc

#include "mgen.hh"

#include "util/range/algo.hh"

#include "log.hh"
#include "assert.hh"

using cpt::Specs;
// using cpt::Ctx::CptType;

cpt::Ctx
ModelGen::make_cpts()
{
  LOG_PUSH_(lcpts)(__func__);

  auto cpts_js = _json.get_child_optional("components");

  // (name, [tuple members])
  // using Specs = pair<string, vector<string> >;
  vector<Specs> cpt_specs;
  
  // process
  for (auto&& c: cpts_js? *cpts_js : THROW_(Not_found, "components")) {

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

    // type:=
    //  t                       // single basic type 
    //  {"a": t0, "b": t1, ...} // named tuple
    //  [t0, t1, ...]           // shortcut for {"t0": t0, "t1": t1, ...}
    
    // for basic, define component
    auto val = tp.get_value<string>();
    if (!val.empty()) {
      // val is present, so it's a single type
      cpt_specs.push_back({name, {{"0", val}}});
      ASSERT_(tp.empty(), "boost::ptree sanity check");
    }
      
    // has multiple child components
    else if (!tp.empty()) {
      // tuple member types
      Specs::Members mems;

      size_t ix{};
      for (auto&& child: tp) {
        auto ch_name = child.first;
        auto ch_val = child.second.get_value<string>();

        if (ch_val.empty())
          THROW_(Invalid, util::concat("empty type name (for \"", ch_name, "\")"));
          
        // boost sets key to "" for json arrays
        // in that case, name it with the index
        if (ch_name.empty())
          ch_name = std::to_string(ix++);
        
        mems.emplace_back(ch_name, ch_val);
      }
      cpt_specs.push_back({name, mems});
    }
          
    // no children; it's an empty {}
    else {
      LOG_(warning)("empty component \"", name, "\"");
      cpt_specs.push_back({name, {}});
    }
  } // processed json
  
  return cpt::Ctx(*this, cpt_specs);
}

void
ModelGen::read_ents()
{
  // auto make_entd = [this](string nm, set<Compt_id> cnts) {};
    
  auto ents_js = _json.get_child_optional("entities");
  
  // process entities
  for (auto&& e: ents_js? *ents_js : THROW_(Not_found, "entities")) {
    auto name = e.second.get<string>("name");
    auto cpts = e.second.get_child("contains");
    
    // auto contents = util::transform<set<Compt_id> >(
    //   util::view(cpts),
    //   [this](Json::value_type c) {return find_type(c.second.get_value<string>());}
    // );
    // _entdefs.emplace(name, contents);
  }
}
