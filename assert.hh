// -*- coding: utf-8 -*-
// assert.hh
#pragma once

#include "err.hh"
#include "log.hh"

// TODO: soft & hard asserts

#define ASSERT_WHAT_(p_, repr_, ...)                                    \
  do {                                                                  \
    if (!(p_)) {                                                        \
      LOG_(fatal)("Assertion `", (repr_), "' failed: ", __VA_ARGS__);   \
      THROW_(Failed_assert, repr_);                                     \
    }                                                                   \
  } while(0)

#define ASSERT_(p_, ...)                        \
  ASSERT_WHAT_(p_, #p_, __VA_ARGS__)

// Show arguments, like gtest
#define ASSERT_BINP_(a_, b_, op_, ...)                                  \
  do {                                                                  \
    const auto aval_ = (a_);                                            \
    const auto bval_ = (b_);                                            \
    ASSERT_WHAT_(                                                       \
      aval_ op_ bval_,                                                  \
      #a_ " " #op_ " " #b_,                                             \
      "[" #a_ " = ", aval_, " ; " #b_ " = ", bval_, "]: ",               \
      __VA_ARGS__                                                       \
    );                                                                  \
  } while (0)

#define ASSERT_EQ_(a_, b_, ...)                 \
  ASSERT_BINP_(a_, b_, ==, __VA_ARGS__)
