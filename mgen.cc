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
  using DtypeSpecs = pair<string, vector<string> >;
  vector<DtypeSpecs> dtypes_read;
  
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

  // translate dtype specs to meta data:
  // evaluate specs, recursing into aggregates
  fn<dtype::Meta(DtypeSpecs)> create_dtype;
  create_dtype = // [&create_dtype, &dtypes_meta]
    [&](DtypeSpecs dtspecs) {
    
    // collect meta info for aggregate members
    vector<dtype::Meta> mems_meta;
    for (auto&& dtnm: dtspecs.second) {

      // look up in existing
      auto it = find_if(
        begin(dtypes_meta), end(dtypes_meta),
        [=](dtype::Meta m){return dtnm == m.name;}
      );

      // exists already
      if (it != end(dtypes_meta)) {
        mems_meta.push_back(*it);
      }
      // need to create
      else {
        // look up in specs
        auto itspec = find_if(
          begin(dtypes_read), end(dtypes_read),
          [=](DtypeSpecs dts){return dtnm == dts.first;}
        );
          
        // found, create meta
        if (itspec != end(dtypes_read)) {
          // fixme: wrong, meta should be created in full set
          mems_meta.push_back(create_dtype(*itspec));
        } else {
          // dtype name isn't in the spec, nothing to be done
          // THROW_(Not_found, dtnm);

          // fixme: workaround for missing basic type (ref)
          mems_meta.push_back({dtype::ty_N, dtype::Tag::N, dtnm, 0});
        }
      }
    }
      
    // meta info checks out. build new (meta) dtype from it
    dtype::T dt = fresh_id();
    // sz = sum of member types
    auto sz = accumulate(
      begin(mems_meta), end(mems_meta), size_t{},
      [](size_t a, dtype::Meta m1){return a + m1.size;}
    );

    // TODO: tag
    return dtype::Meta{dt, dtype::Tag::N, dtspecs.first, sz};
  };

  // ensure all dtypes exist, or can be created
  for (auto&& dtspec: dtypes_read) {
    auto meta = create_dtype(dtspec);
    // can do sanity check here
    dtypes_meta.push_back(meta);
  }

  // process new dtypes
  vector<pair<string, dtype::T> > cpts_new;

  for (auto&& d: dtypes_meta) {
    // if we are good
    // cpts_new.emplace_back(util::concat(name, '.', ch_name), dt);
    // cpts_new.emplace_back(name, dt);
    
    cpts_new.emplace_back(d.name, d.type);
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
