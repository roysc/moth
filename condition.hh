// -*- coding: utf-8 -*-
// condition.hh
#pragma once

#include <functional>
#include "util/io.hh"

#include "event.hh"
#include "expression.hh"
namespace stmt
{
struct Statement;
}

// TODO: rename to Trigger
// Condition:
// c = Condition()
// r = c()
//   => Event(...) -- condition is satisfied
//   => None       -- not
struct Condition
{
  using Expr = expr::Expr;
  using Stmt = stmt::Statement;
protected:
  uptr<Expr> _expr;
  uptr<Stmt> _stmt;

public:
  Condition(Expr* ex, Stmt* st):
    _expr(ex), _stmt(st) {}
  // Condition(uptr<Expr> ex, uptr<Stmt> st):
  //   _expr(move(ex)), _stmt(move(st)) {}

  // ~~~ m a g i c ~~~
  // Here shall happen the magic
  // Conditions ideally are evaluated in as isolated a context as possible
  // Condition works as a closure of several Component instances (fields)
  // references, and a (unary or binary|TODO: N-ary) relationship between them
  // * perhaps analogous to an entity whose components are field references.
  // 
  uptr<Event> operator()()
  {
    uptr<Event> evtp;

    // result data refers to child expression
    // recursively evaluate
    auto result = _expr->eval();
    LOG_(debug)("=> ", result);
    
    if (result.get<data::Bool>().value) {
      evtp.reset(new Event(_stmt.get()));
    }
    return evtp;
  }
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Condition& c)
  {
    out << util::concat("Cond[", *c._expr, ':', *c._stmt, ']');
    return out;
  }

};
using Cond_ptr = Condition*;
