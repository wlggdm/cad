#include "AcGsRecorderImpl.h"

AcGsRecorderImpl::AcGsRecorderImpl() {
  parent = nullptr;
}

AcGsRecorderImpl::~AcGsRecorderImpl() {
}

void AcGsRecorderImpl::display(AcGsDisplayContext& context) {
  for (auto& iter : recorders_)
    iter->display(context);
}

bool AcGsRecorderImpl::extents(OdGeExtents3d& extents) const {
  if (!extent_.isValidExtents())
    return false;

  extents = extent_;
  return true;
}

void AcGsRecorderImpl::addRecorder(AcGsRecorderImpl* recorder) {
  recorders_.push_back(std::unique_ptr<AcGsRecorderImpl>(recorder));
  recorder->parent = this;

  if (recorder->hasExtent()) {
    OdGeExtents3d ext;
    if (recorder->extents(ext))
       extent_.addExt(ext);
  }
}
