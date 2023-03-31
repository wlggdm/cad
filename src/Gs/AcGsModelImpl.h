#ifndef _ACGS_MODEL_IMPL_H_
#define _ACGS_MODEL_IMPL_H_

#include "acgs.h"

#include "base/observer_list.h"

#include "DbDatabaseReactor.h"
#include "StaticRxObject.h"
#include "SlotManager.h"

class AcGsModelImpl : public AcGsModel, 
                      public OdStaticRxObject<OdDbDatabaseReactor>,
                      public OdSlotManager{
public:
  ODRX_HEAP_OPERATORS()

  AcGsModelImpl(AcGsModel::RenderType type, Adesk::IntDbId databaseId,
               AcGsGetInterfaceFunc getFunc, AcGsReleaseInterfaceFunc releaseFunc);
  ~AcGsModelImpl() override;

  void setGraphicsKernel(AcGsGraphicsKernel* graphics_kernel);
  //AcGsModel
  AcGsGraphicsKernel & graphicsKernel(void) override;

  bool addSceneGraphRoot(AcGiDrawable * pRoot) override;
  bool eraseSceneGraphRoot(AcGiDrawable * pRoot) override;

  void onAdded(AcGiDrawable * pAdded, AcGiDrawable * pParent) override;
  void onAdded(AcGiDrawable * pAdded, Adesk::IntDbId parentID) override;

  void onModified(AcGiDrawable * pModified, AcGiDrawable * pParent) override;
  void onModified(AcGiDrawable * pModified, Adesk::IntDbId parentID) override;

  void onErased(AcGiDrawable * pErased, AcGiDrawable * pParent) override;
  void onErased(AcGiDrawable * pErased, Adesk::IntDbId parentID) override;

  void onPaletteModified(void) override;

  void setTransform(const AcGeMatrix3d &) override;
  AcGeMatrix3d transform(void) const override;

  void setExtents(const AcGePoint3d &, const AcGePoint3d &) override;
  void invalidate(InvalidationHint hint) override;
  void setViewClippingOverride(bool bOverride) override;

  void setVisualStyle(const AcDbObjectId visualStyleId) override;
  AcDbObjectId visualStyle(void) const override;

  void setVisualStyle(const AcGiVisualStyle &visualStyle) override;
  bool visualStyle(AcGiVisualStyle &visualStyle) const override;

  void setBackground(const AcDbObjectId backgroundId) override;
  AcDbObjectId background(void) const override;

  void enableLinetypes(bool bEnable) override;
  bool linetypesEnabled(void) const override;

  void setEnableSectioning(bool enable) override;
  bool isSectioningEnabled(void) const override;
  bool setSectioning(const AcGePoint3dArray & pts, const AcGeVector3d & upVector) override;
  bool setSectioning(const AcGePoint3dArray & pts, const AcGeVector3d & upVector, double top, double bottom) override;

  void setSectioningVisualStyle(const AcDbObjectId visualStyleId) override;

  RenderType renderType(void) override;

  double shadowPlaneLocation(void) const override;
  void setShadowPlaneLocation(double planeLocationZ) override;

  void setSelectable(bool bEnable) override;
  bool selectable(void) const override;

  void setDrawOrder(AcGiDrawable* pDrawable, AcGiDrawable* pRelativeDrawable, AcGiDrawable* pParent, OrderActionType action) override;
  bool bounds(const AcGiDrawable& drawable, AcGePoint3d& minPt, AcGePoint3d& maxPt) override;

  void onModified(AcGiDrawable * pModified, AcGiDrawable * pParent, InvalidationHint hint) override;
  void onModified(AcGiDrawable * pModified, Adesk::IntDbId parentID, InvalidationHint hint) override;

  void setUnitScale(double metersPerUnit) override;
  double unitScale(void) const override;

  void setLightingPhotometric(bool bPhotometric) override;
  bool isLightingPhotometric(void) const override;

  void setLightsInBlocksEnabled(bool bEnabled) override;
  bool lightsInBlocksEnabled(void) const override;

  void addReactor(AcGsModelReactor *pReactor);
  void removeReactor(AcGsModelReactor *pReactor);

  //OdDbDatabaseReactor
  void objectAppended(const OdDbDatabase* pDb, const OdDbObject* pObject) override;

  void objectModified(const OdDbDatabase* pDb, const OdDbObject* pObject) override;

  void objectErased(const OdDbDatabase* pDb, const OdDbObject* pObject,bool erased = true) override;

  //self
  void addViewRef(AcGsView*);
  void releaseViewRef(AcGsView*);
  int  getViewId(AcGsView*);

  void invalidateViews();
private:
  AcGsGraphicsKernel* graphics_kernel_;

  AcGsGetInterfaceFunc     open_drawable_;
  AcGsReleaseInterfaceFunc release_drawable_;

  RenderType render_type_;

  Adesk::IntDbId database_id_;

  base::ObserverList<AcGsModelReactor> reactors_;

  AcGeMatrix3d transform_;

  double unit_scale_;

  bool selectable_;

  struct ViewRef {
    AcGsView* view;
    OdUInt32  count;
    OdSlotId  slot;
  };

  std::vector<ViewRef> view_ids_;

  std::vector<AcGiDrawable*> drawable_roots_;
};

#endif //_ACGS_MODEL_IMPL_H_