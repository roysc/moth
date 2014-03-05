// -*- coding: utf-8 -*-
// condition.hh
#pragma once

#include <functional>

#include "basic.hh"
#include "err.hh"
#include "expression.hh"


// Condition:
// """Trigger object
// 
// c = Condition()
// r = c()
//   => Event(...) -- condition is satisfied
//   => None       -- not
// """

// 
struct Condition
{
protected:
  // Field_set _fields;
  Expression _expr;
  v<Compt_addr > _args;

public:
  Condition(Expression ex, v<Compt_addr> as):
    _expr(ex), _args(as)
  { }

  // Here shall happen the magic
  // Conditions ideally are evaluated in as isolated a context as possible
  uptr<Event> operator()()
  {
    // ~~~ m a g i c ~~~
    // Condition works as a closure of several (two?) instance (Field) references,
    // and a (binary?) relationship between them
    //
    // perhaps analogous to an entity whose components are field references.
    
    // An event is simple;
    // 
    
    return {};
  }
};
