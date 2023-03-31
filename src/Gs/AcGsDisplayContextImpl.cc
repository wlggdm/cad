#include "AcGsDisplayContextImpl.h"

AcGsDisplayContextImpl::AcGsDisplayContextImpl() : 
  render_(nullptr), view_(nullptr), view_id_(0), fast_interactivity_(false){
}

AcGsDisplayContextImpl::~AcGsDisplayContextImpl() {
}

void AcGsDisplayContextImpl::setRender(AcGsRender* render) {
  render_ = render;
}

AcGsRender* AcGsDisplayContextImpl::render() {
  return render_;
}

void AcGsDisplayContextImpl::setGsView(AcGsViewImpl* view, const OdGeExtents2d& ext) {
  view_ = view;

  extent_ = ext;

  world_device_ = view->worldToDeviceMatrix();
}

AcGsView* AcGsDisplayContextImpl::view() {
  return view_;
}


bool AcGsDisplayContextImpl::intersectView(const AcGeExtents3d& extent, bool& fade) {
  static OdGeExtents2d ext;

  (world_device_ * extent).convert2d(ext);

  if (ext.isDisjoint(extent_))
    return false;

  if (ext.maxPoint().x - ext.minPoint().x <= 1 ||
      ext.maxPoint().y - ext.minPoint().y <= 1) {
    fade = true;
  }

  return true;
}

void AcGsDisplayContextImpl::setViewId(int id){
  view_id_ = id;
}

int AcGsDisplayContextImpl::viewId(){
  return view_id_;
}

void AcGsDisplayContextImpl::setFastInteractivity(bool fast_interactivity) {
  fast_interactivity_ = fast_interactivity;
}

bool AcGsDisplayContextImpl::fastInteractivity() const {
  return fast_interactivity_;
}

const AcGiMatrix3d* AcGsDisplayContextImpl::worldToDevice() const {
  return &world_device_;
}

const OdSiShape& AcGsDisplayContextImpl::queryShape(const AcGeExtents3d& ext) {
  double x = /*fastInteractivity() ? 1 : */0;
  return spatial_query.build(*view_, ext, 0, reinterpret_cast<const AcGePoint2d*>(0), x);
}
