// -*- coding: utf-8 -*-
// cptctx.hh

#include "cptctx.hh"

#include <algorithm>
#include <regex>
// #include <boost/regex.hpp>

#include "util/range/algo.hh"
#include "parse.hh"

#include "assert.hh"
#include "mgen.hh"

namespace cpt
{
const dtype::Dtypes Ctx::Ctx::_dtypes(dtype::_meta_info);

CptPath::CptPath(const string& s)
{
  namespace rx = std;
  // namespace rx = boost::regex;
  static const rx::regex parts_regex;
  vector<string> p;
  bool r;
  
  r = parse_cptpath(s, p);
  // rx::smatch m;
  // r = regex_match(begin(s), end(s), m, parts_regex);
  
  if (!r) THROW_(Invalid, s);

  _parts = p;
}

Ctx::Ctx(ModelGen& mg, vector<Specs > cpt_specs):
  // _dtypes(dtype::_meta_info),
  _dtype_names(dtype::_meta_names)
{
  map<Compt_id, string> cpt_defns;
  CptAggrs cpt_aggrs;
  
  // translate dtype specs to meta data
  // evaluate specs, recursing into aggregates
  fn<vector<Compt_id>(Specs, vector<string>)> create_aggr;
  create_aggr = [&](Specs cpspecs, vector<string> prefix) -> vector<Compt_id> {

    const auto name = cpspecs.name();
    const auto tpmems = cpspecs.members();

    // collect ids for aggregate members
    vector<Compt_id> ret;
    for (auto&& p: tpmems) {
      string tpk, tpv;
      tie(tpk, tpv) = p;

      // if tpk is empty, this is like:
      // Specs(name, {"", dt}) -> basic compt.
      if (tpk.empty()) {
        ASSERT_EQ_(1, tpmems.size(), "logical duct tape");
        tpk = name;
      }
      
      Data init;
      bool r;
      LOG_(debug)("Parsing: \"", tpv, '"');
      if ((r = parse_data(tpv, init))) {
        LOG_(debug)("=> success: ", init);
          
        // THROW_(Invalid, "basic component must refer to dtype (" + tpv + ")");
      
        // create a basic component
        auto cpid = mg.fresh_id();

        // have to do definition last
        //   names can refer to specs names
        //   ref is a dependent type
        // auto dt = init.dtype();
        // auto rt = cpt_defns.emplace(cpid, CptType(dt, init));
        auto rt = cpt_defns.emplace(cpid, tpv);
        ASSERT_(rt.second, "cpt defn insert: ", cpid);

        // append current to path
        // prefix.push_back(tpk);
        auto nm = util::io::join(prefix, '.');
        auto rn = _cpt_names.emplace(nm, cpid);
        ASSERT_(rn.second, "cpt name insert: ", nm);
        
        // CptAggrs::value_type e(tpv, {cpid});
        auto ra = cpt_aggrs.insert({tpv, {cpid}});
        ASSERT_(ra.second, "cpt aggr insert: ", tpv);

        // base case: 1 dtype member, "basic" component
        ret.push_back(cpid);
      }
        
      // next, check if created already
      // 
      // hairy here, a name can refer to a basic type or aggregate
      // we want absolute mapping for basic types
      // and another mapping for aggregates
      // 
      //   auto it = _cpt_names.find(tpk);
      //   if (it != end(_cpt_names)) {
      //     for (auto&& e: it->second)
      //       ret.push_back(e);
      //   }

        
      auto it = cpt_aggrs.find(tpk);
      if (it != end(cpt_aggrs)) {
        // add all those aggregates to this aggregate
        for (auto&& e: it->second)
          ret.push_back(e);
      }
      // not found, create it
      else {

        // look up type in specs
        auto itspec = find_if(
          begin(cpt_specs), end(cpt_specs),
          [=](Specs s){return tpv == s.name();}
        );
        if (itspec != end(cpt_specs)) {
          prefix.push_back(itspec->name());
          for (auto&& c: create_aggr(*itspec, prefix))
            ret.push_back(c);
        }
        
        // todo: implement rest of dtypes (refs, realms)

        LOG_SHOW_(cpt_specs);
        // LOG_SHOW_(cpt_aggrs);
        LOG_SHOW_(_cpt_names);
        
        // compt name isn't in the spec, nothing to be done
        THROW_(Not_found_T<decltype(p)>, p);
      }
    }
    return ret;
  };

  // iterate and recurse
  for (auto&& s: cpt_specs) {
    auto aggr = create_aggr(s, {s.name()});
    auto r = cpt_aggrs.emplace(s.name(), aggr);
    ASSERT_(r.second, "aggr. insert");
  }

  _cpt_aggrs = cpt_aggrs;
  // _cpt_types <- cpt_defns;
  
}

// using Str_it = string::iterator;
bool Ctx::parse_data(string str, Data& out) const
{
  LOG_PUSH_(lprs)(__func__);
  auto log_at = [&](string s) {
    return [&, s]{ LOG_(debug)("at: ", s); };
  };

  const vector<string> simple_dts{"bool", "int", "real", "str", "nil"};

  using It = string::iterator;
  Rule<It> r_simple;
  for (auto&& e: simple_dts) {
    auto it = _dtype_names.find(e);
    if (it == end(_dtype_names)) THROW_(Not_found, e);
    
    r_simple |= rule_dtype<It>(out, e, it->second) [log_at(e)];
  }

  // named references parsed
  // 
  vector<string> refs;
  
  auto r_data =
    r_simple |
    rule_realm<It>(out) |
    rule_ref<It>(out, [&](string s){refs.push_back(s);}) ;

  auto f = begin(str), l = end(str);
  auto r = phrase_parse(f, l, r_data, ascii::space);
  
  if (f != l) return {};
  return r;
}

}
