// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

#include <queue>
#include <utility>
#include "entity.hh"
#include "event.hh"
#include "condition.hh"

// TODO controller strategy
// Control context for sim. instance
struct ControlCtx 
{
  ControlCtx(ModelGen* mg, set<SystemHandle*> shs):
    _modelgen(mg),
    _end_cond(end_cond()),
    _ctrl_ent(ctrl_entity())
  {}

  set<Entity_ptr> entities() {return _entities;}
  set<Cond_ptr> conditions() {return _conditions;}

  const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}
  
  // make an entity for control...
  static
  Entity_ptr ctrl_entity()
  {
    return {};
  }

  // The condition, which when evaluated as true, ends the game;
  // expressed as: 
  static
  Cond_ptr end_cond()
  {
    // eg. run ends after 4 "days"
    // Condition(= time.n_day 4)
    // -> Halt()
    return {};
  }

protected:
  ModelGen* _modelgen;
  set<Entity_ptr> _entities;
  set<Cond_ptr> _conditions;
  uptr<Entity> _ctrl_ent;
  uptr<Condition> _end_cond;
};

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
    _sys_handles(systems()),
    _controlctx{new ControlCtx{&_modelgen, systems()}}
  {
    // create default controls
  }

  ControlCtx* control_ctx() override {return _controlctx.get();}

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
  ControlCtx* run(ControlCtx* ctx=nullptr) override
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
  }
};
