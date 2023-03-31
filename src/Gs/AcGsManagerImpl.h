#ifndef _ACGS_MANAGER_IMPL_H_
#define _ACGS_MANAGER_IMPL_H_

#include "AcGsManager.h"

class AcGsManagerImpl : public AcGsManager {

public:
  AcGsModel* createAutoCADModel(AcGsGraphicsKernel &kernel) override;

  AcGsView* createAutoCADViewport(AcGsGraphicsKernel &kernel, AcDbViewportTableRecord *pVTR) override;
  AcGsView* createAutoCADView(AcGsGraphicsKernel &kernel, AcGiDrawable *pDrawable) override;

  AcGsDevice* createAutoCADDevice(AcGsGraphicsKernel &kernel, AcGsWindowingSystemID id) override;

  AcGsDevice* createAutoCADOffScreenDevice(AcGsGraphicsKernel &kernel) override;

  void destroyAutoCADModel(AcGsModel *pModel) override;

  void destroyAutoCADView(AcGsView *pView) override;

  void destroyAutoCADDevice(AcGsDevice *pDevice) override;

  AcGsView* createViewport(AcGsDevice* pDevice, AcDbViewportTableRecord *pVTR) override;

  AcGsView* createView(AcGsDevice* pDevice) override;

  void destroyView(AcGsView*, AcDbViewportTableRecord* pVTR = nullptr) override;

  AcGsModel* getDBModel(AcGsGraphicsKernel &kernel) override;

  AcGsDevice* getGUIDevice(AcGsGraphicsKernel &kernel) override;

  void setGsModel(const AcDbDatabase* pDb, AcGsModel* pModel) override;

  AcGsModel* gsModel(const AcDbDatabase* pDb) const override;

  void setGsHighlightModel(const AcDbDatabase* pDb, AcGsModel* pModel) override;

  AcGsModel* gsHighlightModel(const AcDbDatabase* pDb) const override;

  AcGsDevice* getOffScreenDevice(AcGsGraphicsKernel &kernel) override;

  AcGsView* getOffScreenView(AcGsGraphicsKernel &kernel, AcGsClientViewInfo &clientViewInfo) override;

  bool hasGsModel(const AcDbDatabase* pDb) const override;

  bool invalidateGsModel(const AcDbDatabase* pDb) override;
};

#endif //_ACGS_MANAGER_IMPL_H_