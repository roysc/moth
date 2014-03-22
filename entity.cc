// -*- coding: utf-8 -*-
// entity.cc

#include "entity.hh"
#include "controlctx.hh"

// dereference
Data_ptr Compt_addr::operator()() const
{
  return first->get(second);
}

// constructed with set of classes
// need to "flatten" components
Entity::Entity(cpt::Ctx& cx, vector<Compt_id>  cpts):
  _ctx(cx)
{
  LOG_PUSH_(lctor)(__PRETTY_FUNCTION__);
  // "Each C in Cpts represents a component meta-type"

  map<Compt_id, dtype::T> dtypes;
  
  for (auto&& c: cpts) {
    // 
    auto tp = _ctx.get_type(c);
    if (tp) {
      // we want..? :
      //    .location -> (ref(real,real)) ?
      //    .phys.location -> '' ?
      //    .body.phys.mass ?
      //    .body.organs:* ?
      //    
      // ASSERT_EQ_(1, cpts.size(), "logic fail");
      auto r = _compts.emplace(c, _ctx.create_type(c));
      
      // Data d{cpts.at(0)};
    }
    else {
      THROW_(Internal, "Compt_id not found " + std::to_string(c));
    }
  }
  
  LOG_SHOW_TO_(_compts, lctor);
}

string Entity::get_name(Compt_id id) const
{
  return _ctx.get_name(id);
}

// // O(n)
// Compt_ptr Entity::find(const string& cn) 
// {
//   for (auto&& c: _compts)
//     if (_ctx.get_type(c.first)->name() == cn)
//       return &(c.second);
//   return {};
// }
