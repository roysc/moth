
#include <iostream>
#include <fstream>

#include "json.hh"

#include "mgen.hh"
#include "component.hh"
#include "entity.hh"
#include "event.hh"
#include "condition.hh"
#include "mrun.hh"

// #include "util/io.hh"

int main(int argc, const char* argv[])
{
  v<string> args(argv, argv + argc);
  Json js;

  if (argc < 2) 
  {
    std::cout << "Usage: " << args[0] << " <file>.json\n";
    exit(1);
  }


  std::ifstream in(args.at(1));
  json::read_json(in, js);
  
  ModelGen mg(js);
  ControlCtx ctx(&mg);

  ModelRun mr(&ctx);
  mr.run_events();
  // do stuff
  // control sim.

  return 0;
}
