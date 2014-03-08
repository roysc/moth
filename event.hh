// -*- coding: utf-8 -*-
// event.hh
#pragma once

#include "log.hh"
#include "err.hh"

#include "basic.hh"
#include "statement.hh"
#include "component.hh"

// Event
// contains a statement (chain) to be evaluated
// * these are used to define behavior
// * access data of all involved components (Agent, System, Object)
// * create/destroy Entities
// * ?
// 
// example events:
//
// preference made
// q <- e.decide(p)
// 
// child created
// e.spawn(e2) -> ec
//
struct Event
{
  using Stmt = stmt::Statement;
protected:
  uptr<Event> _next;

public:
  Event(const Stmt* st, Event* n = {}):
    _next(n) {}
};
