/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"
#include "KomputeSummator.hpp"

void register_summator_types() {
    ClassDB::register_class<KomputeSummator>();
}

void unregister_summator_types() {
   // Nothing to do here in this example.
}
