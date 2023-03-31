#include "AcGsPolylineRecorder.h"
#include "AcGsDisplayContextImpl.h"

AcGsPolylineRecorder::AcGsPolylineRecorder() {
}

AcGsPolylineRecorder::~AcGsPolylineRecorder() {
}

void AcGsPolylineRecorder::display(AcGsDisplayContext& context) {
  /*
  static AcGePoint3d points[2];
  static OdGeExtents3d extent;
  
  if (vertexs_.size() > 10 && extent_.isValidExtents() && !recorders_.empty()) {
    AcGsView* view = context.view();
    const AcGiMatrix3d* xfm = static_cast<AcGsDisplayContextImpl&>(context).worldToDevice();
    extent = *xfm * extent_;

    if (extent.maxPoint().x - extent.minPoint().x <= 1 ||
        extent.maxPoint().y - extent.minPoint().y <= 1) {
      points[0] = extent_.minPoint();
      points[1] = extent_.maxPoint();

      context.render()->polyline(2, points);

      return;
    }
  }
  */
  //context.render()->polyline(vertexs_.size(), vertexs_.getPtr());
  AcGsRender* render = context.render();

  if (!vertexs_.empty() && !index_.empty()) {
    render->setVertex(vertexs_.size(), vertexs_.getPtr());

    render->polyline(index_.size(), index_.getPtr());
  }
}

void AcGsPolylineRecorder::setVertexs(OdUInt32 count, const OdGePoint3d* vertexs) {
  int index = vertexs_.size();
  vertexs_.insert(vertexs_.end(), vertexs, vertexs + count);

  index_.reserve(index_.size()  + count * 2 - 2);
  for (int i = 0; i < count - 1; ++i) {
    index_.push_back(index + i);
    index_.push_back(index + i + 1);
  }

  for (OdUInt32 i = 0; i < count; ++i)
    extent_.addPoint(vertexs[i]);

  if (parent)
    parent->extent_.addExt(extent_);
}
