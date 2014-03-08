// -*- coding: utf-8 -*-
// basic.hh
// \brief basic types
#pragma once

#include "typedefs.hh"

// // Component meta-data
// struct CptClass;

// // a CptClass that yields controllers
// struct SystemHandle;
// using SystemHandle_opt = uptr<SystemHandle>;

// // Control structs
// struct ControlCtx;
// struct SystemHandle;

// // Generator and Runner
// class ModelGen;
// class ModelRun;

// entity in running game
struct Entity;
using Entity_ptr = Entity*;

// a trigger: (expr(:bool), statement)
struct Condition;
using Cond_ptr = Condition*;

// unique identifier for component class
using Compt_id = unsigned;
