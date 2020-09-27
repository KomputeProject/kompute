/* register_types.cpp */

#include "register_types.h"

#include "core/class_db.h"
#include "KomputeModelMLNode.h"

void register_kompute_summator_types() {
    ClassDB::register_class<KomputeModelMLNode>();
}

void unregister_kompute_summator_types() {
   // Nothing to do here in this example.
}
