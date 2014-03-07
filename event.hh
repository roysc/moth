// -*- coding: utf-8 -*-
// event.hh
#pragma once

#include "log.hh"
#include "err.hh"

#include "basic.hh"
#include "statement.hh"
#include "component.hh"
#include "entity.hh"

// Event
// represents a statement to be evaluated
// * the set of statement operations that are to be defined
//   is the set of behaviors that can be specified
// Events should have the ability to:
// * access data of all involved components (Agent, System, Object)
// * create/destroy Entities
// * anything else?
// 
// it's important that this logic be easily expressable
// howeever, it also need to be clear
// so, expressions should be short to make both goals easier
// So a statement is thought of as always:
// (spawning) an entity, (changing) one, or (destroying) one
//
// also could chain them together for exec. in sequence
struct Event;
// kinds of events
enum class EventKind: unsigned {spawn, update, destroy, N};

// Optional Event
using Event_ptr = ptr<Event>;

// example events:
//
// preference made
// q <- e.decide(p)
// 
// child created
// ec <- e.spawn(e2)
//
struct Event
{
  using Kind = EventKind;
  using Stmt = stmt::Statement;
protected:
  Compt_addr _cpadr;
  Data _data;
  uptr<Event> _next;

public:
  Event(const Stmt* st,
        Compt_addr cpa = {},
        Data data = {},
        Event_ptr n = {}):
    // _kind(k), 
    _data(data), 
    _cpadr(cpa),
    _next(n)
  {}
};
