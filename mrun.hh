// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

#include <queue>
#include <utility>

#include "event.hh"
#include "condition.hh"
#include "cptclass.hh"
#include "controlctx.hh"

// "Contexts" for components needing centralized control
// eg. Location/coordinates
//     Physics
//     Information network
struct ControlCtx;

// Model interface
// class ModelRun
// {
// public:
//   virtual
//   ControlCtx* run() = 0;

// protected:        
//   virtual
//   void _run_events(ControlCtx*) = 0;
// };

// "Runnable model."
// # 1. instantiate systems
// # 2. start iterating:
// #   x = end condition
// #   until x()
// #     k = new conditions met
// #     e = yesterday's entities
// #     v = new events(k)
// #     e <- e + new entities(v)
// #     c <- update(c, v)
// #
class ModelRun
{
  ControlCtx* _controlctx;

public:
  ModelRun(ControlCtx* ctx): _controlctx(ctx) {}

  // Processing this queue
  // yields mutation
  // each event contains instructions to mutate their associated components
  // struct EventQueue : private std::queue<Event_ptr>
  // {
  //   ControlCtx* _controlctx;
  //   void push(Event_ptr e);
  // };
  // using EventQueue = std::queue<Event_ptr>;
  using EventQueue = std::queue<uptr<Event>>;

  void run_events()
  {
    // what does event handle?
    // Types:
    //   Evt(Force, Body -> BodyState)
    //   force: sys. component
    //   obj.: sys. entity
    //
    //   c= Cond(Body.pain > _)
    //   => e= Evt(Agent.Senses -> Agent.Senses)
    //   r = e() = (s -> s.alarmed; s)
    // 
    //   Cond(Agent.decision ~ go(_))
    //   => Evt(Loc -> Loc)
    //   r = e();
    //   r should produce a follow-up condition checking arrival,
    //     and checking path
    //
    //   Cond(Agent.smell ~ meat)
    //   => Evt(Agent -> Go)
    //   Cond(Agent.)
    //   Evt(Agent -> Agent)
    
    //   # An event need only see its operands
    //   # It represents an expression, symbolically
    //   # Can't be too complicated
    // #
    // # met conditions trigger events
    // # create set of Events


    // # The 0-condition
    // # meet this to end the game (just a bool in primary System)
    auto end_cond = _controlctx->end_cond();
    auto&& conds = _controlctx->conditions();
    EventQueue evtq;

    // Create a vector of threads, and of event results, same size as condv
    v<thread> evaljobs;
    v<uptr<Event> > evtv(conds.size());
    for (auto itc = begin(conds), last = end(conds); itc != last; ++itc)
    {
      auto ite = end(evtv) + distance(itc, last);
      evaljobs.emplace_back([=]{*ite = (**itc)();});
    }
    // TODO: threadpool or scheduling here
    for (auto&& th: evaljobs)
      th.join();

    // queue event results, skip duds
    for (auto&& e: evtv)
    {
      if (!e) continue;
      evtq.push(move(e));
    }

    // Scheduling events is the tricky part
    {
      // TaskSchedule sched{evtq};
      // ...?
    }

    // # State at end
    // # self._conditions = uneval'd conditions (may be some met)
    // # self._entities = 
    // # events = last delivered events
  }
};
