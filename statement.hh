// -*- coding: utf-8 -*-
// statement.hh
#pragma once

#include "log.hh"
#include "typedefs.hh"

#include "expression.hh"
// #include "controlctx.hh"

struct Entity;
struct ControlCtx;

namespace stmt
{
// it's important that the Statement logic be easily expressable
// however, it also need to be clear
// so, expressions should be short to make both goals easier
// So a statement is thought of as always:
// spawning an entity, changing one, or destroying one
//
// can chain them together in events
struct Statement
{
protected:
  string _name;
  
public:
  Statement(string n): 
    _name(n)
  {}

  virtual string name() const {return _name;};
  virtual bool fatal() const {return false;}
  virtual string to_string() const = 0;
  virtual void execute(ControlCtx&, Entity*) const = 0;
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Statement& c)
  {return out << c.to_string();}
};

struct Update: Statement
{
  Compt_id _cpid;
  uptr<expr::Expr> _expr;
public:
  Update(string n, Compt_id ci, expr::Expr* e):
    Statement("update"), _cpid(ci), _expr(e) {}
  void execute(ControlCtx& ctx, Entity* ent) const override;
  string to_string() const
  {
    return _name + util::concat_pair(_cpid, *_expr);
  }
};

struct Spawn: Statement
{
  using Statement::Statement;
  void execute(ControlCtx&, Entity*) const override {}
  string to_string() const {return _name;}
};

// signals
struct Signal: Statement
{
  using Statement::Statement;
};

struct Halt: Signal
{
public:
  Halt(): Signal("_halt_") {}
  void execute(ControlCtx& ctx, Entity*) const override;
  string to_string() const {return _name;}
  bool fatal() const override {return true;}
};
}
