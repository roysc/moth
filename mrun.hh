// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

#include <utility>
#include <algorithm>

#include "util/range.hh"

#include "log.hh"
#include "event.hh"
#include "trigger.hh"
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
  bool run_events()
  {
    LOG_PUSH_(lrun)(__PRETTY_FUNCTION__);

    auto ctrl_ent = _controlctx->ctrl_entity();
    
    // The 0-condition, ends the game (bool in a builtin component)
    Compt_addr end_flag = {ctrl_ent, _controlctx->find_class("_end_")};
    Compt_addr tm_ctr = {ctrl_ent, _controlctx->find_class("_time_")};
    
    // transform conditions... read-only here.
    // Create a thread pool, and array of event results, same size as condv
    // for now just in sequence
    v<uptr<Event> > evtv;
    
    LOG_PUSH_(lcond)("evaluating triggers:");
    for (auto&& cond: _controlctx->triggers()) {    
      LOG_TO_(debug, lcond)(*cond);
      evtv.push_back((*cond)());
    }

    LOG_TO_(debug, lrun)("queueing events");
    // queue event results, skip duds
    for (auto&& e: evtv) {
      if (!e) continue;
      
      // If threading, scheduling events is the tricky part
      // queue or some structure that is friendly to concurrency
      
      e->happen(_controlctx);
    }

    // State at end:
    // triggers: for next tick
    // entities: persist; erase triggered by end condn.
    // events: should all be evaluated, unless early terminated

    // tick
    // this needs to work with a single ++
    data::Int tmct{tm_ctr()->get<data::Int>().value + 1};
    tm_ctr()->set(tmct);
    
    return end_flag()->get<data::Bool>().value;
  }
};
