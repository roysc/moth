// -*- coding: utf-8 -*-
// condition.hh
#pragma once

#include <functional>
#include "util/io.hh"

#include "basic.hh"
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
  using Oper = expr::Operation;
  Oper _oper;
  v<Compt_addr> _args;

public:
  Condition(Oper op, v<Compt_addr> as):
    _oper(op), _args(as)
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

  template <class Ch,class Tr>
  friend 
  std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, const Condition& c
  ) 
  {
    util::print_to(out, "(\"", c._oper, "\", ", c._args, ')');
    return out;
  }
};
