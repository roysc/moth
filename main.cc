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
  
  LOG_TO_(info, lmain)("creating ModelRun");
  ModelRun mr(&ctx);

  // LOG_TO_(info, lmain)("run_events");
  // mr.run_events();
  
  LOG_PUSH_TO_(lloop, lmain)("main loop");
  bool end_cond{};
  while (!(end_cond = mr.run_events())) {
    std::this_thread::sleep_for(std::chrono::milliseconds{200});

    auto loc = ctx.find_class("location");
    for (int i = 0; i < m; ++i)
      auto e = ctx.create_entity({loc});

    LOG_SHOW_TO_(ctx.entities().size(), lloop);
    LOG_SHOW_TO_(ctx.triggers().size(), lloop);
  }
  
  return 0;
}
