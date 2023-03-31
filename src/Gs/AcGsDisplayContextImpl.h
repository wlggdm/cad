#ifndef _ACGS_DISPLAY_CONTEXT_IMPL_H_
#define _ACGS_DISPLAY_CONTEXT_IMPL_H_

#include "AcGsDisplay.h"
#include "AcGiMatrix3d.h"
#include "AcGsSpatialQuery.h"

class AcGsDisplayContextImpl : public AcGsDisplayContext{
public:
  AcGsDisplayContextImpl();
  ~AcGsDisplayContextImpl() override;

  void setRender(AcGsRender*);
  AcGsRender* render() override;

  void setGsView(AcGsViewImpl*, const OdGeExtents2d& ext);
  AcGsView* view() override;

  bool intersectView(const AcGeExtents3d& extent, bool & fade) override;

  void setViewId(int id);
  int  viewId() override;

  void setFastInteractivity(bool fast_interactivity);
  bool fastInteractivity() const override;

  const AcGiMatrix3d* worldToDevice() const;

  const OdSiShape& queryShape(const AcGeExtents3d& ext) override;

private:
  AcGsRender*  render_;

  AcGsViewImpl* view_;

  OdGeExtents2d  extent_;

  int          view_id_;

  bool         fast_interactivity_;

  AcGiMatrix3d world_device_;

  AcGsSpatialQuery spatial_query;
};

#endif //_ACGS_DISPLAY_CONTEXT_IMPL_H_