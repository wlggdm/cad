#ifndef _ACGS_RENDER_IMPL_H_
#define _ACGS_RENDER_IMPL_H_

#include "AcGsDisplay.h"
#include "ui/compositor/batched_elements.h"

#include "GeExtents2d.h"
#include "GeExtents3d.h"

#include "AcGiMatrix3d.h"
#include "Int32Array.h"

class AcGsRenderImpl : public AcGsRender {
public:
  AcGsRenderImpl();
  ~AcGsRenderImpl() override;

  void setColor(const uint32_t& color) override;

  void polyline(Adesk::Int32 count, const AcGePoint3d* vertexs) override;
  void triangle(OdUInt32 count, const AcGePoint3d* vertexs) override;

  void setVertex(OdUInt32 count, const AcGePoint3d* vertexs) override;
  void polyline(OdUInt32 count, const OdInt32* index) override;
  void triangle(OdUInt32 count, const OdInt32* index) override;
  void shell(const OdInt32Array& polylines, const OdInt32Array& triangles) override;

private:
  uint32_t color_;
  BatchedElements* batch_elements_;
  std::vector<int> indexs_;
  XMFLOAT4 vertex_;
  XMFLOAT2 uv_;
};

#endif //_ACGS_RENDER_IMPL_H_