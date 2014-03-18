// -*- coding: utf-8 -*-
// mrun.hh
#pragma once

// #include "cptclass.hh"

struct CptClass;

struct ControlHandle
{
  const CptClass* cptclass() const {return _cptclass;}
protected:
  const CptClass* _cptclass;
};
