// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <string>

class ABCTypeContainer
{
  public:
    // Pure Virtual Function
    virtual bool compare(ABCTypeContainer& obj) = 0;
    virtual std::string name() = 0;
};