// -*- coding: utf-8 -*-
// condition.hh
#pragma once

#include <functional>
#include "util/io.hh"

#include "basic.hh"
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
struct Expression
{
protected:
  // Field_set _fields;
  Operation _oper;
  v<Compt_addr> _args;

public:
  Expression(Operation op, v<Compt_addr> as):
    _oper(op), _args(as)
  { }

  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Expression& o)
  {
    util::print_to(out, "(", o._oper, ", ", o._args, ')');
    return out;
  }

  // E: {o: Op, as: [(e:Ent, c:Cpid)]}
  Data eval()
  { // pack as data pointers
    v<Compt_ptr> cptrs;
    for (auto&& arg: _args)
      cptrs.push_back(arg.first->get(arg.second));
    return _oper.eval(cptrs);
  }
};
} // namespace expr

// 
struct Condition: public expr::Expression
{
  using Expression::Expression;

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
}
;
