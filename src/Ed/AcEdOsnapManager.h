#ifndef   _ACED_OSNAP_MANAGER_H_
#define   _ACED_OSNAP_MANAGER_H_

#include "acgs.h"
#include "gs/GsSelectionReactor.h"

class AcEdOsnapManager : public OdGsSelectionReactor {
public:
  AcEdOsnapManager();
  ~AcEdOsnapManager();

  bool Snap(AcGsView* view, AcGePoint3d& pick_point, const AcGePoint3d* last_point);

  void ViewportDraw(OdGiViewportDraw*) const;

private:
  bool selected(const OdGiDrawableDesc& pDrawableDesc) override;

  OdUInt32 selected(const OdGiPathNode&, const OdGiViewport&) override;

  void SnapPoints(const AcDbEntity* entity, unsigned snap_mode, OdGsMarker, const AcGeMatrix3d& world_eye);

  void CheckPoint(OdDb::OsnapMode mode, const AcGePoint3d& point);

  void invalidateViewport(const AcGePoint3d&) const;
private:
  OdDb::OsnapMode osnap_mode_;

  AcGePoint3d     osnap_point_;

  double          osnap_dist_;

  AcGsView*       view_;

  AcGePoint3d     pick_point_;

  const AcGePoint3d*  last_point_;
};

#endif //_ACED_OSNAP_MANAGER_H_