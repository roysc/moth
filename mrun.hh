// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

#include <queue>
#include <utility>
#include <algorithm>

#include "util/range.hh"

#include "log.hh"
#include "event.hh"
#include "condition.hh"
#include "cptclass.hh"
#include "controlctx.hh"

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

  // what does event handle?
  // Types:
  //   Evt(Force, Body -> BodyState)
  //   force: sys. component
  //   obj.: sys. entity
  //
  //   c= Cond(Body.pain > _)
  //   => e= Evt(Agent.Senses -> Agent.Senses)
  //   r = e() = (s > s.alarmed; s)
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
  void run_events()
  {
    LOG_PUSH_(lrun)(__PRETTY_FUNCTION__);

    EventQueue evtq;
    // # The 0-condition
    // # meet this to end the game (just a bool in primary System)
    LOG_TO_(info, lrun)("setting end condition");
    auto end_cond = _controlctx->end_cond();

    // Create a thread pool, and array of event results, same size as condv

    LOG_TO_(info, lrun)("setting conditions");
    auto conds = _controlctx->conditions();

    // show initial conditions
    v<string> condstrs;
    transform(begin(conds), end(conds), back_inserter(condstrs),
              [](Cond_ptr c) {return util::concat(*c);});
    LOG_TO_(debug, lrun)("conditions: ", condstrs);

    // Threadpool evalpool..?
    LOG_TO_(info, lrun)("starting eval");
    v<thread> evaljobs;
    v<uptr<Event>> evtv(conds.size());

    // iterate on conditions... read-only here.
    for (auto itc = begin(conds), last = end(conds); itc != last; ++itc)
    // for (auto&& cond: conds)
    {
      LOG_TO_(debug, lrun)("placing condn. for eval");

      auto ite = end(evtv) - LOG_SHOW_(distance(itc, last));
      evaljobs.emplace_back([=]{
      // evaluate condition, place result in array
          *ite = (**itc)();
      });
    }
    // TODO: threadpool or scheduling here
    for (auto&& th: evaljobs)
      th.join();

    LOG_TO_(debug, lrun)("condition eval finished");

    LOG_TO_(debug, lrun)("queueing events");
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
