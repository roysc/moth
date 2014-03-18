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
// 1. instantiate controls
// 2. start iterating:
//   x = end condition
//   until x()
//     k = new conditions met
//     e = yesterday's entities
//     v = new events(k)
//     e <- e + new entities(v)
//     c <- update(c, v)
namespace run
{
// transform conditions... read-only here.
vector<uptr<Event> > eval_triggers(ControlCtx& ctx)
{
  vector<uptr<Event> > evtv;
  LOG_PUSH_(ltrig)(__func__);
  for (auto&& cond: ctx.triggers()) {    
    LOG_TO_(debug, ltrig)(*cond);
    auto e = (*cond)();
    if (e) evtv.push_back(move(e));
  }
  return evtv;
}


// make events happen
// If threading, scheduling events is the tricky part
// queue or some structure that is friendly to concurrency
template <class Evts>
bool run_events(ControlCtx& ctx, const Evts& evts)
{
  for (auto&& e: evts) {
    if (!e) {
      LOG_(warning)(__func__, ": null event passed");
      continue;
    }
      
    try {
      e->happen(ctx);
    } catch (const stmt::Signal& sig) {
      return ctx.handle_signal(sig);
    }
  }
  
  // State at end:
  // ctx.triggers: for next tick
  // ctx.entities: persist; erase triggered by end condn.
  // events: should all be evaluated, unless early terminated
  // in that case, ctx is not valid anymore, returned false
  return true;
}
}
