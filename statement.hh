// -*- coding: utf-8 -*-
// statement.hh
#pragma once

#include "log.hh"
#include "typedefs.hh"

namespace stmt
{
struct Statement
{
protected:
  string _name;
  
public:
  Statement(string n): 
    _name(n)
  {}

  string name() const {return _name;};
  // EventKind kind() const {return _kind;};

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
  void execute() override
  {
    // set target data
    LOG_(debug)(__PRETTY_FUNCTION__);
    // auto cp = _cpadr.first->get(_cpadr.second);
    // *cp = _data;
    
  }
};

struct EndGame: Statement
{
public:
  EndGame(): Statement("_game_over_") {}
  void execute() override
  {
    /* delete everything? */
  }
};
}
