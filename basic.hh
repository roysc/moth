// -*- coding: utf-8 -*-
// basic.hh
// \brief basic types
#pragma once

#include "typedefs.hh"

// // Component meta-data
// struct cpt::Specs;

// // a cpt::Specs that yields controllers
// struct ControlHandle;
// using ControlHandle_opt = uptr<ControlHandle>;

// // Control structs
// struct ControlCtx;
// struct ControlHandle;

// // Generator and Runner
// class ModelGen;
// class ModelRun;

// entity in running game
struct Entity;
using Entity_ptr = Entity*;

// a trigger: (expr(:bool), statement)
struct Trigger;
using Trig_ptr = Trigger*;

// unique identifier for component class
using Compt_id = unsigned;
