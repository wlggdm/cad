#include "AcGsShellRecorder.h"

AcGsShellRecorder::AcGsShellRecorder() {
}

AcGsShellRecorder::~AcGsShellRecorder() {
}

void AcGsShellRecorder::display(AcGsDisplayContext& context) {
  AcGsRender* render = context.render();

  render->setVertex(points_.size(), points_.getPtr());

  render->shell(polylines_, triangles_);
  /*
  if (!polylines_.empty())
    render->polyline(polylines_.size(), polylines_.getPtr());

  if (!triangles_.empty())
    render->triangle(triangles_.size(), triangles_.getPtr());
  */
}

void AcGsShellRecorder::setVertexs(OdUInt32 count, const OdGePoint3d* vertexs) {
  points_.insert(points_.end(), vertexs, vertexs + count);

  for (OdUInt32 i = 0; i < points_.size(); ++i)
    extent_.addPoint(points_[i]);
}

void AcGsShellRecorder::polyline(OdUInt32 count, const OdInt32* index) {
  polylines_.reserve(polylines_.size() + count*2-2);
  for (int i = 0; i < count - 1; ++i) {
    polylines_.push_back(index[i]);
    polylines_.push_back(index[i+1]);
    if (polylines_.size() > 198){
      const OdInt32* ii = polylines_.getPtr();
      continue;
    }
  }
}

void AcGsShellRecorder::triangle(OdUInt32 count, const OdInt32* index) {
  triangles_.insert(triangles_.end(), index, index + count);
}
