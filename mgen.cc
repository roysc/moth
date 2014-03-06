// -*- coding: utf-8 -*-
// mgen.cc

#include "err.hh"
#include "log.hh"

#include "mgen.hh"

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
ModelGen::ModelGen(Json js)
{
  LOG_PUSH_(l)(__PRETTY_FUNCTION__);

  // js = json.load(fp)
  LOG_TO_(info, l)("loading JSON");
    
  auto cpts_js = js.get_child_optional("components");
  auto ents_js = js.get_child_optional("entities");

  // Create CptClass
  auto make_cpt = [&l, this](string n, dtype::T dt) -> Compt_id {
    LOG_PUSH_TO_(lmk, l)("creating: ", n, ": ", dtype::to_string(dt));

    auto cpid = fresh_id();
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

  // process components
  for (auto&& c: cpts_js? *cpts_js : Json()) {
    // name. easy.
    auto name = c.second.get<string>("name");

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
        LOG_TO_(info, l)("single-type");
        auto dt = dtype::from_string(val);
        make_cpt(name, dt);
      }
      // no children; it's an empty {}
      else if (tp.empty()) {
        LOG_TO_(info, l)("empty-type");
        make_cpt(name, dtype::ty_N);
      }
      // has multiple child components
      else {
        LOG_TO_(info, l)("multi-type");
          
        for (auto&& child: tp)
        {
          auto ch_name = child.first;
          auto ch_val = child.second.get_value<string>();
          // LOG_TO_(info, l)("type node: \"", ch_val, '"');

          auto dt = dtype::from_string(ch_val);
          // LOG_TO_(info, l)(name, " child: ", ch_name);

          auto mkname = util::concat(name, '.', ch_name);
          make_cpt(mkname, dt);
        }
      }
    } // get types
  } // process components

  // set switch component (end condition result)
  auto itc = find_if(
    begin(_cptclasses), end(_cptclasses), 
    [](CptClasses::value_type v) {return v.second.name() == konst::end_cond_name;}
  );
  assert(itc == end(_cptclasses) && "end component already defined");
    
  auto cpid = make_cpt(konst::end_cond_name, dtype::ty_bool);
  _switch_cptid = cpid;

  // entities
  for (auto&& c: ents_js? *ents_js : Json()) {
    
  }
}

