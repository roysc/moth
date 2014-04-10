// -*- coding: utf-8 -*-
// main.cc

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>
#include <random>

#include "log.hh"
#include "json.hh"

#include "typedefs.hh"
#include "mgen.hh"
#include "controlctx.hh"
// #include "expression.hh"
#include "mrun.hh"

int main(int argc, const char* argv[])
{
  vector<string> args(argv, argv + argc);

  if (argc < 2) {
    std::cout << "Usage: " << args[0] << " <file>.json\n";
    exit(1);
  }

  // file, n turns, m entities/turn
  int n = 5, m = 10;
  if (argc >= 3) n = stoi(args[2]);
  if (argc >= 4) m = stoi(args[3]);
  
  Json js;
  std::ifstream in(args.at(1));
  json::read_json(in, js);

  LOG_PUSH_(lmain)("main()");

  LOG_(info)("creating ModelGen");
  ModelGen mg(js);
  
  LOG_(info)("creating ControlCtx");
  ControlCtx ctx(mg);

  // ---
  // Trigger & entity initialization
  // ---
  Compt_id
    tmid = ctx.find_type("_Time").at(0),
    locid = ctx.find_type("_Loc").at(0);

  auto ctrl_ent = ctx.create_entity({tmid, locid});

  // 0-condition, ends the game (bool in a builtin component)
  // when evaluated as true, ends the game. eg. run ends after 5 ticks:
  // 
  auto tm_ctr = ctrl_ent->ref({tmid});

  // expression/statement DSLs
  namespace exb = expr::builder;
  namespace stb = stmt::builder;

  // tick the clock
  ctx.create_trigger({}, stb::incr(tm_ctr, 1));

  // when to stop
  ctx.create_trigger(
    exb::ref(tm_ctr) >= exb::lit(10),
    stb::halt()
  );
  
  // auto f = ctx.create_entity("Fish", {0, 0});
  // f->set("_Loc_")(0,0);
  // ctrl_ent->set()
  auto lrid = ctx.find_type("Loc_ref");
    
  // "Move fish (x,y)", possible syntax:
  // ''''
  // (x,y) = uniform(Space:Loc)
  // # mx = Fish:max_swim
  // # Rng = R[-mx,mx]
  // # (x,y) = uniform(Rng,Rng)
  // (Fish).loc <- loc(x,y)
  // ''''
  // ctx.set_trigger({}, stb::ref(f->ref(lrid)) += data(+1, -1));

  // auto swd = ctx.free_cpt("Item", {"sword", "atk:+5", "dur:2"});
  // auto shl = ctx.free_cpt("Item", {"shield", "def:+5", "dur:4"});
  // auto get_loc = exb::data(5,5);
  // auto get_ex = exb::ref(f, lrid) == get_loc;
  // ctx.set_trigger(get_ex, stb::ref(f, "Inv") << swd << shl);

  auto r = run::event_loop(ctx);
  
  return 0;
}
