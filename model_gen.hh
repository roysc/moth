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
class ModelRun;
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
using Compt_own = uptr<Component>;
using Compt_ptr = ptr<Component>;
using Compt_data = m<Compt_id, Compt_own>;
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
  ModelGen* _modelgen;
  // Compt_id _id;
  string _name;
  // DataType _dtype;

public:
  CptClass(ModelGen* mg, string name): 
    _modelgen(mg), _name(name) {}
  
  // meta data
  string name() const {return _name;}
  // Compt_id id() const {return _id;}
  // components needed to impl. this one
  set<CptClass*> dependencies() const {return {};}
  // create a control context for components of this type
  SystemHandle* system_handle() {return new SystemHandle;}

  // bool operator<(const CptClass& that) {return _id }
};

// """
// The generation context for a generator;
// Namespace of Generation implementation
// """
class ModelGen 
{
protected:
  Compt_id _next_id;
  // Component types
  // m<Compt_id, CptClass> _cptclasses;
  m<Compt_id, CptClass> _cptclasses;
  
public:
  ModelGen(Json js):
    _next_id{}
  {
    // js = json.load(fp)
    auto cs = js.get_child("components");
    for (auto&& c : cs)
      _cptclasses.emplace(
        fresh_id(), CptClass(this, c.second.get<string>("name")));
    
  }

  Compt_id fresh_id() {return ++_next_id;}
  set<CptClass*> component_types() 
  {
    set<CptClass*> ret;
    for (auto&& c: _cptclasses) ret.emplace(&c.second);
    return ret;
  }

  const CptClass* get_class(Compt_id cpid) 
  {
    auto it = _cptclasses.find(cpid);
    return it != end(_cptclasses) ? &it->second : nullptr;
  }
};


// TODO controller strategy
// Control context for sim. instance
struct ControlCtx 
{
  ControlCtx(ModelGen* mg, set<SystemHandle*> shs):
    _modelgen(mg),
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

  const CptClass* get_class(Compt_id cpid) {return _modelgen->get_class(cpid);}

protected:
  ModelGen* _modelgen;
  set<Entity_ptr> _entities;
  set<Cond_ptr> _conditions;
  uptr<Entity> _ctrl_ent;
  uptr<Condition> _end_cond;
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

protected:        
  virtual
  void _run_events(ControlCtx*) = 0;
};

struct Entity
{
protected:
  ControlCtx* _ctx;
  Compt_data _compts;

public:
  // constructed with set of classes
  Entity(ControlCtx* cx, m<Compt_id, CptClass*> cpts):
    _ctx(cx)
  {
    // "Each C in Cpts represents a component meta-type"
    for (auto&& c : cpts)
      _compts.emplace(c.first, Compt_own{create(c.second)});
  }

  // O(log n)
  Compt_ptr get(const Compt_id id) {return _compts.at(id).get();}
  // O(n)
  Compt_ptr find(const string& cn) 
  {
    for (auto&& c: _compts)
      if (_ctx->get_class(c.first)->name() == cn)
        return c.second.get();
    return {};
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
  Expression(const char* c): Expression(string(c)) {}

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
  uptr<Event> operator()()
  {
    // ~~~ m a g i c ~~~
    // Condition works as a closure of several (two?) instance (Field) references,
    // and a (binary?) relationship between them
    
    // An event is simple;
    // 
    
    return {};
  }
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
    
