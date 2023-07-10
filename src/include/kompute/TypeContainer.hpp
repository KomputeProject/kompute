// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ABCTypeContainer.hpp"
#include <typeinfo>

template<typename T>
class TypeContainer : public ABCTypeContainer
{
  public:
    TypeContainer() : dt(typeid(T)) {}

    bool operator==(const ABCTypeContainer& other) const override
    {
        const TypeContainer* obj = dynamic_cast<const TypeContainer*>(&other);
        return obj && this->dt == obj->dt; //we might be able to simplfy to just checking if the cast succeeded 
    }

    std::string name() override { return this->dt.name(); }

    const std::type_info& dt;
};