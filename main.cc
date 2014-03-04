
#include "json.hh"

#include "mgen.hh"
#include "component.hh"
#include "entity.hh"
#include "event.hh"
#include "condition.hh"
#include "mrun.hh"

int main()
{
  Json js;
  ModelGen mg(js);
  uptr<ModelRun> mr{new ModelRun_impl(mg)};

  return 0;
}
