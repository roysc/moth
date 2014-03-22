// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

#include "cptctx.hh"

struct ControlHandle
{
  const cpt::Specs* cptclass() const {return _cptclass;}
protected:
  const cpt::Specs* _cptclass;
};
