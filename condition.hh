// -*- coding: utf-8 -*-
// condition.hh
#pragma once

#include <functional>
#include "util/io.hh"

// #include "basic.hh"
#include "statement.hh"
#include "event.hh"
#include "entity.hh"
#include "operation.hh"

// Condition:
// """Trigger object
// 
// c = Condition()
// r = c()
//   => Event(...) -- condition is satisfied
//   => None       -- not
// """
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
struct EFun: Expr {
  using Args_own = v<uptr<Expr> >;
  using Args = v<Expr*>;
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

// 
struct Condition
{
  using Expr = expr::Expr;
  using Stmt = stmt::Statement;
  uptr<Expr> _expr;
  uptr<Stmt> _stmt;
  // Expr _expr;
  // Stmt _stmt;
protected:

public:
  Condition(Expr* ex, Stmt* st):
    _expr(ex), _stmt(st) {}
  // Condition(const Expr& ex, const Stmt& st):
  //   _expr(ex), _stmt(st) {}

  // Here shall happen the magic
  // Conditions ideally are evaluated in as isolated a context as possible
  uptr<Event> operator()()
  {
    // ~~~ m a g i c ~~~
    // Condition works as a closure of several (two?) instance (Field)
    // references, and a (binary?) relationship between them
    // * perhaps analogous to an entity whose components are field references.
    
    // An event is simple;
    // auto kind = _stmt.kind();
    uptr<Event> evtp;

    // One way to do this:
    // result data refers to child expression
    // recursively evaluate

    // for now, just impl. end condition
    // (Data) <- eval()
    auto result = _expr->eval();
    data::Bool res_bool;
    result.set(res_bool);
    
    if (res_bool.value) {

      evtp.reset(new Event(_stmt.get()));
    }
    return evtp;
  }
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Condition& c)
  {
    out << util::concat("Cond[if(", *c._expr, "): ", *c._stmt, ']');
    return out;
  }

};
using Cond_ptr = Condition*;
