// -*- coding: utf-8 -*-
// 
#include <vector>
#include <string>
#include <memory>
#include <set>
#include <map>
#include <queue>
#include <utility>
#include <iterator>
#include <algorithm>
#include <thread>

// import json
#include "json.hh"
// 

template <class T> using v = std::vector<T>;
using std::string;
using std::set;
template <class K,class V> using m = std::map<K,V>;
using std::pair;
// template <class T> using table = std::unordered_map<T>;
using std::thread;

template <class T> using ptr = T*;
// can write "T*" or "ptr<T>"
template <class T> using uptr = std::unique_ptr<T>;
template <class T> using sptr = std::shared_ptr<T>;
template <class T> using wptr = std::weak_ptr<T>;

class ControlCtx;
class ModelGen;
class ModelRun_impl;
struct CptClass;
// using CptClass_set = set<CptClass>;

// An instance in the running model
struct Entity;
using Entity_ptr = ptr<Entity>;
using Entity_set = set<Entity_ptr>;
using Entity_vec = v<Entity_ptr>;

// """A trait of an entity"""
struct Component
{
public:
  Component(string e)
  {
    // ?
  }
};
using Compt_id = uint32_t;
using Compt_ptr = ptr<Component>;
using Compt_data = m<Compt_id, Compt_ptr>;
// using Compt_data = m<Compt_id, uptr<Component>>;
using Compt_addr = pair<Entity_ptr, Compt_id>;
// using Compt_set = set<Compt_ptr>;

// "Trigger" object
struct Condition;
struct Expression;
using Cond_ptr = Condition*;


// # each CptClass C
// # represents state that is shared across System s
// # hence, instantiate once. no need for special ctor.
// # def __init__(self, Cs):
// struct Entity;

struct SystemHandle {};
using SystemHandle_opt = uptr<SystemHandle>;

struct CptClass
{
protected:
  ModelRun_impl* _modelrun;
  uint32_t _id;
  string _name;

public:
  CptClass(ModelRun_impl* mr, uint32_t id, string name): 
    _modelrun(mr), _name(name), _id(id) {}

  // meta data
  string name() { return _name; }
  set<CptClass> dependencies()
  {
    return {};
  }

  SystemHandle* system_handle() {return {new SystemHandle{}};}
};

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
class ModelGen 
{
protected:
  // Component types
  set<CptClass> _components;
  
public:
  ModelGen(Json js)
  {
    {
    // js = json.load(fp)
      auto cs = js.get_child("components");
      for (auto&& c : cs)
        _components.emplace(c.second.get<string>("name"), this);
    }
  }
  set<CptClass*> component_types() 
  {
    set<CptClass*> ret;
    std::transform(begin(_components), end(_components), back_inserter(ret),
                   [](CptClass& c) {return &c;});
    return ret;
    
  }
};

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
  
  virtual
  CptClass* get_class(Compt_id) = 0;

protected:        
  virtual
  void _run_events(ControlCtx*) = 0;
};

struct Entity
{
protected:
  ModelRun* _modelrun;
  Compt_data _compts;

public:
  // constructed with set of classes
  Entity(ModelRun* mr, set<CptClass*> cpts):
    _modelrun(mr)
  {
    // "Each C in Cpts represents a component meta-type"
    for (auto&& c : cpts)
      _compts.emplace(c->name(), create(c));
  }

  // O(log n)
  Compt_ptr get(const Compt_id ci) {return _compts.at(ci);}
  // O(n)
  Compt_ptr find(const string& cn) 
  {
    auto it = find_if(begin(_compts), end(_compts), 
                      [&](Compt_id k, Compt_ptr) {
                        return _modelrun->get_class(k)->name() == cn;
                      });
    return it != end(_compts) ? it->second : 0;
  }


  // TODO
  Compt_ptr create(CptClass* cpc) {return {};}
};

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
enum class EventKind: uint32_t {spawn, update, destroy, N};

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


// Condition:
// """Trigger object
// 
// c = Condition()
// r = c()
//   => Event(...) -- condition is satisfied
//   => None       -- not
// """

// Optional Event
using Event_ptr = ptr<Event>;
using Event_opt = uptr<Event>;

// _simple_ AST
struct Expression
{
protected:
  string _repr;

public:
  Expression(const string& e)
  {
    assert(valid(e));
    _repr = e;
  }
  Expression(const char* c): Expression(c) {}

  // check validity of string
  static bool valid(const string& e) {return true;}
};

// 
struct Condition
{
protected:
  // Field_set _fields;
  Expression _expr;
  v<Compt_addr > _args;

public:
  Condition(Expression ex, v<Compt_addr> ca):
    _expr(ex)
  { }

  // Here shall happen the magic
  // Conditions ideally are evaluated in as isolated a context as possible
  Event_opt operator()()
  {
    // ~~~ m a g i c ~~~
    // Condition works as a closure of several (two?) instance (Field) references,
    // and a (binary?) relationship between them
    
    // An event is simple;
    // 
    
    return nullptr;
  }
};


// TODO controller strategy
// Control context for sim. instance
struct ControlCtx 
{
  ControlCtx(set<SystemHandle*> shs):
    _end_cond(end_cond()),
    _ctrl_ent(ctrl_entity())
  {}
  
  // make an entity for control...
  static
  Entity_ptr ctrl_entity()
  {
    return 0;
  }

  // The condition, which when evaluated, ends the game;
  // expressed as: 
  static
  Cond_ptr end_cond()
  {
    // eg. run ends after 4 "days"
    // Condition(= time.n_day 4)
    // -> Halt()
    return 0;
  }
  set<Entity_ptr> entities() {return _entities;}
  set<Cond_ptr> conditions() {return _conditions;}

protected:
  set<Entity_ptr> _entities;
  set<Cond_ptr> _conditions;
  uptr<Entity> _ctrl_ent;
  uptr<Condition> _end_cond;
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

  ControlCtx* control_ctx() override {return new ControlCtx{};}

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

  ptr<ControlCtx>
  run(ControlCtx* ctx=nullptr) override
  {
    if (!ctx)
      ctx = control_ctx(_sys_handles);
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
  //   void push(Event_ptr e)
  //   {
      
  //   }
  // };
  using EventQueue = std::queue<Event_ptr>;

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
    v<thread> condjobs;
    v<Event_opt> evtv(conds.size());
    for (size_t i{}; i < conds.size(); ++i)
    {
      auto ci = begin(conds) + i;
      auto ei = begin(evtv) + i;
      condjobs.emplace_back([=]{*ei = (**ci)();});
    }
    // TODO: threadpool or scheduling here
    for (auto&& cj: condjobs)
      cj.join();

    // queue event results, skip duds
    for (auto&& e: evtv)
    {
      if (!e) continue;
      evtq.push(e);
    }

    // Scheduling events is the tricky part
    {
      // TaskSchedule sched{evtq};
      // ...?
    }
  }
};
    
