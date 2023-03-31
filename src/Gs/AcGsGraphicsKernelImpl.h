#ifndef _ACGS_GRAPHICSKERNEL_IMPL_H_
#define _ACGS_GRAPHICSKERNEL_IMPL_H_

#include "acgs.h"

#include "base/observer_list.h"

class AcGsGraphicsKernelImpl : public AcGsGraphicsKernel {
public:
  AcGsGraphicsKernelImpl();
  ~AcGsGraphicsKernelImpl() override;

  AcGsModel * createModel(AcGsModel::RenderType type, Adesk::IntDbId databaseId, AcGsGetInterfaceFunc getFunc, AcGsReleaseInterfaceFunc releaseFunc) override;
  void deleteModel(AcGsModel *pModel) override;

  AcGsView * createView(const AcGsClientViewInfo * pClientViewInfo = nullptr, bool bEnableLayerVisibilityPerView = false) override;
  void deleteView(AcGsView *pView) override;

  AcGsDevice * createDevice(AcGsWindowingSystemID id, GS::DeviceType deviceType = GS::kScreenDevice) override;
  AcGsDevice * createOffScreenDevice(void) override;
  void deleteDevice(AcGsDevice *pDevice) override;

  void addReactor(AcGsReactor *pReactor) override;
  void removeReactor(AcGsReactor *pReactor) override;

  void addModelReactor(AcGsModel *pModel, AcGsModelReactor *pReactor) override;
  void removeModelReactor(AcGsModel *pModel, AcGsModelReactor *pReactor) override;

  void associate(Adesk::LongPtr databasePtr, AcGsModel *pModel, AcGsModel::RenderType renderType) override;
  AcGsModel* find(Adesk::LongPtr databasePtr, AcGsModel::RenderType renderType) override;

private:
  base::ObserverList<AcGsReactor> reactors_;
};

#endif //_ACGS_GRAPHICSKERNEL_IMPL_H_