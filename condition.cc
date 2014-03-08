// -*- coding: utf-8 -*-
// condition.cc

#include "condition.hh"

namespace expr
{
// literal
Data ELit::eval() const {return value;}
dtype::Tag ELit::result_of() const {return dtype::tag_of(value.dtype());}

// reference
Data ERef::eval() const
{ // data ptr
  auto dptr = addr.first->get(addr.second);
  return *dptr;
}
dtype::Tag ERef::result_of() const
{
  auto dptr = addr.first->get(addr.second);
  return dtype::tag_of(dptr->dtype());
}
string ERef::to_string() const {return util::concat(addr);}

// function/op
EFun::EFun(Operation o, Args as):
  oper(o)
{
  for (auto&& a: as) args.emplace_back(a);

  // type-check arguments
  assert(oper.arity() == args.size());
  auto dtags = oper.arg_dtags();
  for (size_t i{}; i < args.size(); ++i)
  { 
    auto dtag = args[i]->result_of();
    assert(dtags[i] == dtag 
           && "Wrong argument dtype");
  }
}
string EFun::to_string() const
{
  v<string> argstrs;
  for (auto&& e: args)
    argstrs.push_back(e->to_string());
  return util::concat("(\"", oper, "\", ", argstrs,')');
}

dtype::Tag EFun::result_of() const {return oper.res_dtag();}

// implementing overloads makes this messy. semantics will need
// more careful planning
Data EFun::eval() const
{
  // recursively evaluate args
  // args: v<Expr*>
  // dtags: v<DTag>
  auto dtags = oper.arg_dtags();
  dtype::T dt_call{dtype::ty_N}; // dtype to find overload

  v<Data> evalled;
  for (size_t i{}; i < args.size(); ++i)
  {
    Data res = args.at(i)->eval();
    evalled.push_back(res);
    // type check
    auto dt = res.dtype();
    // do an after-eval type/sanity check
    assert((dtags[i] == dtype::tag_of(dt)) 
           && "Wrong arg type during eval()");
    
    // hack. need to identify overload, based on actual arg types
    // but for now they must all be the same. so take one
    if (i==0) dt_call = dt;
  }
  LOG_SHOW_(evalled);

  Tbl_key k{oper.optype(), dt_call};
  auto it = eval_fn_tbl().find(k);
  if (it != end(eval_fn_tbl())) {
    // call function
    return it->second(evalled);
  } else {
    LOG_(warning)("No operation implemented for ", k);
    // return nil
    return {};
  }
}

// eval functions
const m<Tbl_key, Eval_fn>& eval_fn_tbl()
{
  static const m<Tbl_key, Eval_fn>
    _eval_fn_tbl = {
    // not: bool -> bool
    {{OpType::op_not, dtype::ty_bool},
     [](v<Data> as) {
       LOG_(debug)(OpType::op_not, ": bool -> bool");
       // get
       data::Bool res;
       as.at(0).get(res);
       // return
       Data rd(dtype::ty_bool);
       rd.set(data::Bool{!res.value});
       return rd;
     }
    },
  };
  return _eval_fn_tbl;
}

}
