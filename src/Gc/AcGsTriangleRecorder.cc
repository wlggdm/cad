#include "AcGsTriangleRecorder.h"

AcGsTriangleRecorder::AcGsTriangleRecorder() {
}

AcGsTriangleRecorder::~AcGsTriangleRecorder() {
}

void AcGsTriangleRecorder::display(AcGsDisplayContext& context) {
   context.render()->triangle(points_.size(), points_.getPtr());
}

void AcGsTriangleRecorder::setVertexs(OdUInt32 count, const OdGePoint3d* vertexs) {
  points_.insert(points_.end(), vertexs, vertexs + count);

  for (OdUInt32 i = 0; i < points_.size(); ++i)
    extent_.addPoint(points_[i]);
}

void AcGsTriangleRecorder::setVertexs(const OdGePoint3d& v1, const OdGePoint3d& v2, const OdGePoint3d& v3) {
  points_.resize(3);
  points_[0] = v1;
  points_[1] = v2;
  points_[2] = v3;

  for (OdUInt32 i = 0; i < points_.size(); ++i)
    extent_.addPoint(points_[i]);
}
