// -*- coding: utf-8 -*-
// statement.hh
#pragma once

#include "log.hh"
#include "typedefs.hh"
#include "expression.hh"

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

  string name() const {return _name;};

  virtual void execute() = 0;

  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Statement& c)
  {
    out << "Statement(TODO)";
    return out;
  }
};

struct Update: Statement
{
  Compt_addr _cpadr;
  expr::Expr* _expr;
public:
  Update(string n, Compt_addr ca, expr::Expr* e):
    Statement(n), _cpadr(ca), _expr(e) {}
  void execute() override
  {
    LOG_(debug)(__PRETTY_FUNCTION__);
    // set target data
    auto res = _expr->eval();
    _cpadr()->set(res);
  }
};

// signals
struct Signal: Statement {using Statement::Statement;};

struct EndGame: Signal
{
public:
  EndGame(): Signal("_game_over_") {}
  void execute() override
  {
    LOG_(debug)(__PRETTY_FUNCTION__);
    // create warning entity?
    // delete everything?

    
    // 
  }
};
}
