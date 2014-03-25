// -*- coding: utf-8 -*-
// main.cc

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

#include "log.hh"
#include "json.hh"

#include "typedefs.hh"
#include "mgen.hh"
#include "controlctx.hh"
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
  ControlCtx ctx(&mg);

  // ---
  // Trigger & entity initialization
  // ---
  Compt_id
    tmid = ctx.find_type("_time_").at(0),
    locid = ctx.find_type("_loc_").at(0);

  // auto ctrl_ent = ctx
  auto ctrl_ent = ctx.create_entity({tmid, locid});

  // expressions DSL would be cool
  // namespace exb = expr::builder;
  // auto tm_expr = exb::lit(5) == ctrl_ent->ref(tmid);
  
  // 0-condition, ends the game (bool in a builtin component)
  // when evaluated as true, ends the game. eg. run ends after 5 ticks:
  // 
  auto tm_ctr = ctrl_ent->ref({tmid});

  // when to stop
  auto end_ex = ctx.expression("=", {
      new expr::ERef(tm_ctr),
      new expr::ELit(Data::make<data::Int>(5))
    });
  ctx.set_trigger(end_ex, new stmt::Halt);

  // tick the clock
  auto tick_st = new stmt::Incr(tm_ctr, +1);
  ctx.set_trigger(nullptr, tick_st);

  LOG_PUSH_(lloop)("main loop");
  for (bool stop{}; !stop; ) {

    // zzz
    std::this_thread::sleep_for(std::chrono::milliseconds{200});
    
    auto evts = run::eval_triggers(ctx);
    // return val indicates current validity
    stop = !run::run_events(ctx, evts);

    // LOG_SHOW_TO_(ctx.entities().size(), lloop);
    // LOG_SHOW_TO_(ctx.triggers().size(), lloop);
  }
  
  return 0;
}
