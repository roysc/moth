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

using Statement = fn<bool(ControlCtx&)>;

// signals
struct Signal
{
  virtual bool fatal() const {return false;}
};

struct Halt: Signal
{
  bool fatal() const override {return true;}
};

namespace builder
{
// same as expression wrapper

inline Statement operator<<(Compt_addr r, Data v)
{
  return [=](ControlCtx&) {
    r().set(v); return true;
  };
}
template <class T>
inline Statement incr(Compt_addr r, T v)
{
  return [=](ControlCtx&) {
    r().at<data::Int, 0>() += v;
    return true;
  };
}

inline Statement halt()
{
  return [](ControlCtx&) {throw Halt{}; return true;};
}
}
}
