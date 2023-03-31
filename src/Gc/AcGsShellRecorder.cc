#include "AcGsShellRecorder.h"

AcGsShellRecorder::AcGsShellRecorder() {
}

AcGsShellRecorder::~AcGsShellRecorder() {
}

void AcGsShellRecorder::display(AcGsDisplayContext& context) {
  AcGsRender* render = context.render();

  render->setVertex(points_.size(), points_.getPtr());

  render->shell(polylines_, triangles_);
}

void AcGsShellRecorder::addVertexs(OdUInt32 count, const OdGePoint3d* vertexs) {
  index_ = points_.size();

  points_.insert(points_.end(), vertexs, vertexs + count);

  for (OdUInt32 i = 0; i < points_.size(); ++i)
    extent_.addPoint(points_[i]);
}

void AcGsShellRecorder::polyline(OdUInt32 count, const OdInt32* index) {
  polylines_.reserve(polylines_.size() + count * 2 - 2);

  for (int i = 0; i < count - 1; ++i) {
    polylines_.push_back(index[i]   + index_);
    polylines_.push_back(index[i+1] + index_);
  }
}

void AcGsShellRecorder::triangle(OdUInt32 count, const OdInt32* index) {
  triangles_.reserve(triangles_.size() + count);

  for (int i = 0; i < count; ++i)
    triangles_.push_back(index[i] + index_);
}
