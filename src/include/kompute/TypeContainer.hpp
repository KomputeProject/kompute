// SPDX-License-Identifier: Apache-2.0
#pragma once

#include "ABCTypeContainer.hpp"
#include <typeinfo>


struct IdCounter
{
    static size_t counter;
};

template<typename T>
class TypeContainer : public ABCTypeContainer, IdCounter
{
  private:
    size_t classId()
    {
        static size_t id = counter++;
        return id;
    }

  public:
    TypeContainer() : dt(typeid(T)) {}

    bool compare(ABCTypeContainer& other) override
    {
        TypeContainer& obj = static_cast<TypeContainer&>(other);
        return this->classId() == obj.classId();
    }

    std::string name() override { return this->dt.name(); }

    const std::type_info& dt;
};