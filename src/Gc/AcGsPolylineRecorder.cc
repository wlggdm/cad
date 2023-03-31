#include "AcGsPolylineRecorder.h"
#include "AcGsDisplayContextImpl.h"


AcGsPolylineRecorder::AcGsPolylineRecorder() {
}

AcGsPolylineRecorder::~AcGsPolylineRecorder() {
}

void AcGsPolylineRecorder::display(AcGsDisplayContext& context) {
#if 0
  static AcGePoint3d points[2];

  if (vertexs_.size() > 10 && extent_.isValidExtents() && !recorders_.empty()) {
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
#endif
  context.render()->polyline(vertexs_.size(), vertexs_.getPtr());
}

void AcGsPolylineRecorder::setVertexs(OdUInt32 count, const OdGePoint3d* vertexs) {
  vertexs_.insert(vertexs_.end(), vertexs, vertexs + count);

  for (OdUInt32 i = 0; i < count; ++i)
    extent_.addPoint(vertexs[i]);
}
