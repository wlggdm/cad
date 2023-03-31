#ifndef _ACGS_RENDER_IMPL_H_
#define _ACGS_RENDER_IMPL_H_

#include "AcGsDisplay.h"
#include "ui/compositor/batched_elements.h"

#include "GeExtents2d.h"
#include "GeExtents3d.h"

#include "AcGiMatrix3d.h"

class AcGsRenderImpl : public AcGsRender {
public:
  AcGsRenderImpl();
  ~AcGsRenderImpl() override;

  void setColor(const uint32_t& color) override;

  void polyline(Adesk::Int32 count, const AcGePoint3d* vertexs) override;
  void triangle(OdUInt32 count, const AcGePoint3d* vertexs) override;
  void shell(OdUInt32 count, const AcGePoint3d* points,
             OdUInt32 index_count, const OdInt32* indexs) override;

  void setBatchElements(BatchedElements* batch_elements) {
    batch_elements_ = batch_elements;
  }

private:
  uint32_t color_;
  BatchedElements* batch_elements_;
  std::vector<int> indexs_;
  XMFLOAT4 vertex_;
  XMFLOAT2 uv_;
  
};

#endif //_ACGS_RENDER_IMPL_H_