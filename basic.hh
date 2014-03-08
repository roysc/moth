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

// the idea is that a component is only accessed through Compt_addr keys
using Compt_id = unsigned;

// a trigger: (expr(:bool), statement)
struct Condition;
using Cond_ptr = Condition*;
