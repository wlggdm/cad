#include "AcGsRenderImpl.h"

AcGsRenderImpl::AcGsRenderImpl() {
  vertex_.w = 1.0f;
}

AcGsRenderImpl::~AcGsRenderImpl() {
}

void AcGsRenderImpl::setColor(const uint32_t& color) {
  color_ = color;
}

void AcGsRenderImpl::polyline(Adesk::Int32 count, const AcGePoint3d* vertexs) {
  if (count < 2)
    return;

  if (indexs_.size() < count)
    indexs_.resize(count);

  for (OdInt32 i = 0; i < count; ++i) {
    vertex_.x = vertexs[i].x;
    vertex_.y = vertexs[i].y;
    vertex_.z = vertexs[i].z;

    indexs_[i] = batch_elements_->AddVertex(vertex_, uv_, color_);
  }

  for (OdInt32 i = 0; i < count - 1; ++i)
    batch_elements_->AddLine(indexs_[i], indexs_[i + 1]);
}

void AcGsRenderImpl::triangle(OdUInt32 count, const AcGePoint3d* vertexs) {
  if (indexs_.size() < count)
    indexs_.resize(count);

  for (OdInt32 i = 0; i < count; ++i) {
    vertex_.x = vertexs[i].x;
    vertex_.y = vertexs[i].y;
    vertex_.z = vertexs[i].z;

    indexs_[i] = batch_elements_->AddVertex(vertex_, uv_, color_);
  }

  for (OdInt32 i = 0; i < count; i += 3)
    batch_elements_->AddTriangle(indexs_[i], indexs_[i+1], indexs_[i+2]);
}

void AcGsRenderImpl::shell(OdUInt32 count, const AcGePoint3d* vertexs,
                           OdUInt32 index_count, const OdInt32* indexs) {
  if (indexs_.size() < count)
    indexs_.resize(count);

  for (OdInt32 i = 0; i < count; ++i) {
    vertex_.x = vertexs[i].x;
    vertex_.y = vertexs[i].y;
    vertex_.z = vertexs[i].z;

    indexs_[i] = batch_elements_->AddVertex(vertex_, uv_, color_);
  }

  OdInt32 begin = indexs_.front();
  for (OdInt32 i = 0; i < index_count; i += 3)
    batch_elements_->AddTriangle(indexs[i] + begin, indexs[i+1] + begin, indexs[i+2] + begin);
}
