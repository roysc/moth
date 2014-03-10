// -*- coding: utf-8 -*-
// main.cc

#include <iostream>
#include <fstream>
#include <string>
#include <thread>
#include <chrono>

#include "json.hh"
#include "log.hh"

#include "mgen.hh"
#include "mrun.hh"

int main(int argc, const char* argv[])
{
  v<string> args(argv, argv + argc);

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

  LOG_TO_(info, lmain)("creating ModelGen");
  ModelGen mg(js);
  
  LOG_TO_(info, lmain)("creating ControlCtx");
  ControlCtx ctx(&mg);

  // ---
  // Trigger & entity initialization
  // ---
  auto endid = ctx.find_class("_end_");
  auto tmid = ctx.find_class("_time_");
  auto ctrl_ent = ctx.create_entity({tmid, endid});
    
  // 0-condition, ends the game (bool in a builtin component)
  // when evaluated as true, ends the game. eg. run ends after 4 "days":
  //  Trigger(= time.n_day 4) -> Halt()
  // 
  // expressions should perhaps have value semantics
  // namespace exb = expr::builder;
  // auto tm_expr = exb::ex(5) == ctrl_ent->ref(tmid);
  auto tm_expr = new expr::EFun("=="_op, {
      new expr::ERef(ctrl_ent->ref(tmid)),
      new expr::ELit(Data::make<data::Int>(5))
    });
  
  auto tm_trig = new Trigger(tm_expr, ctrl_ent, new stmt::Halt);
  // auto tick_stmt = new stmt::Spawn("_tick_");

  ctx.set_trigger(tm_trig);
    
  Compt_addr end_flag = ctrl_ent->ref(endid);
  Compt_addr tm_ctr = ctrl_ent->ref(tmid);

  LOG_PUSH_TO_(lloop, lmain)("main loop");
  for (bool end_cond{}; !end_cond; ) {

    LOG_SHOW_TO_(*end_flag(), lmain);
    LOG_SHOW_TO_(*tm_ctr(), lmain);

    auto evts = run::eval_triggers(ctx);
    // There must be some way to know when the model is invalidated, or aborted
    auto valid = run::run_events(ctx, evts);
    if (!valid) break;
    
    // namespace stb = stmt::builder;
    // auto inc_stmt = ++stb::ref(tmid);
    // => new stmt::Update{tmid, (exb::ref(tmid) + 1)}
    // => new stmt::Update{tmid, new expr::EFun("+"_op, new expr::ERef(tmid), new expr::ELit<)}

    // auto loc = ctx.find_class("location");
    // for (int i = 0; i < m; ++i)
    //   auto e = ctx.create_entity({loc});

    std::this_thread::sleep_for(std::chrono::milliseconds{200});

    // LOG_SHOW_TO_(ctx.entities().size(), lloop);
    // LOG_SHOW_TO_(ctx.triggers().size(), lloop);

    data::Int tmct{tm_ctr()->get<data::Int>().value + 1};
    tm_ctr()->set(tmct);
    
    end_cond = end_flag()->get<data::Bool>().value;
  }
  
  return 0;
}
