/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"
#include "summator.h"

void register_summator_types() {
    ClassDB::register_class<Summator>();
}

void unregister_summator_types() {
   // Nothing to do here in this example.
}
