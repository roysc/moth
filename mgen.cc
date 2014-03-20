// -*- coding: utf-8 -*-
// mgen.cc

#include "util/range/algo.hh"

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
  LOG_SHOW_(_cptclasses);
  read_ents(ents_js? *ents_js : THROW_(Invalid, "entities node"));
}

void
ModelGen::read_cpts(Json cpts_js)
{
  LOG_PUSH_(lcpts)(__func__);

  // (name, [tuple members])
  vector<pair<string, vector<string> > > dtypes_read;
  
  // process
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

    // type:=
    //  t                       // single basic type 
    //  {"a": t0, "b": t1, ...} // named tuple
    //  [t0, t1, ...]           // shortcut for {"t0": t0, "t1": t1, ...}
    
    // for basic, define component
    auto val = tp.get_value<string>();
    // boost treats {} as ("", [...])
    if (!val.empty()) {
      dtypes_read.push_back({name, {val}});
      // pair<string, vector<string> > dtr(name, {val});
      // dtypes_read.push_back(dtr);
      // dtypes_read.emplace_back(name, {val});
      ASSERT_(tp.empty(), "boost::ptree sanity check");
    }
      
    // has multiple child components
    else if (!tp.empty()) {
      // tuple member types
      vector<string> mem_dts;
      for (auto&& child: tp) {
        auto ch_name = child.first;
        auto ch_val = child.second.get_value<string>();

        if (ch_val.empty())
          THROW_(Invalid, util::concat("empty type name (for \"", ch_name, "\")"));
          
        // boost sets key to "" for json arrays
        // in that case, name it with the type
        if (ch_name.empty())
          ch_name = ch_val;

        mem_dts.push_back(ch_val);
      }
      dtypes_read.emplace_back(name, mem_dts);
    }
          
    // no children; it's an empty {}
    else {
      LOG_(warning)("empty component \"", name, "\"");
      dtypes_read.push_back({name, {}});
    }
  } // processed json

  // type check, create dtype meta
  // can just initialize w/ basic types
  vector<dtype::Meta> dtypes_meta(dtype::_meta_info);
  
  // ensure all dtypes exist, or can be created
  for (auto&& dtnew: dtypes_read) {
    string name;
    vector<string> mem_dts;
    tie(name, mem_dts) = dtnew;
    // auto itnm = find_name(dtypes_read, );

    // // translate dtype specs to meta data:
    // // evaluate specs, recursing into aggregates
    // {
    //   // look up in 
    //   auto it = find_if(
    //     begin(dtypes_meta), end(dtypes_meta),
    //     [=](dtype::Meta m){return dtnm == m.name;}
    //   );

    //   // exists
    //   if (it != end(dtypes_meta)) {
        
        
    //   }
    //   // need to create
    //   else {
    //     // dtype::T dt = ?
    //     // auto sz = sum of member types
    //     // dtype::Meta m{dt, dtype::Tag::N, name, sz};
    //     // dtypes_meta.push_back(m);
    //   }
    // }
  }

  // process new dtype meta
  vector<pair<string, dtype::T> > cpts_new;
  for (auto&& d: dtypes_meta) {
    // if we are good
    // cpts_new.emplace_back(util::concat(name, '.', ch_name), dt);
    // cpts_new.emplace_back(name, dt);
    
  }
  
  for (auto&& c: cpts_new) {
    auto n = c.first;
    auto dt = c.second;
    
    auto cpid = fresh_id();
    
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
  };

}

void
ModelGen::read_ents(Json ents_js)
{
  // auto make_entd = [this](string nm, set<Compt_id> cnts) {};
  
  // process entities
  for (auto&& e: ents_js) {
    auto name = e.second.get<string>("name");
    auto cpts = e.second.get_child("contains");
    
    auto contents = util::map<set<Compt_id> >(
      util::view(cpts),
      [this](Json::value_type& c) {return find_cptclass(c.second.get_value<string>());}
    );
    _entdefs.emplace(name, contents);
  }
}
