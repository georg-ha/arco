#include "type_anno.h"
#include "visitor.h"


void TypeAnno::accept(Visitor &visitor) {
    return visitor.visit(*this);
}
