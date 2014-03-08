// -*- coding: utf-8 -*-
// expression.hh
#pragma once

#include <functional>
#include <utility>
#include <ostream>

#include "util/io.hh"

#include "basic.hh"
#include "operation.hh"

struct Compt_addr: pair<Entity_ptr, Compt_id>
{
  using pair::pair;
  Data_ptr operator()() const;
};

namespace expr
{
// using Cptrs = v<Compt_ptr>;
// using Cptaddrs = v<Compt_addr>;
// using DataIndex = pair<Data, Compt_addr>;
using Tbl_key = pair<OpType, dtype::T>;
using Eval_fn = fn<Data(v<Data>)>;
const m<Tbl_key, Eval_fn>& eval_fn_tbl();

// expression types
struct Expr {
  virtual Data eval() const = 0; 
  virtual dtype::Tag result_of() const = 0;
  virtual string to_string() const = 0;

  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Expr& o)
  {
    util::print_to(out, o.to_string());
    return out;
  }
};

// literal
struct ELit: Expr {
  Data value; 
  Data eval() const override;
  dtype::Tag result_of() const override;
  string to_string() const override {return value.to_string();}
};
// function/operator
using Args_own = v<uptr<Expr> >;
using Args = v<Expr*>;
struct EFun: Expr {
  Operation oper; 
  dtype::Tag dt_res;
  Args_own args;
public:
  EFun(Operation, Args);
  Data eval() const override;
  dtype::Tag result_of() const override;
  string to_string() const override;
};
// compt. value reference
struct ERef: Expr { 
  Compt_addr addr;
public:
  ERef(Compt_addr ca): addr(ca) {}
  Data eval() const override;
  dtype::Tag result_of() const override;
  string to_string() const override;
};
} // namespace expr
