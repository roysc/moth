// -*- coding: utf-8 -*-
// main.cc

#include <iostream>
#include <fstream>

#include "json.hh"
#include "log.hh"

#include "mgen.hh"
#include "mrun.hh"

int main(int argc, const char* argv[])
{
  v<string> args(argv, argv + argc);

  if (argc < 2) 
  {
    std::cout << "Usage: " << args[0] << " <file>.json\n";
    exit(1);
  }

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
  while (!(end_cond = mr.run_events()))
  {}
  
  return 0;
}
