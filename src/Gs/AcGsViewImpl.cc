#include "AcGsViewImpl.h"

#include "GeRay2d.h"
#include "GeLineSeg2d.h"

#include "gs/SiVolume.h"
#include "gs/GsSelectionReactor.h"
#include "OdRound.h"
#include "DbViewportTableRecord.h"

#include "AcGsDisplayContextImpl.h"
#include "AcGsModelImpl.h"
#include "AcGsContainerNode.h"
#include "AcGsDeviceImpl.h"
#include "AcGsRenderImpl.h"
#include "AcGsSpatialQuery.h"
#include "AcGsSelectVisitor.h"

#include "ui/compositor/context_provider.h"
#include "ui/gfx/geometry/rect.h"

namespace {
  const double PERSP_CONST = 42.0;

  bool isInside(const OdGePoint2d* pVts, int numVts, const OdGePoint2d& pt) {
    OdGeRay2d ray(OdGePoint2d(pt.x, pt.y), OdGeVector2d::kXAxis);
    int i, numInters(0);
    for (i = 0; i < numVts; i++){
      int next = (i + 1) % numVts;
      OdGePoint2d p1(pVts[i].x, pVts[i].y);
      OdGePoint2d p2(pVts[next].x, pVts[next].y);
      if (p1.isEqualTo(p2)) {
        if (ray.isOn(p1))
          ++numInters;
        continue;
      }
      OdGeLineSeg2d seg(p1, p2);
      OdGePoint2d intPt;
      if (ray.intersectWith(seg, intPt))
        numInters++;
    }
    return (numInters % 2) != 0;
  }

#define ODGS_GETSAFEDIRECTION(var) \
  OdGeVector3d var = target_ - position_; \
  if (var.isZeroLength()) \
    var = eye_vector_ * eye_length_;
}

AcGsViewImpl::DrawableHolder::DrawableHolder(OdGiDrawable* gidrawable, AcGsModel* gsmodel) {
  if (gidrawable->isPersistent()) {
    drawable_id = gidrawable->id();
    drawable = nullptr;
  } else {
    drawable_id = nullptr;
    drawable = gidrawable;
  }

  model = gsmodel;
}

AcGsViewImpl::AcGsViewImpl()  : 
  graphics_kernel_(nullptr),
  device_(nullptr),
  rendering_(false),
  view_flags_(0),
  border_color_({ 0 }),
  border_weight_(1),
  fast_interactivity_(false){
}

AcGsViewImpl::~AcGsViewImpl() {

}

void AcGsViewImpl::setDevice(AcGsDevice* device) {
  device_ = device;
}

void AcGsViewImpl::setGraphicsKernel(AcGsGraphicsKernel* graphics_kernel) {
  graphics_kernel_ = graphics_kernel;
}

AcGsGraphicsKernel & AcGsViewImpl::graphicsKernel(void) {
  return *graphics_kernel_;
}

void AcGsViewImpl::setViewport(const AcGePoint2d & lowerLeft, const AcGePoint2d & upperRight) {
  OdGeTol tol(1e-100);
  if (!lower_left_.isEqualTo(lowerLeft, tol) || !upper_right_.isEqualTo(upperRight, tol)) {
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    lower_left_ = lowerLeft;
    upper_right_ = upperRight;
    //initCullingVolume();
    invalidate();
  }
}

void AcGsViewImpl::setViewport(const AcGsDCRect & screen_rect) {
  if (screen_min_ != screen_rect.m_min || screen_max_ != screen_rect.m_max) {
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    screen_min_ = screen_rect.m_min;
    screen_max_ = screen_rect.m_max;
    //initCullingVolume();
    invalidate();
  }
}

void AcGsViewImpl::getViewport(AcGePoint2d & lowerLeft, AcGePoint2d & upperRight) const {
  lowerLeft = lower_left_;
  upperRight = upper_right_;
}

void AcGsViewImpl::getViewport(AcGsDCRect & screen_rect) const {
  screen_rect.m_min = screen_min_;
  screen_rect.m_max = screen_max_;
}

void AcGsViewImpl::setViewportClipRegion(int contours, int const * counts, AcGsDCPoint const * points) {
  OdGePoint2dArray::size_type num_points = 0;
  for (int j = 0; j < contours; ++j)
    num_points += counts[j];

  clip_counts_.clear();
  clip_counts_.insert(clip_counts_.end(), counts, counts + contours);

  clip_points_.resize(num_points);
  for (OdGePoint2dArray::size_type i = 0; i < num_points; ++i, ++points) {
    clip_points_[i].x = points->x;
    clip_points_[i].y = points->y;
  }
}

void AcGsViewImpl::removeViewportClipRegion(void) {
  clip_counts_.clear();
  clip_points_.clear();
}

void AcGsViewImpl::setViewportBorderProperties(AcGsColor const & color, int weight) {
  border_color_ = color;
  border_weight_ = weight;
}

void AcGsViewImpl::getViewportBorderProperties(AcGsColor & color, int & weight) const {
  color = border_color_;
  weight = border_weight_;
}

void AcGsViewImpl::setViewportBorderVisibility(bool bVisible) {
  SETBIT(view_flags_, kBorderVisible, bVisible);
}

bool AcGsViewImpl::isViewportBorderVisible(void) const {
  return GETBIT(view_flags_, kBorderVisible);
}

void AcGsViewImpl::setView(const AcGePoint3d & position, const AcGePoint3d & target, const AcGeVector3d& upVector, double fieldWidth, double fieldHeight, Projection projection /*= kParallel*/) {
  OdGeTol tol(1e-100);
  if (position_.isEqualTo(position, tol) &&
    target_.isEqualTo(target, tol) &&
    up_vector_.isEqualTo(upVector, tol) &&
    OdEqual(field_width_, fieldWidth, tol.equalPoint()) &&
    OdEqual(field_height_, fieldHeight, tol.equalPoint()) &&
    isPerspective() == (projection == kPerspective))
    return;

  const double validateTol = 1.e-12;
  tol.setEqualPoint(validateTol); tol.setEqualVector(validateTol);
  const OdGeVector3d newEyeVector = position - target;
  if (!OdPositive(newEyeVector.length(), validateTol) || !OdPositive(upVector.length(), validateTol) ||
    newEyeVector.isParallelTo(upVector, tol) ||
    (fieldWidth < 0.0) || (fieldHeight < 0.0) ||
    (!OdPositive(fieldWidth, validateTol) && !OdPositive(fieldHeight, validateTol)))
    return;

  SETBIT(view_flags_, kWorldToDeviceValid, false);

  position_  = position;
  target_    = target;
  up_vector_ = upVector;

  eye_vector_   = newEyeVector;
  eye_length_   = eye_vector_.normalizeGetLength(); // seems to be need for perspective views
  x_vector_     = upVector.crossProduct(eye_vector_);
  field_width_  = fieldWidth;
  field_height_ = fieldHeight;

  setPerspectiveEnabled(projection == kPerspective);

  onWorldToEyeChanged();
}

AcGePoint3d AcGsViewImpl::position(void) const {
  return position_;
}

AcGePoint3d AcGsViewImpl::target(void) const {
  return target_;
}

AcGeVector3d AcGsViewImpl::upVector(void) const {
  return up_vector_;
}

bool AcGsViewImpl::isPerspective(void) const {
  return projection_ == Projection::kPerspective;
}

double AcGsViewImpl::fieldWidth(void) const {
  double dScHeight = windowAspect() * field_height_;
  if (dScHeight > field_width_)
    return dScHeight;

  return field_width_;
}

double AcGsViewImpl::fieldHeight(void) const {
  double winAspect = windowAspect();
  if (winAspect * field_height_ < field_width_)
    return field_width_ / winAspect;

  return field_height_;
}

void AcGsViewImpl::setEnableFrontClip(bool enable) {
  if (GETBIT(view_flags_, kDoFrontClip) != enable) {
    SETBIT(view_flags_, kDoFrontClip, enable);
    invalidate();
  }
}

bool AcGsViewImpl::isFrontClipped(void) const {
  return GETBIT(view_flags_, kDoFrontClip);
}

void AcGsViewImpl::setFrontClip(double distance) {
  if (!OdEqual(front_dist_, distance, OdGeContext::gZeroTol.equalPoint())) {
    front_dist_ = distance;
    invalidate();
  }
}

double AcGsViewImpl::frontClip(void) const {
  return front_dist_;
}

void AcGsViewImpl::setEnableBackClip(bool enable) {
  if (GETBIT(view_flags_, kDoBackClip) != enable) {
    SETBIT(view_flags_, kDoBackClip, enable);
    invalidate();
  }
}

bool AcGsViewImpl::isBackClipped(void) const {
  return GETBIT(view_flags_, kDoBackClip);;
}

void AcGsViewImpl::setBackClip(double distance) {
  if (!OdEqual(back_dist_, distance, OdGeContext::gZeroTol.equalPoint())) {
    back_dist_ = distance;
    invalidate();
  }
}

double AcGsViewImpl::backClip(void) const {
  return back_dist_;
}

AcGeMatrix3d AcGsViewImpl::viewingMatrix(void) const {
  if (!GETBIT(view_flags_, kWorldToEyeValid)) {
    world_eye_ = eyeToWorldMatrix().inverse();
    SETBIT(view_flags_, kWorldToEyeValid, true);
  }

  return world_eye_;
}

AcGeMatrix3d AcGsViewImpl::projectionMatrix(void) const {
  AcGeMatrix3d res;

  const double field_width  = fieldWidth();
  const double field_height = fieldHeight();

  res(0, 0) = 2 / field_width;
  res(1, 1) = 2 / field_height;

  constexpr double dFar  = -1E20;
  constexpr double dNear =  1E20;

  res(2, 2) = 1 / (dFar - dNear);
  res(2, 3) = -dNear / (dFar - dNear);
  
  return res;
}

AcGeMatrix3d AcGsViewImpl::screenMatrix(void) const {
  AcGeMatrix3d res;
  //change to view width
  res(0, 0) =  fabs(dcScreenMax().x - dcScreenMin().x) / 2.0;
  res(1, 1) = -fabs(dcScreenMax().y - dcScreenMin().y) / 2.0;

  res(0, 3) = (dcScreenMax().x + dcScreenMin().x) / 2.0;
  res(1, 3) = (dcScreenMax().y + dcScreenMin().y) / 2.0;

  return res;
}

AcGeMatrix3d AcGsViewImpl::worldToDeviceMatrix(void) const {
  if (!GETBIT(view_flags_, kWorldToDeviceValid)) {
    world_device_ = screenMatrix() * projectionMatrix() * viewingMatrix();
    SETBIT(view_flags_, kWorldToDeviceValid, true);
  }
  return world_device_;
}

AcGeMatrix3d AcGsViewImpl::objectToDeviceMatrix(void) const {
  return worldToDeviceMatrix();
}

bool AcGsViewImpl::add(AcGiDrawable * drawable, AcGsModel * model) {
  AcGsModel* drawable_mode = getModel(drawable);
  if (drawable_mode)
    return true;

  drawable_holders_.push_back({drawable, model});
  model->addSceneGraphRoot(drawable);

  static_cast<AcGsModelImpl*>(model)->addViewRef(this);

  return true;
}

bool AcGsViewImpl::erase(AcGiDrawable * drawable) {
  for (auto iter = drawable_holders_.begin(); iter != drawable_holders_.end(); ++iter) {
    OdGiDrawablePtr drawable_ptr = drawableAt(*iter);
    if (drawable_ptr.get() == drawable)
      continue;

    AcGsModelImpl* model = static_cast<AcGsModelImpl*>(iter->model);

    model->eraseSceneGraphRoot(drawable);

    model->releaseViewRef(this);

    drawable_holders_.erase(iter);
    break;
  }
  return true;
}

void AcGsViewImpl::eraseAll(void) {
  for (auto iter = drawable_holders_.begin(); iter != drawable_holders_.end(); ++iter) {
    OdGiDrawablePtr drawable = drawableAt(*iter);
    AcGsModelImpl* model = static_cast<AcGsModelImpl*>(iter->model);

    model->eraseSceneGraphRoot(drawable);

    model->releaseViewRef(this);
  }

  drawable_holders_.clear();
}

void AcGsViewImpl::invalidate(void) {
  int device_width  = screen_max_.x - screen_min_.x;
  int device_height = screen_max_.y - screen_min_.y;

  int left  = device_width * lower_left_.x;
  int bottom = device_height * lower_left_.y;

  int right = device_width * upper_right_.x;
  int top   = device_height * upper_right_.y;
  
  AcGsDCRect rect(left, right, bottom, top);

  invalidate(rect);
}

void AcGsViewImpl::invalidate(const AcGsDCRect &rect) {
  if (device_)
     device_->invalidate(rect);
}

bool AcGsViewImpl::isValid(void) const {
  return true;
}

void AcGsViewImpl::update(void) {
  static AcGsRenderImpl render;

  AcGsDeviceImpl* device = (AcGsDeviceImpl*)getDevice();
  if (device == nullptr)
    return;

  int height = fabs((screen_max_.y - screen_min_.y) * (upper_right_.y - lower_left_.y));
  int width  = fabs((screen_max_.x - screen_min_.x) * (upper_right_.x - lower_left_.x));
  if (width < 10 || height < 10)
    return;

  gfx::Rect rect(0, 0, width, height);

  HWND hwnd = reinterpret_cast<HWND>(device->windowSystemID());
  ContextProvider* context_provider = ContextProvider::GetInstance();
  if (context_provider->BeginFrame(hwnd, rect)) {
    //clip
    context_provider->ContextDirectX()->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(255, 0, 0, 0), 0, 0);

    AcGeMatrix3d view_mat = viewingMatrix();
    AcGeMatrix3d project_mat = projectionMatrix();
    AcGeMatrix3d screen_mat = screenMatrix();

    AcGeMatrix3d view_project = project_mat * view_mat;
    //AcGeMatrix3d world_device = screen_mat * view_project;

    XMFLOAT4X4 world_project;
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j)
        world_project(i, j) = view_project(i, j);
    }

    context_provider->GetBatchedElements()->Begin(world_project);

    AcGsDisplayContextImpl context;
    OdGeExtents2d extent(OdGePoint2d(0, 0), OdGePoint2d(width,height));
    context.setRender(&render);
    context.setGsView(this, extent);
    context.setFastInteractivity(fast_interactivity_);

    AcGsModelImpl* model = nullptr;
    for (auto iter = drawable_holders_.begin(); iter != drawable_holders_.end(); ++iter) {
      model = static_cast<AcGsModelImpl*>(iter->model);
      int view_id = model->getViewId(this);
      context.setViewId(view_id);

      OdGiDrawablePtr drawable = drawableAt(*iter);
      if (!drawable.isNull() && drawable->gsNode()) {
        AcGsContainerNode* container = reinterpret_cast<AcGsContainerNode*>(drawable->gsNode());
        container->display(context);
      } else {
        //iter->drawable->worldDraw(context.worlddraw());
      }
    }

    context_provider->EndFrame(hwnd, rect);
  }
}

void AcGsViewImpl::update(GS::SyncBehavior sync) {
 
}

bool AcGsViewImpl::isPointInViewport(const Adesk::Int32 x, const Adesk::Int32 y) {
  OdGePoint2d pt(x, y);
  int numConts = clip_counts_.size();
  if (numConts) {
    int numContsPtInside(0), startPt(0), i;
    for (i = 0; i < numConts; i++){
      int numVerts = clip_counts_[i];
      if (isInside(clip_points_.getPtr() + startPt, numVerts, pt))
        numContsPtInside++;
      startPt += numVerts;
    }
    return (numContsPtInside % 2) != 0;
  }
  else {
    AcGsDCPoint pt1, pt2;
    screenRect(pt1, pt2);
    if (pt1.x > pt2.x)
      std::swap(pt1.x, pt2.x);
    if (pt1.y > pt2.y)
      std::swap(pt1.y, pt2.y);
    return (pt.x >= pt1.x && pt.x <= pt2.x &&
      pt.y >= pt1.y && pt.y <= pt2.y);
  }
}

void AcGsViewImpl::beginInteractivity(double fFrameRateInHz) {

}

void AcGsViewImpl::endInteractivity(void) {

}

bool AcGsViewImpl::isInteractive(void) const {
  return true;
}

void AcGsViewImpl::flush(void) {

}

void AcGsViewImpl::setMaximumUpgrade(int step) {

}

int AcGsViewImpl::maximumUpgrade(void) {
  return 10;
}

void AcGsViewImpl::setMinimumDegrade(int step) {

}

int AcGsViewImpl::minimumDegrade(void) {
  return 1;
}

void AcGsViewImpl::hide(void) {
  SETBIT(view_flags_, kHidden, true);
}

void AcGsViewImpl::show(void) {
  SETBIT(view_flags_, kHidden, false);
}

bool AcGsViewImpl::isVisible(void) {
  return !GETBIT(view_flags_, kHidden);
}

void AcGsViewImpl::freezeLayer(Adesk::IntDbId layerID) {
  //todo
}

void AcGsViewImpl::thawLayer(Adesk::IntDbId layerID) {
  //todo
}

void AcGsViewImpl::clearFrozenLayers(void) {
  //todo
}

void AcGsViewImpl::invalidateCachedViewportGeometry(void) {
}

void AcGsViewImpl::invalidateCachedViewportGeometry(AcGiDrawable* pDrawable) {
}

void AcGsViewImpl::dolly(const AcGeVector3d & vector) {
  dolly(vector.x, vector.y, vector.z);
}

void AcGsViewImpl::dolly(double x, double y, double z) {
  SETBIT(view_flags_, kWorldToDeviceValid, false);
  OdGeVector3d vec = x_vector_ * x + up_vector_ * y + eye_vector_ * z;
  position_ += vec;
  target_ += vec;

  onWorldToEyeChanged();
}

void AcGsViewImpl::roll(double angle) {
  SETBIT(view_flags_, kWorldToDeviceValid, false);
  up_vector_.rotateBy(angle, eye_vector_);
  x_vector_.rotateBy(angle, eye_vector_);

  onWorldToEyeChanged();
}

void AcGsViewImpl::orbit(double x, double y) {
  if (!OdZero(x)) {
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    ODGS_GETSAFEDIRECTION(dir);
    dir.rotateBy(x, x_vector_);
    eye_vector_ = dir;
    eye_length_ = eye_vector_.normalizeGetLength();
    up_vector_.rotateBy(x, x_vector_);
    position_ = target_ + dir;
  }

  if (!OdZero(y)) {
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    ODGS_GETSAFEDIRECTION(dir);
    dir.rotateBy(y, up_vector_);
    eye_vector_ = dir;
    eye_length_ = eye_vector_.normalizeGetLength();
    x_vector_ = up_vector_.crossProduct(eye_vector_);
    position_ = target_ + dir;
  }

  if (!OdZero(x) || !OdZero(y))
    onWorldToEyeChanged();
}

void AcGsViewImpl::zoom(double factor) {
  if (factor > 0.0) {
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    if (isPerspective()) {
      OdGeVector3d viewdir = target_ - position_;
      double distance = viewdir.normalizeGetLength();
      double focal = focalLength() + lens_length_;
      if (distance < focal) {
        position_ = target_ - viewdir*focal;
        viewdir = (target_ - position_).normalize();
        distance = focal;
      }

      if (distance < factor*lens_length_) {
        // move target and position
        target() += (factor - 1.)*viewdir*distance;
        position_ += (factor - 1.)*viewdir*distance;
      } else {
        // move position
        double revFactor = 1. / factor;
        field_height_ *= revFactor;
        field_width_ *= revFactor;
        position_ = target_ - viewdir*focal*revFactor;
      }
    } else {
      field_width_ /= factor;
      field_height_ /= factor;
    }

    onWorldToEyeChanged();
  }
}

void AcGsViewImpl::zoomExtents(const AcGePoint3d& minPoint, const AcGePoint3d& maxPoint) {
  // Compute current view plane in WCS space
  const OdGePoint3d cameraPos = target_;
  const OdGeVector3d cameraY = up_vector_.normal();
  const OdGeVector3d cameraX = x_vector_.normal();
  // Compute all box points
  const OdGeVector3d boxVecX(maxPoint.x - minPoint.x, 0.0, 0.0),
    boxVecY(0.0, maxPoint.y - minPoint.y, 0.0),
    boxVecZ(0.0, 0.0, maxPoint.z - minPoint.z);
  const OdGePoint3d boxPt[8] = { minPoint, minPoint + boxVecX, minPoint + boxVecY, minPoint + boxVecZ,
    maxPoint, maxPoint - boxVecX, maxPoint - boxVecY, maxPoint - boxVecZ };
  // Compute offset and new fieldWidth/fieldHeight
  OdGeVector3d diffVec;
  double fw, fh;
  if (!isPerspective()) {
    // Project extents onto view plane
    OdGeExtents2d projectedExt;
    {
      for (int nPt = 0; nPt < 8; nPt++)
        projectedExt.addPoint(OdGePoint2d((boxPt[nPt] - cameraPos).dotProduct(cameraX), (boxPt[nPt] - cameraPos).dotProduct(cameraY)));
    }
    // Compute center of extents
    const OdGePoint2d extCenter = projectedExt.minPoint() + (projectedExt.maxPoint() - projectedExt.minPoint()) * 0.5;
    diffVec = cameraX * extCenter.x + cameraY * extCenter.y;
    fw = projectedExt.maxPoint().x - projectedExt.minPoint().x;
    fh = projectedExt.maxPoint().y - projectedExt.minPoint().y;
  } else {
    const OdGeVector3d &cameraZ = eye_vector_;
    const double tgX = fieldWidth() / focalLength() * 0.5;
    const double tgY = fieldHeight() / focalLength() * 0.5;
    // Trace each box point to detect maximal fov angle
    double minWDist = DBL_MAX, minWPtX = 0.0, minWPtZ = 0.0;
    double maxWDist = DBL_MAX, maxWPtX = 0.0, maxWPtZ = 0.0;
    double minHDist = DBL_MAX, minHPtY = 0.0, minHPtZ = 0.0;
    double maxHDist = DBL_MAX, maxHPtY = 0.0, maxHPtZ = 0.0;
    for (int nPt = 0; nPt < 8; nPt++) {
      const double projX = (boxPt[nPt] - cameraPos).dotProduct(cameraX);
      const double projY = (boxPt[nPt] - cameraPos).dotProduct(cameraY);
      const double projZ = (boxPt[nPt] - cameraPos).dotProduct(cameraZ);
      const double distW = projZ * tgX;
      const double distH = projZ * tgY;
      if (projX - distW < minWDist) { minWDist = projX - distW; minWPtX = projX; minWPtZ = projZ; }
      if (-projX - distW < maxWDist) { maxWDist = -projX - distW; maxWPtX = projX; maxWPtZ = projZ; }
      if (projY - distH < minHDist) { minHDist = projY - distH; minHPtY = projY; minHPtZ = projZ; }
      if (-projY - distH < maxHDist) { maxHDist = -projY - distH; maxHPtY = projY; maxHPtZ = projZ; }
    }
    double szWX, szWZ, szHY, szHZ;
    double tz = -(tgX * (maxWPtZ - minWPtZ) + (maxWPtX - minWPtX)) / (tgX * 2);
    szWX = minWPtX - tgX * tz; szWZ = minWPtZ - tz;
    tz = -(tgY * (maxHPtZ - minHPtZ) + (maxHPtY - minHPtY)) / (tgY * 2);
    szHY = minHPtY - tgY * tz; szHZ = minHPtZ - tz;
    OdGePoint3d newPos(szWX, szHY, odmax(szWZ, szHZ));
    newPos.transformBy(eyeToWorldMatrix());
    OdGePoint3d newTarget(newPos - cameraZ * ((newPos - cameraPos).dotProduct(cameraZ)));
    const double newFL = (newTarget - newPos).length();
    fw = field_width_ / focalLength() * newFL;
    fh = field_height_ / focalLength() * newFL;
    diffVec = newTarget - cameraPos;
  }
  if (!diffVec.isZeroLength() || !OdEqual(field_width_, fw) || !OdEqual(field_height_, fh)) {
    position_ += diffVec; target_ += diffVec;
    field_width_ = fw; field_height_ = fh;
    // Update cached transforms
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    onWorldToEyeChanged();
  }
}

void AcGsViewImpl::zoomWindow(const AcGePoint2d& lowerLeft, const AcGePoint2d& upperRight) {
  bool bPerspective = isPerspective();
  const OdGeMatrix3d projMtx = projectionMatrix();

  OdGePoint3d eyePtLL(lowerLeft.x, lowerLeft.y, (bPerspective) ? projMtx(2, 3) : 0.0),
    eyePtUR(upperRight.x, upperRight.y, (bPerspective) ? projMtx(2, 3) : 0.0);
  const OdGeMatrix3d toEyeMtx = (screenMatrix() * projMtx).inverse();
  eyePtLL.transformBy(toEyeMtx); eyePtUR.transformBy(toEyeMtx);

  OdGeVector3d diffVec;
  double fw, fh;
  if (!bPerspective) {
    const OdGeVector3d cameraY = up_vector_.normal();
    const OdGeVector3d cameraX = x_vector_.normal();
    const OdGePoint2d ptMin(odmin(eyePtLL.x, eyePtUR.x), odmin(eyePtLL.y, eyePtUR.y)),
      ptMax(odmax(eyePtLL.x, eyePtUR.x), odmax(eyePtLL.y, eyePtUR.y));
    diffVec = cameraX * (ptMin.x + (ptMax.x - ptMin.x) * 0.5) +
      cameraY * (ptMin.y + (ptMax.y - ptMin.y) * 0.5);
    fw = ptMax.x - ptMin.x; fh = ptMax.y - ptMin.y;
  } else {
    eyePtLL.transformBy(eyeToWorldMatrix()); eyePtUR.transformBy(eyeToWorldMatrix());
    zoomExtents(eyePtLL, eyePtUR);
    fw = field_width_; fh = field_height_;
  }
  if (!diffVec.isZeroLength() || !OdEqual(field_width_, fw) || !OdEqual(field_height_, fh)) {
    position_ += diffVec; target_ += diffVec;
    field_width_ = fw; field_height_ = fh;

    SETBIT(view_flags_, kWorldToDeviceValid, false);
    onWorldToEyeChanged();
  }
}

void AcGsViewImpl::pan(double x, double y) {
  if (!OdZero(x)) {
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    ODGS_GETSAFEDIRECTION(dir);
    dir.rotateBy(x, x_vector_);
    eye_vector_ = dir;
    eye_length_ = eye_vector_.normalizeGetLength();
    up_vector_.rotateBy(x, x_vector_);
    target_ = position_ - dir;
  }

  if (!OdZero(y)) {
    SETBIT(view_flags_, kWorldToDeviceValid, false);
    ODGS_GETSAFEDIRECTION(dir);
    dir.rotateBy(y, up_vector_);
    eye_vector_ = dir;
    eye_length_ = eye_vector_.normalizeGetLength();
    x_vector_ = up_vector_.crossProduct(eye_vector_);
    target_ = position_ - dir;
  }

  if (!OdZero(x) || !OdZero(y))
    onWorldToEyeChanged();
}

bool AcGsViewImpl::pointInView(const AcGePoint3d& pt) {
  const OdGeTol &tol = OdGeContext::gTol;
  const double fw = fieldWidth(), fh = fieldHeight();
  OdGePoint3d eyePt(pt);
  eyePt.transformBy(viewingMatrix());
  // Check depth
  if (isFrontClipped()) {
    if (OdGreater(eyePt.z, frontClip(), tol.equalPoint()))
      return false;
  }

  if (isBackClipped()) {
    if (OdLess(eyePt.z, backClip(), tol.equalPoint()))
      return false;
  }

  if (!isPerspective()) {
    return OdGreaterOrEqual(eyePt.x, -fw * 0.5, tol.equalPoint()) &&
      OdGreaterOrEqual(eyePt.y, -fh * 0.5, tol.equalPoint()) &&
      OdLessOrEqual(eyePt.x, fw * 0.5, tol.equalPoint()) &&
      OdLessOrEqual(eyePt.y, fh * 0.5, tol.equalPoint());
  }
  else {
    const double fLen = focalLength();
    OdSi::Volume volume(target_ + eye_vector_ * fLen, -eye_vector_, up_vector_,
      (-eye_vector_).crossProduct(up_vector_),
      windowAspect(), OdSi::Volume::fovFromPlane(fh, fLen),
      true, false,
      true, 0.0, false, 0.0);
    OdGeExtents3d extChk(pt, pt);
    return volume.contains(extChk, false, tol);
  }
}

bool AcGsViewImpl::extentsInView(const AcGePoint3d& minPoint, const AcGePoint3d& maxPoint) {
  const OdGeTol &tol = OdGeContext::gTol;
  const double fw = fieldWidth(), fh = fieldHeight();
  OdGeExtents3d eyeExt(minPoint, maxPoint);
  eyeExt.transformBy(viewingMatrix());
  // Check depth
  if (isFrontClipped()) {
    if (OdGreater(eyeExt.minPoint().z, frontClip(), tol.equalPoint()))
      return false;
  }

  if (isBackClipped()) {
    if (OdLess(eyeExt.maxPoint().z, backClip(), tol.equalPoint()))
      return false;
  }

  if (!isPerspective()) {
    return OdGreaterOrEqual(eyeExt.maxPoint().x, -fw * 0.5, tol.equalPoint()) &&
      OdGreaterOrEqual(eyeExt.maxPoint().y, -fh * 0.5, tol.equalPoint()) &&
      OdLessOrEqual(eyeExt.minPoint().x, fw * 0.5, tol.equalPoint()) &&
      OdLessOrEqual(eyeExt.minPoint().y, fh * 0.5, tol.equalPoint());
  }
  else {
    const double fLen = focalLength();
    OdSi::Volume volume(target_ + eye_vector_ * fLen, -eye_vector_, up_vector_,
      (-eye_vector_).crossProduct(up_vector_),
      windowAspect(), OdSi::Volume::fovFromPlane(fh, fLen),
      true, false,
      true, 0.0, false, 0.0);
    OdGeExtents3d extChk(minPoint, maxPoint);
    return volume.intersects(extChk, false, tol);
  }
}

AcGsView * AcGsViewImpl::clone(bool bCloneViewParameters /*= true*/, bool bCloneGeometry /*= false*/) {
  return nullptr;
}

AcGsView * AcGsViewImpl::clone(bool bCloneViewParameters, ModelCloneOption cloneOption) {
  return nullptr;
}

Atil::Image * AcGsViewImpl::createSnapshot(const AcGsDCPoint& offset, const AcGsDCRect& imageDim, GS::ImageDataFormat format, GS::ImageOrientation orientation, bool bOnlyCaptureRenderedImage /*= false*/) {
  return nullptr;
}

AcGsView::EnableIRResult AcGsViewImpl::beginInteractiveRender(AcGsRenderProgressMonitor* pProgressMonitor) {
  return EnableIRResult::kSuccess;
}

void AcGsViewImpl::endInteractiveRender(void) {

}

bool AcGsViewImpl::isRendering(void) const {
  return rendering_;
}

void AcGsViewImpl::beginFastInteractivity(void) {
  fast_interactivity_ = true;

  //ContextProvider::GetInstance()->GetBatchedElements()->Clear();
}

bool AcGsViewImpl::endFastInteractivity(void) {
  fast_interactivity_ = false;
  return true;
}

bool AcGsViewImpl::isFastInteractivity(void) {
  return fast_interactivity_;
}

bool AcGsViewImpl::exceededBounds(void) {
  return false;
}

void AcGsViewImpl::enableStereo(bool bEnable) {

}

bool AcGsViewImpl::isStereoEnabled(void) const {
  return false;
}

void AcGsViewImpl::setStereoParameters(double magnitude, double parallax) {

}

void AcGsViewImpl::getStereoParameters(double & magnitude, double & parallax) const {

}

void AcGsViewImpl::getSnapShot(Atil::Image * pOutput, AcGsDCPoint const & offset) {

}

bool AcGsViewImpl::renderToImage(Atil::Image* pOutput, AcGiDrawable* pSettings, AcGsRenderProgressMonitor* pProgressMonitor, const AcGsDCRect & rectScreen, bool bReuseScene /*= false*/) {
  return false;
}

AcGsDevice * AcGsViewImpl::getDevice(void) const {
  return device_;
}

void AcGsViewImpl::setVisualStyle(const AcDbObjectId visualStyleId) {

}

void AcGsViewImpl::setVisualStyle(const AcGiVisualStyle &visualStyle) {

}

AcDbObjectId AcGsViewImpl::visualStyle(void) const {
  return AcDbObjectId::kNull;
}

bool AcGsViewImpl::visualStyle(AcGiVisualStyle &visualStyle) const {
  return false;
}

void AcGsViewImpl::setBackground(const AcDbObjectId backgroundId) {

}

AcDbObjectId AcGsViewImpl::background(void) const {
  return AcDbObjectId::kNull;
}

void AcGsViewImpl::enableDefaultLighting(bool bEnable, DefaultLightingType type /*= kTwoLights*/) {

}

void AcGsViewImpl::getNumPixelsInUnitSquare(const AcGePoint3d& givenWorldpt, AcGePoint2d& pixelArea, bool includePerspective /*= true*/) const {
  OdGePoint2d ll, ur;
  getViewport(ll, ur);

  const double field_width = fieldWidth();
  const double field_height = fieldHeight();

  pixelArea.x = fabs(double(dcScreenMax().x - dcScreenMin().x) / field_width*(ur.x - ll.x));
  pixelArea.y = fabs(double(dcScreenMax().y - dcScreenMin().y) / field_height*(ur.y - ll.y));

  if (includePerspective && isPerspective()) {
    OdGePoint3d eyePt(givenWorldpt);
    eyePt.transformBy(viewingMatrix());
    double cft = 1.0 - eyePt.z / focalLength();
    pixelArea.x = fabs(pixelArea.x / cft);
    pixelArea.y = fabs(pixelArea.y / cft);
  }
}

AcGsModel* AcGsViewImpl::getModel(AcGiDrawable* drawable) const {
  AcGsViewImpl* pThis = const_cast<AcGsViewImpl*>(this);
  for (auto& iter : pThis->drawable_holders_) {
    if (pThis->drawableAt(iter).get() == drawable)
      return iter.model;
  }

  return nullptr;
}

void AcGsViewImpl::getModelList(AcArray<AcGsModel *>& model_list) const {
  for (auto& iter : drawable_holders_)
    model_list.append(iter.model);
}

const AcGsClientViewInfo& AcGsViewImpl::getClientViewInfo() const {
  return client_view_info_;
}

AcDbObjectId AcGsClientViewInfo::viewportObjectAsObjectId() const {
  return AcDbObjectId(reinterpret_cast<OdDbStub*>(viewportObjectId));
}

//self
void AcGsViewImpl::setClientViewInfo(const AcGsClientViewInfo& client_view_info) {
  client_view_info_ = client_view_info;

  AcDbObjectId object_id = client_view_info_.viewportObjectAsObjectId();
  OdDbViewportTableRecordPtr view_record = object_id.safeOpenObject();

  OdGePoint3d target = view_record->target();
  double width = view_record->width();
  double height = view_record->height();
  OdGePoint2d center = view_record->centerPoint();
  OdGeVector3d direction = view_record->viewDirection();

  target.x += center.x;
  target.y += center.y;

  OdGePoint3d position = target + direction;
  
  setView(position, target, AcGeVector3d::kYAxis, width, height);

  OdGePoint2d lower_left = view_record->lowerLeftCorner();
  OdGePoint2d upper_right = view_record->upperRightCorner();
  setViewport(lower_left, upper_right);
}

#include "DbObjectId.h"
#include "DbLayerTable.h"
#include "DbLayerTableRecord.h"
#include "DbSymUtl.h"
void AcGsViewImpl::updateLayer(OdDbStub* layer_table) {
  OdDbLayerTablePtr layer_record = OdDbObjectId(layer_table).openObject();
  OdDbSymbolTableIteratorPtr symbol_iter = layer_record->newIterator();
  for (symbol_iter->start(); !symbol_iter->done(); symbol_iter->step()) {
    OdDbLayerTableRecordPtr layer_record = symbol_iter->getRecord();

    OdGiLayerTraitsData layer_trait;

    layer_trait.setByBlock(OdDbSymUtil::isLayerZeroName(layer_record->getName()));
    layer_trait.setFrozen(layer_record->isFrozen());
    layer_trait.setOff(layer_record->isOff());
    layer_trait.setPlottable(layer_record->isPlottable());
    layer_trait.setLocked(layer_record->isLocked());

    layer_trait.setTrueColor(layer_record->color().entityColor());
    layer_trait.m_plotStyleNameId = layer_record->plotStyleNameId();
    layer_trait.setLinetype(layer_record->linetypeObjectId());
    layer_trait.setLineweight(layer_record->lineWeight());
    layer_trait.setMaterial(layer_record->materialId());
    layer_trait.setTransparency(layer_record->transparency());

    AcDbObjectId viewport_id = client_view_info_.viewportObjectAsObjectId();
    if (!layer_record->worldDraw(nullptr)) {
      OdCmEntityColor col;
      col.setColor(layer_record->color(viewport_id).color());
      layer_trait.setTrueColor(col);
      layer_trait.m_plotStyleNameId = layer_record->plotStyleNameId(viewport_id);
      layer_trait.setLinetype(layer_record->linetypeObjectId(viewport_id));
      layer_trait.setLineweight(layer_record->lineWeight(viewport_id));
      layer_record->setTransparency(layer_record->transparency(viewport_id));
    }

    layer_map_.insert({layer_record->id(), layer_trait});
  }
}

OdGiLayerTraitsData* AcGsViewImpl::layerNode(OdDbStub* layer_id) {
  auto iter = layer_map_.find(layer_id);
  if (iter != layer_map_.end())
    return &iter->second;

  return nullptr;
}

void AcGsViewImpl::onWorldToEyeChanged() {
  SETBIT(view_flags_, kEyeToWorldValid, false);
  SETBIT(view_flags_, kWorldToEyeValid, false);
  //initCullingVolume();
  invalidate();
}

double AcGsViewImpl::windowAspect() const {
  double height = double(screen_max_.y - screen_min_.y) * (upper_right_.y - lower_left_.y);
  if (OdZero(height, .5))
    height = 0.5;

  double width = double(screen_max_.x - screen_min_.x) * (upper_right_.x - lower_left_.x);
  if (OdZero(width, .5))
    width = 0.5;

  return fabs(width / height);
}

const AcGeMatrix3d& AcGsViewImpl::eyeToWorldMatrix() const {
  if (!GETBIT(view_flags_, kEyeToWorldValid)) {
    eye_world_.setCoordSystem(target(), xVector(), upVector(), eyeVector());
    SETBIT(view_flags_, kEyeToWorldValid, true);
  }
  return eye_world_;
}

AcGeMatrix3d AcGsViewImpl::perspectiveMatrix() const {
  AcGeMatrix3d res;
  if (isPerspective()) {
    double fl = focalLength();
    if (!OdZero(fl))
      res[3][2] = -1.0 / fl;
    else
      res[3][2] = -1.0;
  }
  return res;
}

void AcGsViewImpl::screenRect(AcGsDCPoint &min, AcGsDCPoint &max) const {
  OdGePoint2d ll, ur;
  screenRect(ll, ur);
  min.x = OdTruncateToLong(ll.x);
  max.x = OdTruncateToLong(ur.x);
  min.y = OdTruncateToLong(ll.y);
  max.y = OdTruncateToLong(ur.y);
}

void AcGsViewImpl::screenRect(OdGePoint2d &ll, OdGePoint2d &ur) const {
  getViewport(ll, ur);
  AcGeMatrix3d xScreen3d = screenMatrix();
  OdGePoint3d orig;
  OdGeVector3d x, y, z;
  xScreen3d.getCoordSystem(orig, x, y, z);
  OdGeMatrix2d xScreen2d;
  xScreen2d.setCoordSystem((const OdGePoint2d&)orig, (const OdGeVector2d&)x, (const OdGeVector2d&)y);

  ll.transformBy(xScreen2d);
  ur.transformBy(xScreen2d);

  if ( ll.x > ur.x)
    std::swap(ll.x, ur.x);

  if (ll.y > ur.y)
    std::swap(ll.y, ur.y);
}

bool AcGsViewImpl::doPerspectivePt(OdGePoint3d& pointToTransform) const {
  if (isPerspective()) {
    if (pointToTransform.z < focalLength()) {
      pointToTransform.transformBy(perspectiveMatrix());
      return true;
    }
    else
      return false;
  }
  return false;
}

OdGiDrawablePtr AcGsViewImpl::drawableAt(DrawableHolder& holder) {
  if (holder.drawable.get())
    return holder.drawable;

  OdDbObjectId id(holder.drawable_id);
  return id.safeOpenObject();
}

double AcGsViewImpl::lensLength() const {
  return lens_length_;
}

double AcGsViewImpl::focalLength() const {
  double field_width = fieldWidth();
  double field_height = fieldHeight();
  return lensLength() / PERSP_CONST * sqrt(field_width*field_width + field_height*field_height);
}

OdGeMatrix3d AcGsViewImpl::eyeToScreenMatrix() const {
  return screenMatrix() * projectionMatrix();
}

#include "AcGiVectorizer.h"
void AcGsViewImpl::select(const AcGsDCPoint* dcpts, int num_points, OdGsSelectionReactor* reactor, AcGsView::SelectionMode mode) {
  AcGsDisplayContextImpl display_context;
  OdGeExtents2d extent;
  display_context.setGsView(this, extent);
 
  OdStaticRxObject<AcGiVectorizer> vectorizer;
  vectorizer.setGsView(this);

  AcGsSelectVisitor visitor(reactor, &display_context, &vectorizer);
  display_context.setRender(&visitor);

  AcGsSpatialQuery spatial_query;

  for (unsigned i = 0; i < drawable_holders_.size(); ++i) {
    DrawableHolder& holder = drawable_holders_[i];
    OdGiDrawablePtr drawable = drawableAt(holder);

    if (drawable->isPersistent()) {
      AcGsContainerNode* container_node = reinterpret_cast<AcGsContainerNode*>(drawable->gsNode());
      int view_id = static_cast<AcGsModelImpl*>(holder.model)->getViewId(this);
      display_context.setViewId(view_id);

      const OdSiSpatialIndex* spatial_index = container_node->spatialIndex(view_id);

      AcGeExtents3d extent;
      spatial_index->extents(extent);

      const OdSiShape& shape = spatial_query.build(*this, extent, num_points, dcpts);

      spatial_index->query(shape, visitor);
    }

  }
}