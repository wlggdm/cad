#include "AcGsEntityRecorder.h"
#include "AcGsDisplayContextImpl.h"

AcGsEntityRecorder::AcGsEntityRecorder(bool flag) : flag_(flag){
}

AcGsEntityRecorder::~AcGsEntityRecorder() {
}

void AcGsEntityRecorder::display(AcGsDisplayContext& context) {
  static AcGePoint3d points[2];
  
  if (1/*flag_*/) {
    if (extent_.isValidExtents() && !recorders_.empty()) {
      bool fade = false;
      if (!context.intersectView(extent_, fade))
        return;

      if (fade) {
        points[0] = extent_.minPoint();
        points[1] = extent_.maxPoint();

        recorders_[0]->display(context);
        context.render()->polyline(2, points);

        return;
      }
    }
  }

  AcGsRecorderImpl::display(context);
}
