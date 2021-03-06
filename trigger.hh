// -*- coding: utf-8 -*-
// trigger.hh
#pragma once

#include <functional>
#include "util/io.hh"

#include "log.hh"
#include "event.hh"
#include "expression.hh"
#include "statement.hh"

// TODO: rename to Trigger
// Trigger:
// c = Trigger()
// r = c()
//   => Event(...) -- condition is satisfied
//   => None       -- not
struct Trigger
{
  using Expr = expr::Expr;
  using Stmt = stmt::Statement;
protected:
  uptr<const Expr> _expr;
  // Entity* _ent;                 // the "bound" entity
  Stmt _stmt;

public:
  Trigger(const Expr* ex, Stmt st):
    _expr(ex), _stmt(st) {}

  // ~~~ m a g i c ~~~
  // Here shall happen the magic
  // Conditions ideally are evaluated in as isolated a context as possible
  // Trigger works as a closure of several Component instances (fields)
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
    
    if (result.at<data::Bool, 0>()) {
      // bind statement to entity
      evtp.reset(new Event(_stmt));
    }
    return evtp;
  }
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Trigger& c)
  { return out << "Trigger{" << *c._expr << ":\"" // << c._stmt
               << "?" << "\"}"; }

};
using Cond_ptr = Trigger*;
