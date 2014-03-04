#include "json.hh"
#include "model_gen.hh"

int main()
{
  Json js;
  ModelGen mg(js);
  uptr<ModelRun> mr{new ModelRun_impl(mg)};

  
}
