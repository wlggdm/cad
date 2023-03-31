#include "AcGsModelImpl.h"
#include "AcGsContainerNode.h"

AcGsModelImpl::AcGsModelImpl(AcGsModel::RenderType type, Adesk::IntDbId databaseId,
                             AcGsGetInterfaceFunc getFunc, AcGsReleaseInterfaceFunc releaseFunc) :
  render_type_(type),
  database_id_(databaseId),
  open_drawable_(getFunc),
  release_drawable_(releaseFunc),
  selectable_(true) {

}

AcGsModelImpl::~AcGsModelImpl() {
}

void AcGsModelImpl::setGraphicsKernel(AcGsGraphicsKernel* graphics_kernel) {
  graphics_kernel_ = graphics_kernel;
}

AcGsGraphicsKernel & AcGsModelImpl::graphicsKernel(void) {
  return *graphics_kernel_;
}

bool AcGsModelImpl::addSceneGraphRoot(AcGiDrawable * pRoot) {
  for (AcGsModelReactor& reactor : reactors_)
    reactor.onSceneGraphRootAdded(this, pRoot);

  auto iter = std::find(drawable_roots_.begin(), drawable_roots_.end(), pRoot);
  if (iter == drawable_roots_.end()) {
    drawable_roots_.push_back(pRoot);

    AcGsContainerNode* container = new AcGsContainerNode;
    pRoot->setGsNode(reinterpret_cast<OdGsCache*>(container));
    container->setDrawable(pRoot);
  }

  return true;
}

bool AcGsModelImpl::eraseSceneGraphRoot(AcGiDrawable * pRoot) {
  for (AcGsModelReactor& reactor : reactors_)
    reactor.onSceneGraphRootErased(this, pRoot);

  auto iter = std::find(drawable_roots_.begin(), drawable_roots_.end(), pRoot);
  if (iter != drawable_roots_.end()) {
    AcGsContainerNode* container = reinterpret_cast<AcGsContainerNode*>(pRoot->gsNode());
    pRoot->setGsNode(nullptr);
    container->setDrawable(nullptr);
    delete container;

    drawable_roots_.erase(iter);
  }

  return true;
}

void AcGsModelImpl::onAdded(AcGiDrawable * pAdded, AcGiDrawable * pParent) {
  for (AcGsModelReactor& reactor : reactors_)
    reactor.onAdded(this, pAdded, pParent);

  if (pParent && pParent->gsNode()) {
    AcGsContainerNode* parent_node = reinterpret_cast<AcGsContainerNode*>(pParent->gsNode());
    if (parent_node->type() == AcGsNodeImpl::kContainer){
      parent_node->add(pAdded);
      invalidateViews();
    }
  }
}

void AcGsModelImpl::onAdded(AcGiDrawable * pAdded, Adesk::IntDbId parentID) {
  onAdded(pAdded, open_drawable_(parentID, false));
}

void AcGsModelImpl::onModified(AcGiDrawable * pModified, AcGiDrawable * pParent) {
  for (AcGsModelReactor& reactor : reactors_)
    reactor.onModified(this, pModified, pParent);

  if (pModified && pModified->gsNode()) {
    AcGsEntityNode* modified_node = reinterpret_cast<AcGsEntityNode*>(pModified->gsNode());
    if (modified_node->type() == AcGsNodeImpl::kEntity) {
      //modified_node->setUpdateToDate(false);
      invalidateViews();
    }
  }
}

void AcGsModelImpl::onModified(AcGiDrawable * pModified, Adesk::IntDbId parentID) {
  onModified(pModified, open_drawable_(parentID, false));
}

void AcGsModelImpl::onModified(AcGiDrawable * pModified, AcGiDrawable * pParent, InvalidationHint hint) {
  for (AcGsModelReactor& reactor : reactors_)
    reactor.onModified(this, pModified, pParent);
}

void AcGsModelImpl::onModified(AcGiDrawable * pModified, Adesk::IntDbId parentID, InvalidationHint hint) {
  onModified(pModified, open_drawable_(parentID, false), hint);
}

void AcGsModelImpl::onErased(AcGiDrawable * pErased, AcGiDrawable * pParent) {
  for (AcGsModelReactor& reactor : reactors_)
    reactor.onErased(this, pErased, pParent);

  if (pParent && pParent->gsNode()) {
    AcGsContainerNode* parent_node = reinterpret_cast<AcGsContainerNode*>(pParent->gsNode());
    if (parent_node->type() == AcGsNodeImpl::kContainer) {
      parent_node->remove(reinterpret_cast<AcGsEntityNode*>(pErased->gsNode()));
      invalidateViews();
    }
  }
}

void AcGsModelImpl::onErased(AcGiDrawable * pErased, Adesk::IntDbId parentID) {
  onErased(pErased, open_drawable_(parentID, false));
}

void AcGsModelImpl::setUnitScale(double metersPerUnit) {
  unit_scale_ = metersPerUnit;
}

double AcGsModelImpl::unitScale(void) const {
  return unit_scale_;
}

void AcGsModelImpl::setLightingPhotometric(bool bPhotometric) {
  
}

bool AcGsModelImpl::isLightingPhotometric(void) const {
  return false;
}

void AcGsModelImpl::setLightsInBlocksEnabled(bool bEnabled) {
  
}

bool AcGsModelImpl::lightsInBlocksEnabled(void) const {
  return false;
}

void AcGsModelImpl::onPaletteModified(void) {
  
}

void AcGsModelImpl::setTransform(const AcGeMatrix3d & transform) {
  transform_ = transform;
}

AcGeMatrix3d AcGsModelImpl::transform(void) const {
  return transform_;
}

void AcGsModelImpl::setExtents(const AcGePoint3d &, const AcGePoint3d &) {
  
}

void AcGsModelImpl::invalidate(InvalidationHint hint) {
  
}

void AcGsModelImpl::setViewClippingOverride(bool bOverride) {
  
}

void AcGsModelImpl::setVisualStyle(const AcDbObjectId visualStyleId) {
  
}

void AcGsModelImpl::setVisualStyle(const AcGiVisualStyle &visualStyle) {
  
}

AcDbObjectId AcGsModelImpl::visualStyle(void) const {
  return AcDbObjectId();
}

bool AcGsModelImpl::visualStyle(AcGiVisualStyle &visualStyle) const {
  return false;
}

void AcGsModelImpl::setBackground(const AcDbObjectId backgroundId) {
}

AcDbObjectId AcGsModelImpl::background(void) const {
  return false;
}

void AcGsModelImpl::enableLinetypes(bool bEnable) {
  
}

bool AcGsModelImpl::linetypesEnabled(void) const {
  return true;
}

void AcGsModelImpl::setEnableSectioning(bool enable) {
  
}

bool AcGsModelImpl::isSectioningEnabled(void) const {
  return true;
}

bool AcGsModelImpl::setSectioning(const AcGePoint3dArray & pts, const AcGeVector3d & upVector) {
  return true;
}

bool AcGsModelImpl::setSectioning(const AcGePoint3dArray & pts, const AcGeVector3d & upVector, double top, double bottom) {
  return true;
}

void AcGsModelImpl::setSectioningVisualStyle(const AcDbObjectId visualStyleId) {
  
}

AcGsModel::RenderType AcGsModelImpl::renderType(void) {
  return render_type_;
}

double AcGsModelImpl::shadowPlaneLocation(void) const {
  return 1.0;
}

void AcGsModelImpl::setShadowPlaneLocation(double planeLocationZ) {
  
}

void AcGsModelImpl::setSelectable(bool bEnable) {
  selectable_ = bEnable;
}

bool AcGsModelImpl::selectable(void) const {
  return selectable_;
}

void AcGsModelImpl::setDrawOrder(AcGiDrawable* pDrawable, AcGiDrawable* pRelativeDrawable, AcGiDrawable* pParent, OrderActionType action) {
  AcGsEntityNode* drawable_node = reinterpret_cast<AcGsEntityNode*>(pDrawable->gsNode());
  AcGsEntityNode* relative_node = reinterpret_cast<AcGsEntityNode*>(pRelativeDrawable->gsNode());

  switch (action) {
    case kMoveBelow: {
      if (drawable_node->next() != relative_node) {
        drawable_node->RemoveFromList();
        drawable_node->InsertBefore(relative_node);
      }
      break;
    }
    case kMoveAbove: {
      if (drawable_node->previous() != relative_node) {
        drawable_node->RemoveFromList();
        drawable_node->InsertAfter(relative_node);
      }
      break;
    }
    case kSwapOrder: {
      //insert befor or after both ok
      base::LinkNode<AcGsEntityNode> temp;
      temp.InsertBefore(drawable_node);
      drawable_node->RemoveFromList();
      drawable_node->InsertBefore(relative_node);
      relative_node->RemoveFromList();
      relative_node->InsertBefore(&temp);
      temp.RemoveFromList();
      break;
    }
  }
}

bool AcGsModelImpl::bounds(const AcGiDrawable& drawable, AcGePoint3d& minPt, AcGePoint3d& maxPt) {
  return true;
}

void AcGsModelImpl::addReactor(AcGsModelReactor *pReactor) {
  reactors_.AddObserver(pReactor);
}

void AcGsModelImpl::removeReactor(AcGsModelReactor *pReactor) {
  reactors_.RemoveObserver(pReactor);
}

void AcGsModelImpl::objectAppended(const OdDbDatabase* pDb, const OdDbObject* pObject){
  OdDbStub* object_id = pObject->ownerId();
  onAdded(const_cast<OdDbObject*>(pObject), reinterpret_cast<Adesk::IntDbId>(object_id));
}

void AcGsModelImpl::objectModified(const OdDbDatabase* pDb, const OdDbObject* pObject){
  OdDbStub* object_id = pObject->ownerId();
  onModified(const_cast<OdDbObject*>(pObject), reinterpret_cast<Adesk::IntDbId>(object_id));
}

void AcGsModelImpl::objectErased(const OdDbDatabase* pDb, const OdDbObject* pObject, bool erased /*= true*/){
  OdDbStub* object_id = pObject->ownerId();
  onErased(const_cast<OdDbObject*>(pObject), reinterpret_cast<Adesk::IntDbId>(object_id));
}

void AcGsModelImpl::addViewRef(AcGsView* view){
  if (getViewId(view) == -1)
     view_ids_.push_back({view, 1, newSlot()});
}

void AcGsModelImpl::releaseViewRef(AcGsView* view){
  for (auto iter = view_ids_.begin(); iter != view_ids_.end(); ++iter) {
    if (iter->view != view)
      continue;

    if (--iter->count == 0) {
      freeSlot(iter->slot);
      view_ids_.erase(iter);
    }

    break;
  }
}

int AcGsModelImpl::getViewId(AcGsView* view) {
  for (auto iter : view_ids_){
    if (iter.view == view)
      return iter.slot;
  }

  return -1;
}

void AcGsModelImpl::invalidateViews() {
  for (auto& iter : view_ids_)
    iter.view->invalidate();
}
