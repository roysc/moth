// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

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
    
  //   # An event need only see its operands
  //   # It represents an expression, symbolically
  //   # Can't be too complicated
  // #
  // # met conditions trigger events
  // # create set of Events
  void run_events()
  {
    LOG_PUSH_(lrun)(__PRETTY_FUNCTION__);

    // # The 0-condition
    // # meet this to end the game (just a bool in primary System)
    LOG_TO_(info, lrun)("setting end condition");
    auto end_cond = _controlctx->end_cond();

    LOG_TO_(info, lrun)("setting conditions");
    auto conds = _controlctx->conditions();

    // show initial conditions
    v<string> condstrs;
    transform(begin(conds), end(conds), back_inserter(condstrs),
              [](Cond_ptr c) {return util::concat(*c);});
    LOG_TO_(debug, lrun)("conditions: ", condstrs);

    // Create a thread pool, and array of event results, same size as condv
    // for now just a thread vector
    LOG_TO_(info, lrun)("starting eval");

    // transform conditions... read-only here.
    v<uptr<Event> > evtv;
    for (auto&& cond: conds) {
      LOG_TO_(debug, lrun)("placing condn. for eval");
      evtv.push_back((*cond)());
    }
    
    LOG_TO_(debug, lrun)("condition eval finished");

    LOG_TO_(debug, lrun)("queueing events");
    // queue event results, skip duds
    for (auto&& e: evtv) {
      if (!e) continue;

      // If threading, scheduling events is the tricky part
      // queue or some structure that is friendly to concurrency
      
      // e->execute()
    }

    // # State at end
    // # self._conditions = uneval'd conditions (may be some met)
    // # self._entities = 
    // # events = last delivered events
  }
};
