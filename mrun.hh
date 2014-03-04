// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

#include <queue>

// Model interface
class ModelRun
{
public:
  // 
  virtual
  ControlCtx* control_ctx() = 0;

  // """\return a set of Systems and a control system
  // => (control, [contexts])
  // """
  virtual
  set<SystemHandle*> systems() = 0;

  virtual
  ControlCtx* run(ControlCtx* ctx=nullptr) = 0;

protected:        
  virtual
  void _run_events(ControlCtx*) = 0;
};

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
class ModelRun_impl : public ModelRun
{
  ModelGen _modelgen;
  set<SystemHandle*> _sys_handles;
  uptr<ControlCtx> _controlctx;

public:
  ModelRun_impl(ModelGen mgen):
    _modelgen(mgen),
    // # the "primary" system, an abstract interface for the user's control.
    _sys_handles(systems())
  {
    // create default controls
  }

  ControlCtx* control_ctx() override {return new ControlCtx{&_modelgen, systems()};}

  // """\return a set of Systems and a control system
  // => (control, [contexts])
  // """
  set<SystemHandle*> systems() override
  {
    // # Cs = [C for C in self._Components if C.name == name]
    // # ret = System(Cs)
    // # "Contexts" for components needing centralized control
    // # eg. Location/coordinates
    // #     Physics
    // #     Information network
    set<SystemHandle*> ret;
    
    for (auto&& c : _modelgen.component_types())
    {
      SystemHandle* hdl;
      // user ctrl integration:
      // system_handle() => Handle(_) -- supports user control
      //                  | nullptr   -- no
      if ((hdl = c->system_handle()))
        ret.emplace(hdl);
    }
    return ret;
  }

  // Run the model
  ptr<ControlCtx>
  run(ControlCtx* ctx=nullptr) override
  {
    if (!ctx)
      ctx = control_ctx();
    // ctx = new ControlCtx{&conds, &events};
    
    _run_events(ctx);

    // # State at end
    // # self._conditions = uneval'd conditions (may be some met)
    // # self._entities = 
    // # events = last delivered events
    return ctx;
  }

protected:        

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

  void _run_events(ControlCtx* ctx) override
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
    auto end_cond = ctx->end_cond();
    auto&& conds = ctx->conditions();
    EventQueue evtq;

    // Create a vector of threads, and of event results, same size as condv
    v<thread> evaljobs;
    v<uptr<Event> > evtv(conds.size());
    for (auto itc = begin(conds); itc != end(conds); ++itc)
    {
      auto ite = begin(evtv) + distance(begin(conds), itc);
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
  }
};
