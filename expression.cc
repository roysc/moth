// -*- coding: utf-8 -*-
// trigger.cc

#include "expression.hh"

#include "util/range/algo.hh"

#include "log.hh"
#include "entity.hh"

namespace expr
{
dtype::T ERef::result_of() const {return _addr().dtype();}
string ERef::to_string() const 
{
  return util::concat("~", eval());
}

// function/op
EFun::EFun(Operation o, Args as):
  _oper(o), _args{}
{
  for (auto&& a: as) _args.emplace_back(a);

  // type-check arguments
  ASSERT_EQ_(_oper.arity(), _args.size(), "arity");
  auto dtags = _oper.arg_dtags();
  for (size_t i{}; i < _args.size(); ++i) { 
    auto dtr = _args[i]->result_of();
    // subtyping relationship, {dtr <: dtags}
    DTYPE_CHECK_(dtags[i], dtr);
  }
}
string EFun::to_string() const
{
  vector<string> argstrs;
  for (auto&& e: _args)
    argstrs.push_back(e->to_string());
  return util::concat('(', _oper, ',', argstrs,')');
}

dtype::T EFun::result_of() const {return _oper.res_dtype();}

// * eval *

// literal
Data ELit::eval() const {return _value;}
dtype::T ELit::result_of() const {return _value.dtype();}

// reference
Data ERef::eval() const { return _addr(); }

// implementing overloads makes this messy. semantics will need
// more careful planning
Data EFun::eval() const
{
  LOG_PUSH_(leval)(__PRETTY_FUNCTION__);
  
  // recursively evaluate args
  // _args: vector<Expr*>
  // dtags: vector<DTag>
  auto dtags = _oper.arg_dtags();
  dtype::T dt_call = dtype::ty_N; // dtype to find overload

  vector<Data> evalled;
  for (size_t i{}; i < _args.size(); ++i)
  {
    Data res = _args.at(i)->eval();
    evalled.push_back(res);
    // type check
    auto dt = res.dtype();
    // do an after-eval type/sanity check
    // ASSERT_EQ_(dtags[i], dtype::tag_of(dt), "Wrong arg type during eval()");
    
    // hack. need to identify overload, based on actual arg types
    // but for now they must all be the same. so take one
    if (i==0) dt_call = dt;
  }
  // LOG_SHOW_TO_(evalled, leval);

  FnTbl_key k{_oper.optype(), dt_call};
  auto it = eval_fn_tbl().find(k);
  if (it != end(eval_fn_tbl())) {

    // * call function *
    return it->second(evalled);
  } else {
    LOG_(warning)("No operation defined for ", k);
    // return nil for dtype
    return {result_of()};
  }
}

// eval functions
const map<FnTbl_key, Eval_fn>& eval_fn_tbl()
{
#define EXPR_FNTBL_ENTRY_BINP_(op_, opsym_, ATy_, RTy_)         \
  {{OpType::op_, data::d_type<data::ATy_>()},                  \
      [](vector<Data> as) {                                          \
        LOG_(debug)(OpType::op_, ": " #ATy_ " -> " #RTy_);      \
        auto r0 = as.at(0).at<data::ATy_, 0>();                  \
        auto r1 = as.at(1).at<data::ATy_, 0>();                    \
        Data rd{data::d_type<data::RTy_>()};                \
        rd.set<data::RTy_>(r0 opsym_ r1);                     \
        return rd;                                              \
      }                                                         \
  }
#define EXPR_FNTBL_ENTRY_BINP_2X_(op_, opsym_)          \
  EXPR_FNTBL_ENTRY_BINP_(op_, opsym_, Int, Bool),       \
  EXPR_FNTBL_ENTRY_BINP_(op_, opsym_, Real, Bool)

  static const map<FnTbl_key, Eval_fn> _eval_fn_tbl = {
    EXPR_FNTBL_ENTRY_BINP_2X_(op_eq, ==),
    EXPR_FNTBL_ENTRY_BINP_2X_(op_ne, !=),
    EXPR_FNTBL_ENTRY_BINP_2X_(op_gt, >),
    EXPR_FNTBL_ENTRY_BINP_2X_(op_ge, >=),
    EXPR_FNTBL_ENTRY_BINP_2X_(op_lt, <),
    EXPR_FNTBL_ENTRY_BINP_2X_(op_le, <=),
    // not: bool -> bool
    {{OpType::op_not, dtype::ty_bool},
     [](vector<Data> as) {
       LOG_(debug)(OpType::op_not, ": Bool -> Bool");
       auto res = as.at(0).at<data::Bool, 0>();
       // return
       Data rd(dtype::ty_bool);
       rd.set<data::Bool>(!res);
       return rd;
     }
    },
  };
  
#undef EXPR_FNTBL_ENTRY_BINP_2X_
#undef EXPR_FNTBL_ENTRY_BINP_
  
  return _eval_fn_tbl;
}

namespace builder
{

}

}
