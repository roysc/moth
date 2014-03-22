// -*- coding: utf-8 -*-
// cptctx.hh

#include <algorithm>

#include "assert.hh"

#include "cptctx.hh"
#include "mgen.hh"

namespace cpt
{
const dtype::Dtypes Ctx::Ctx::_dtypes(dtype::_meta_info);

Ctx::Ctx(ModelGen& mg, vector<Specs > dtype_specs):
  // _dtypes(dtype::_meta_info),
  _dtype_names(dtype::_meta_names),
  _cpt_types{},
  _cpt_names{}
{
  // translate dtype specs to meta data
  // evaluate specs, recursing into aggregates
  fn<vector<Compt_id>(Specs, vector<string>)> create_compt;
  create_compt = [&](Specs dtspecs, vector<string> prefix) -> vector<Compt_id> {

    const auto name = dtspecs.name();
    const auto tpmems = dtspecs.members();
    
    // append this name to the path
    prefix.push_back(name);
    
    // base case: 1 dtype member, "basic" component
    if (tpmems.size() == 1) {
      auto mem = tpmems.at(0);
      
      auto it = _dtype_names.find(mem);
      if (it != end(_dtype_names)) {
        // create a basic component
        auto cpid = mg.fresh_id();
        auto r = _cpt_types.emplace(cpid, CptType{true, it->second});
        auto nm = util::join(prefix, '.');
        auto rn = _cpt_names.emplace(nm, cpid);
        CptAggrs::value_type e(mem, {cpid});
        auto ra = _cpt_aggrs.emplace(e);
        ASSERT_(rn.second, "cpt name insert: ", nm);
        // ASSERT_(ra.second, "cpt aggr insert");
        return {cpid};
      }
      // else THROW_(
      //   Invalid, "basic component must refer to dtype (" + tpmems.at(0) + ")"
      // );
      else return {};
    }
    
    // collect ids for aggregate members
    vector<Compt_id> ret;
    for (auto&& memname: tpmems) {
      // next, check if created already

      // hairy here, a name can refer to a basic type or aggregate
      // we want absolute mapping for basic types
      // and another mapping for aggregates
      // {
      //   auto it = _cpt_names.find(memname);
      //   if (it != end(_cpt_names)) {
      //     for (auto&& e: it->second)
      //       ret.push_back(e);
      //   }
      // }

      auto it = _cpt_aggrs.find(memname);
      if (it != end(_cpt_aggrs)) {
        for (auto&& e: it->second)
          ret.push_back(e);
      }
      // not found, create it
      else {
        // look up in specs
        auto itspec = find_if(
          begin(dtype_specs), end(dtype_specs),
          [=](Specs dts){return memname == dts.name();}
        );
        if (itspec != end(dtype_specs)) {
          for (auto&& e: create_compt(*itspec, prefix))
            ret.push_back(e);
        }

        // dtype name isn't in the spec, nothing to be done
        THROW_(Not_found, memname);
        
        // todo: implement rest of dtypes (refs, realms)
      }
    }
    return ret;
  };

  // iterate and recurse
  for (auto&& s: dtype_specs) {
    auto r = _cpt_aggrs.emplace(s.name(), create_compt(s, {}));
    ASSERT_(r.second, "aggr. insert");
  }
}
}
