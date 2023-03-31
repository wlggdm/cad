#include "AcGsNodeImpl.h"

AcGsNodeImpl::AcGsNodeImpl(NodeType type) : 
  type_(type) {
}

AcGiDrawable * AcGsNodeImpl::drawable(void) const {
  OdGiDrawablePtr drawable = object_id_.openObject().get();
  drawable->addRef();

  return drawable.get();
}

void AcGsNodeImpl::setDrawable(AcGiDrawable* drawable) {
  object_id_ = drawable->id();
}

AcGsNodeImpl::NodeType AcGsNodeImpl::type() const {
  return type_;
}
