// -*- coding: utf-8 -*-
// event.hh
#pragma once

#include "basic.hh"

// Event
// represents a statement to be evaluated
// * the set of statement operations that are to be defined
//   is the set of behaviors that can be specified?
// Events should have the ability to:
// * access data of all involved components (Agent, System, Object)
// * create new Entities
// * access other data?
// 
// it's important that this logic be easily expressable
// howeever, it also need to be clear
// so, expressions should be short to make both goals easier
// So a statement is thought of as always:
// (spawning) an entity, (changing) one, or (destroying) one
struct Event;
// kinds of events
enum class EventKind: unsigned {spawn, update, destroy, N};

// Optional Event
using Event_ptr = ptr<Event>;
using Event_opt = uptr<Event>;

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

  Event(Kind k, v<Compt_addr> args):
    _kind(k)
  {
    switch (k)
    {
    case Kind::spawn:
      // 
      break;
    case Kind::update:

      break;
    case Kind::destroy:

      break;
    default:
      {}
    }
  }

protected:
  Kind _kind;
  v<Compt_addr> _args;
};
