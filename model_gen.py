# -*- coding: utf-8 -*-

import json

class Controls: pass

class ModelGen:
    """
    The generation context for a generator;
    Namespace of Generation implementation
    
    """
    def __init__(self, js):
        # self._components = []
        js = json.load(fp)
        cs = js.get("components", [])
        self._Components = [Component(c["name"], self) for c in cs]
        self._Conditions = [Condition(c["name"], self) for k in ks]
        # self._SystemComponents = ?

    def read_file(f):
        fp = open(f)
        
        # es = read_entities(js.get("entities", []))

        # conds = read_conditions(js.get("conditions", []))
        # evts = read_events(js.get("events", []))

    def systems(self):
        """Return a set of Systems and a control system
        => (control, [contexts])
        """
        # create a System entity, made up a group of (singleton) components

        # Cs = [C for C in self._Components if C.name == name]
        # ret = System(Cs)
        # "Contexts" for components needing centralized control
        # eg. Location/coordinates
        #     Physics
        #     Information network
        ctxs = System(C for C in self._Components if C.name == name)
        ctrl = Controls() ??
        return (ctrl, ctxs)
        
    def create_model(self):
        "Create model."
        modelrun = ModelRun(self)
        return modelrun


class ModelRun():
    def __init__(self, mgen):
        "Runnable model."
        # 1. instantiate systems
        # 2. start iterating:
        #   x = end condition
        #   until x()
        #     k = new conditions met
        #     e = yesterday's entities
        #     v = new events(k)
        #     e <- e + new entities(v)
        #     c <- update(c, v)
        #
        
        self._modelgen = mgen
        # the "primary" system, an abstract interface for the user's control.
        (self._control, self._systems) = mgen.systems()

    def loop(self, end_cond=None):
        from functools import reduce

        # The 0-condition
        # meet this to end the game (just a bool in primary System)
        if end_cond is None:
            end_cond = self._control.end_cond()

        self._conditions = []
        events = []
        while not end_cond():
            for (ok, evtr) in self._run_events():
                if ok:
                    self._conditions.extend(evtr.new_conditions())
                    self._entities.extend(evtr.new_entities())
                    # print(evtr)
                else:
                    # Failed event
                    err = evtr.error
                    print("Failed:", evtr.error)

        # State at end
        # self._conditions = uneval'd conditions (may be some met)
        # self._entities = 
        # events = last delivered events
        
    def _run_events(self):
        # what does event handle?
        # Types:
        #   Evt(Force, Body -> BodyState)
        #   force: sys. component
        #   obj.: sys. entity
        #
        #   c= Cond(Body.pain > _)
        #   => e= Evt(Agent.Senses -> Agent.Senses)
        #   r = e() = (s -> s.alarmed; s)
        # 
        #   Cond(Agent.decision ~ go(_))
        #   => Evt(Loc -> Loc)
        #   r = e();
        #   r should produce a follow-up condition checking arrival,
        #     and checking path
        #
        #   Cond(Agent.smell ~ meat)
        #   => Evt(Agent -> Go)
        #   Cond(Agent.)
        #   Evt(Agent -> Agent)
        
        # An event need only see its operands
        # It represents an expression, symbolically
        # Can't be too complicated
        #
        # met conditions trigger events
        # create set of Events
        self.__events = filter(None, map(lambda c: c(), self._conditions))

        # create EventResults...?
        for e in self.__events:
            ok = True
            r = e()
            if not r: ok = False
            yield (ok, r)

        
class Entity:

    def __init__(self, Cs):
        "Each C in CS represents a type"
        self._component_state = [C() for C in Cs]

    pass

class System(Entity):
    # each Component C
    # represents state that is shared across System s
    # hence, instantiate once. no need for special ctor.
    # def __init__(self, Cs):
    pass

class Component:
    """A trait of an entity"""
    
    pass

class Condition:
    """Trigger object

    c = Condition()
    r = c()
      => Event(...) -- condition is satisfied
      => None       -- not
    """
    def __init__(self, *args):
        pass
    def __call__(self, *args):
        pass
    def __bool__(self):
        

class Event:
    def __init__(self, *args):
        pass
    pass

        
def read_model(file):
    mg = ModelGen()
    return mg
    
