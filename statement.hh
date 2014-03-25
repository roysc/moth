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
  virtual void execute(ControlCtx&) const = 0;
  
  template <class Ch,class Tr>
  friend std::basic_ostream<Ch,Tr>& operator<<(
    std::basic_ostream<Ch,Tr>& out, 
    const Statement& c)
  {return out << c.to_string();}
};

struct Update: Statement
{
  // Compt_id _cpid;
  Compt_addr _caddr;
  uptr<const expr::Expr> _expr;
  
public:
  Update(Compt_addr ca, const expr::Expr* e):
    Statement("update"), _caddr(ca), _expr(e) {}
  void execute(ControlCtx& ctx) const override;
  string to_string() const
  {
    return _name + ": " + util::concat(_caddr);
  }
};

struct Incr: Update
{
  int _amt;
public:
  Incr(Compt_addr ca, int a): Update(ca, 0), _amt(a) {}
  void execute(ControlCtx& ctx) const override;
};

struct Spawn: Statement
{
  using Statement::Statement;
  void execute(ControlCtx&) const override
  {
    // TODO: create entity
  }
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
  void execute(ControlCtx& ctx) const override;
  string to_string() const {return _name;}
  bool fatal() const override {return true;}
};

namespace builder
{
// same as expression wrapper
struct St
{
  Statement* _stmt;
};

struct Ref_ { Compt_addr address; };
inline Ref_ ref(Compt_addr ca) {return Ref_{ca}; }
// inline St ref(Compt_addr ca) {return Ref_{ca}; }

Update* operator<<(Ref_ r, const expr::Expr* v);
Incr* operator+=(Ref_ r, int v);
// St operator<<(Ref_ r, const expr::Expr* v);
// St operator+=(Ref_ r, int v);

inline Halt* halt() {return new Halt;}
// inline St halt() {return St(new Halt);}
}
}
