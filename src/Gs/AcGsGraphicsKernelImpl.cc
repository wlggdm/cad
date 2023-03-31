#include "AcGsGraphicsKernelImpl.h"
#include "AcGsModelImpl.h"
#include "AcGsViewImpl.h"
#include "AcGsDeviceImpl.h"

AcGsGraphicsKernelImpl::AcGsGraphicsKernelImpl() {

}

AcGsGraphicsKernelImpl::~AcGsGraphicsKernelImpl() {

}

AcGsModel * AcGsGraphicsKernelImpl::createModel(AcGsModel::RenderType type, Adesk::IntDbId databaseId, AcGsGetInterfaceFunc getFunc, AcGsReleaseInterfaceFunc releaseFunc) {
  AcGsModelImpl* model = new AcGsModelImpl(type, databaseId, getFunc, releaseFunc);
  model->setGraphicsKernel(this);
  return model;
}

void AcGsGraphicsKernelImpl::deleteModel(AcGsModel *pModel) {
  AcGsModelImpl* model = static_cast<AcGsModelImpl*>(pModel);
  delete model;
}

AcGsView * AcGsGraphicsKernelImpl::createView(const AcGsClientViewInfo * pClientViewInfo /*= nullptr*/, bool bEnableLayerVisibilityPerView /*= false*/) {
  AcGsViewImpl* view = new AcGsViewImpl;
  view->setClientViewInfo(*pClientViewInfo);
  view->setGraphicsKernel(this);
  return view;
}

void AcGsGraphicsKernelImpl::deleteView(AcGsView *pView) {
  AcGsViewImpl* view = static_cast<AcGsViewImpl*>(pView);
  delete view;
}

AcGsDevice * AcGsGraphicsKernelImpl::createDevice(AcGsWindowingSystemID id, GS::DeviceType deviceType /*= GS::kScreenDevice*/) {
  AcGsDeviceImpl* device = new AcGsDeviceImpl;
  device->setWindowSystemID(id);
  device->setGraphicsKernel(this);
  return device;
}

AcGsDevice * AcGsGraphicsKernelImpl::createOffScreenDevice(void) {
  return nullptr;
}

void AcGsGraphicsKernelImpl::deleteDevice(AcGsDevice *pDevice) {
  AcGsDeviceImpl* device = static_cast<AcGsDeviceImpl*>(pDevice);
  delete device;
}

void AcGsGraphicsKernelImpl::addReactor(AcGsReactor *pReactor) {
  reactors_.AddObserver(pReactor);
}

void AcGsGraphicsKernelImpl::removeReactor(AcGsReactor *pReactor) {
  reactors_.RemoveObserver(pReactor);
}

void AcGsGraphicsKernelImpl::addModelReactor(AcGsModel *pModel, AcGsModelReactor *pReactor) {
  AcGsModelImpl* model = static_cast<AcGsModelImpl*>(pModel);
  model->addReactor(pReactor);
}

void AcGsGraphicsKernelImpl::removeModelReactor(AcGsModel *pModel, AcGsModelReactor *pReactor) {
  AcGsModelImpl* model = static_cast<AcGsModelImpl*>(pModel);
  model->removeReactor(pReactor);
}

void AcGsGraphicsKernelImpl::associate(Adesk::LongPtr databasePtr, AcGsModel *pModel, AcGsModel::RenderType renderType) {
  
}

AcGsModel* AcGsGraphicsKernelImpl::find(Adesk::LongPtr databasePtr, AcGsModel::RenderType renderType) {
  return nullptr;
}
