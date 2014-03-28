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

CptPath::CptPath(const string& s):
  _parts{}
{
  namespace rx = std;
  // = boost::regex;
  // static const rx::regex parts_regex;

  if (s.empty()) return;
  
  vector<string> p;
  bool r = parse_cptpath(s, p);
  // rx::smatch m;
  // r = regex_match(begin(s), end(s), m, parts_regex);
  if (!r) THROW_(Invalid, s);
  
  _parts = p;
}

Ctx::Ctx(ModelGen& mg, vector<Specs > cpt_specs):
  // _dtypes(dtype::_meta_info),
  _dtype_names(dtype::_meta_names)
{
  map<Compt_id, CptDefn> cpt_defns;
  CptNames cpt_names;
  CptAggrs cpt_aggrs;

  auto create_basic = [&] (CptPath path, CptDefn cdef) -> Compt_id {
    LOG_PUSH_(lbc)("create_basic(", path, ",", cdef, ")");
    // create a basic component
    auto cpid = mg.fresh_id();
        
    // have to do definition last
    //   names can refer to specs names
    //   ref is a dependent type
    auto rt = cpt_defns.emplace(cpid, cdef);
    ASSERT_(rt.second, "cpt defn insert: ", cpid);

    auto rn = cpt_names.emplace(cpid, path);
    ASSERT_(rn.second, "cpt name insert: ", path);

    return cpid;
  };
  
  // translate dtype specs to meta data
  // evaluate specs, recursing into aggregates
  fn<vector<Compt_id>(Specs, CptPath)> create_aggr;
  create_aggr = [&](Specs cpspecs, CptPath prefix) -> vector<Compt_id> {
    LOG_PUSH_(lcra)("create_aggr(", cpspecs, ',',  prefix,')');

    const auto tpmems = cpspecs.members();
    const CptPath bpath{cpspecs.name()};
    const auto fpath = prefix + bpath;
    
    // collect ids for aggregate members
    vector<Compt_id> ret;
    for (auto&& p: tpmems) {
      string tpk, tpv;
      tie(tpk, tpv) = p;

      // could be a dtype, but defined inside another defn
      // or an aggregate...
      auto itb = cpt_aggrs.find(tpk);
      if (itb != end(cpt_aggrs)) {
        for (auto&& c: itb->second)
          ret.push_back(c);
      }
      else {
        LOG_(debug)("not in cpt_aggrs: ", tpk);
        // base case: 1 dtype member, "basic" component
        CptDefn cdef; bool r;
        if ((r = parse_data(tpv, cdef))) {
          LOG_(debug)("=> success: ", cdef);
          auto cpid = create_basic(bpath + tpk, cdef);
          ret.push_back(cpid);
        }
        else {
          LOG_(dtype)("=> failed!");
          // not dtype, not defined yet. look up in specs
          LOG_(debug)("looking up type in specs: ", tpv);

          auto itspec = find_if(
            begin(cpt_specs), end(cpt_specs),
            [=](Specs s){return tpv == s.name();}
          );
          
          // found in specs
          if (itspec != end(cpt_specs)) {
            LOG_(debug)("=> found");
            for (auto&& c: create_aggr(*itspec, fpath + tpk))
              ret.push_back(c);
            
          }
          else {          
            // compt name isn't in the spec, nothing to be done
            // THROW_(Not_found_T<decltype(p)>, p);
            THROW_(Not_found, tpv);
          }
        }
      }
    } // for tpmems
      
    return ret;
  };

  // iterate and recurse
  for (auto&& s: cpt_specs) {
    auto aggr = create_aggr(s, {});
    auto r = cpt_aggrs.emplace(s.name(), aggr);
    // ASSERT_(r.second, "aggr. insert: ", *r.first);
    if (!r.second) ASSERT_("aggr. insert failed: ", *r.first);
  }
  LOG_SHOW_(cpt_specs);
  LOG_SHOW_(cpt_names);
  LOG_SHOW_(cpt_aggrs);
            
  _cpt_names = cpt_names;
  _cpt_aggrs = cpt_aggrs;

  // translate to basic types
  for (auto&& e: cpt_defns) {
    LOG_PUSH_(lcdf)("CptDefn: ", e.first, " ", get_name(e.first));
    
    auto& init = get<0>(e.second);
    string dep = get<1>(e.second);
    // a dependent type?
    if (!dep.empty()) {
      // LOG_(debug)("  depends on \"", dep, '"');

      auto it = cpt_aggrs.find(dep);
      if (it == end(cpt_aggrs)) THROW_(Not_found, dep);
      auto depids = it->second;

      // set init
      init.set<data::Ref>(Compt_mult{0,depids});
    }

    LOG_(debug)("=> ", init);
    // store type
    _cpt_types.emplace(e.first, init);
  }

  // LOG_SHOW_(_cpt_types);
}

// using Str_it = string::iterator;
bool Ctx::parse_data(string str, CptDefn& out) const
{
  LOG_PUSH_(lprs)(__func__, "(\"", str, "\")");

  using qi::int_;
  using qi::double_;
  using qi::lit;
  using It = string::iterator;
  
  Rule<It> r_data;

  qi::symbols<char, dtype::T> r_dt_sym;
  for (auto&& e: _dtype_names)
    r_dt_sym.add(e.second, e.first);

  auto cpt_sym = r_sym;
  
  r_data =
    lit("R") [ (                // unbound R
      [&] { get<0>(out) = Data{dtype::ty_float}; }
    )] >> -(                    // R [a, b]
      ('[' >> (double_ % ',') >> ']') [(
          [&] (vector<double> v) {
            ASSERT_EQ_(2, v.size(), "realm args");
            Data r{dtype::ty_rcont};
            r.set<data::Real_rlm>(0, v.at(0), v.at(1));
            get<0>(out) = r;
          })]
    ) |
    
    lit("Z") [ (                // unbound Z
      [&] { get<0>(out) = Data{dtype::ty_int}; }
    )] >> -(                    // Z [a, b]
      ('[' >> (int_ % ',') >> ']') [(
          [&] (vector<int> v) {
            ASSERT_EQ_(2, v.size(), "realm args");
            Data r{dtype::ty_rdisc};
            r.set<data::Int_rlm>(0, v.at(0), v.at(1));
            get<0>(out) = r;
          })]
    ) |
    
    // ref (cptsym)
    lit("ref") >> ('(' >> cpt_sym >> ')') [(
      [&] (string s) {
        get<0>(out) = Data{dtype::ty_ref};
        get<1>(out) = s;
      }
    )] |
    // set(cptsym)
    lit("set") >> '(' >> cpt_sym >> ')' |

    // dtype name
    r_dt_sym // [([&](dtype::T dt){get<0>(out) = Data{dt};})]

    // component name
    // cpt_sym
    ;

  auto f = begin(str), l = end(str);
  bool r = phrase_parse(f, l, r_data, ascii::space);
  
  if (f != l) return {};
  return r;
}

}
