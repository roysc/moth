// -*- coding: utf-8 -*-
// expression.hh
#pragma once

#include <functional>
#include <utility>
#include <ostream>

#include "util/io.hh"

#include "basic.hh"
#include "operation.hh"
// #include "entity.hh"

namespace expr
{
using FnTbl_key = pair<OpType, dtype::T>;
using Eval_fn = fn<Data(vector<Data>)>;
const map<FnTbl_key, Eval_fn>& eval_fn_tbl();

// expression types
struct Expr {
  virtual Data eval() const = 0; 
  virtual dtype::T result_of() const = 0;
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
  Data _value;
public:
  ELit(Data v): _value(v) {}
  Data eval() const override;
  dtype::T result_of() const override;
  string to_string() const override {return _value.to_string();}
};

// function/operator
struct EFun: Expr {
  using Args_own = vector<uptr<const Expr> >;
  using Args = vector<const Expr*>;
  // cpt::Ctx& _ctx;
  Operation _oper; 
  Args_own _args;

public:
  EFun(Operation, Args);
  Data eval() const override;
  dtype::T result_of() const override;
  string to_string() const override;
};

// compt. value reference
struct ERef: Expr { 
  Compt_addr _addr;
public:
  ERef(Compt_addr ca): _addr(ca) {}
  Data eval() const override;
  dtype::T result_of() const override;
  string to_string() const override;
  Compt_addr address() const {return _addr;}
};

namespace builder
{
// ADL-assistance class for operators
struct Ex
{
  const Expr* _expr;
  Ex(Compt_addr r): _expr(new ERef(r)) {}
  Ex(Data d): _expr(new ELit(d)) {}
  Ex(string o, EFun::Args as): _expr(new EFun(Operation(o), as)) {}
  // template <class... Es>
  // Ex(string o, const Es&... es): _expr(new EFun(Operation(o), {es...})) {}

  operator const Expr*() const {return _expr;}
};

inline
Ex ref(Compt_addr ca) {return Ex{ca};}

template <class D, class... Ts>
Ex lit(Ts&&... vs) { return Ex(Data::make<D>(std::forward<Ts>(vs)...)); }

inline Ex operator==(Ex a, Ex b) { return Ex("=", {a, b}); }
inline Ex operator!=(Ex a, Ex b) { return Ex("≠", {a, b}); }
inline Ex operator>(Ex a, Ex b) { return Ex(">", {a, b}); }
inline Ex operator<(Ex a, Ex b) { return Ex("<", {a, b}); }
inline Ex operator>=(Ex a, Ex b) { return Ex("≥", {a, b}); }
inline Ex operator<=(Ex a, Ex b) { return Ex("≤", {a, b}); }
}

} // namespace expr
