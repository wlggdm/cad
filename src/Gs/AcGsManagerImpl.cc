#include "AcGsManagerImpl.h"

#include "AcGsModelImpl.h"
#include "AcGsViewImpl.h"
#include "AcGsDeviceImpl.h"
#include "AcGsGraphicsKernelImpl.h"

//#include "oda/source/database/DbDatabaseImpl.h"

namespace{
  AcGiDrawable*  AcGsGetInterfaceFunction(Adesk::IntDbId objectId, bool bNeedsValidation) {
    OdDbObjectId object_id(reinterpret_cast<OdDbStub*>(objectId));
    return (AcGiDrawable*)(object_id.openObject().get());
  }

  void AcGsReleaseInterfaceFunction(AcGiDrawable* drawable) {
    drawable->release();
  }
}

AcGsModel* AcGsManagerImpl::createAutoCADModel(AcGsGraphicsKernel &kernel) {
  AcGsModel* model = kernel.createModel(AcGsModel::kMain, 0, 
    AcGsGetInterfaceFunction, AcGsReleaseInterfaceFunction);
  return model;
}

AcGsView* AcGsManagerImpl::createAutoCADViewport(AcGsGraphicsKernel &kernel, AcDbViewportTableRecord *pVTR) {
  if (!pVTR)
    return nullptr;

  AcGsClientViewInfo client_view_info;
  client_view_info.viewportObjectId = reinterpret_cast<Adesk::IntDbId>((OdDbStub*)pVTR->objectId());

  AcGsView* view = kernel.createView(&client_view_info, true);
  return view;
}

AcGsView* AcGsManagerImpl::createAutoCADView(AcGsGraphicsKernel &kernel, AcGiDrawable *pDrawable) {
  return nullptr;
}

AcGsDevice* AcGsManagerImpl::createAutoCADDevice(AcGsGraphicsKernel &kernel, AcGsWindowingSystemID id) {
  AcGsDevice* device = kernel.createDevice(id);
  return device;
}

AcGsDevice* AcGsManagerImpl::createAutoCADOffScreenDevice(AcGsGraphicsKernel &kernel) {
  return nullptr;
}

void AcGsManagerImpl::destroyAutoCADModel(AcGsModel *pModel) {
  AcGsModelImpl* model = static_cast<AcGsModelImpl*>(pModel);
  delete model;
}

void AcGsManagerImpl::destroyAutoCADView(AcGsView *pView) {
  AcGsViewImpl* view = static_cast<AcGsViewImpl*>(pView);
  delete view;
}

void AcGsManagerImpl::destroyAutoCADDevice(AcGsDevice *pDevice) {
  AcGsDeviceImpl* device = static_cast<AcGsDeviceImpl*>(pDevice);
  delete device;
}

AcGsView* AcGsManagerImpl::createViewport(AcGsDevice* pDevice, AcDbViewportTableRecord *pVTR) {
  if (pDevice == nullptr || pVTR == nullptr)
    return nullptr;


  return nullptr;
}

AcGsView* AcGsManagerImpl::createView(AcGsDevice* pDevice) {
  return nullptr;
}

void AcGsManagerImpl::destroyView(AcGsView* view, AcDbViewportTableRecord* pVTR /*= nullptr*/) {
  
}

AcGsModel* AcGsManagerImpl::getDBModel(AcGsGraphicsKernel &kernel) {
  return nullptr;
}

AcGsDevice* AcGsManagerImpl::getGUIDevice(AcGsGraphicsKernel &kernel) {
  return nullptr;
}

void AcGsManagerImpl::setGsModel(const AcDbDatabase* pDb, AcGsModel* pModel) {
  //pDb->addReactor(reinterpret_cast<AcGsModelImpl*>(pModel));
}

AcGsModel* AcGsManagerImpl::gsModel(const AcDbDatabase* pDb) const {
  return nullptr;
}

void AcGsManagerImpl::setGsHighlightModel(const AcDbDatabase* pDb, AcGsModel* pModel) {
  
}

AcGsModel* AcGsManagerImpl::gsHighlightModel(const AcDbDatabase* pDb) const {
  return nullptr;
}

AcGsDevice* AcGsManagerImpl::getOffScreenDevice(AcGsGraphicsKernel &kernel) {
  return nullptr;
}

AcGsView* AcGsManagerImpl::getOffScreenView(AcGsGraphicsKernel &kernel, AcGsClientViewInfo &clientViewInfo) {
  return nullptr;
}

bool AcGsManagerImpl::hasGsModel(const AcDbDatabase* pDb) const {
  //const OdDbDatabaseImpl* pDbImpl = OdDbDatabaseImpl::getImpl(pDb);
  //return !pDbImpl->m_Reactors.empty();
  return false;
}

bool AcGsManagerImpl::invalidateGsModel(const AcDbDatabase* pDb) {
  return true;
}

AcGsGraphicsKernel*  AcGsManager::acquireGraphicsKernel() {
  return new AcGsGraphicsKernelImpl();
}

void  AcGsManager::releaseGraphicsKernel(AcGsGraphicsKernel *pKernel) {
  delete (AcGsGraphicsKernelImpl*)pKernel;
}