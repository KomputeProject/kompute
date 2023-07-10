// SPDX-License-Identifier: Apache-2.0
#pragma once
#include <string>

class ABCTypeContainer
{
  public:
    // Pure Virtual Function
    virtual bool operator==(const ABCTypeContainer& other) const = 0;
    virtual std::string name() = 0;

    bool operator!=(const ABCTypeContainer& other) const
    {
        return !(*this == other);
    }
};