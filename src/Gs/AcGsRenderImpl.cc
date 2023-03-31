#include "AcGsRenderImpl.h"
#include "ui/compositor/context_provider.h"

AcGsRenderImpl::AcGsRenderImpl() {
  vertex_.w = 1.0f;
  batch_elements_ = ContextProvider::GetInstance()->GetBatchedElements();
}

AcGsRenderImpl::~AcGsRenderImpl() {
}

void AcGsRenderImpl::setColor(const uint32_t& color) {
  color_ = color;
}

void AcGsRenderImpl::polyline(Adesk::Int32 count, const AcGePoint3d* vertexs) {
  batch_elements_->AddLines(count, vertexs, color_);
#if 0
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
#endif
}

void AcGsRenderImpl::polyline(OdUInt32 count, const OdInt32* index) {
  batch_elements_->AddLines(count, index);
}

void AcGsRenderImpl::triangle(OdUInt32 count, const AcGePoint3d* vertexs) {
  batch_elements_->AddTriangle(count, vertexs, color_);
#if 0
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
#endif
}

void AcGsRenderImpl::triangle(OdUInt32 count, const OdInt32* index) {
  batch_elements_->AddTriangle(count, index);
}

void AcGsRenderImpl::shell(const OdInt32Array& polylines, const OdInt32Array& triangles) {
  batch_elements_->AddShell(polylines, triangles);
}

void AcGsRenderImpl::setVertex(OdUInt32 count, const AcGePoint3d* vertexs) {
  batch_elements_->AddVertex(count, vertexs, color_);
}
