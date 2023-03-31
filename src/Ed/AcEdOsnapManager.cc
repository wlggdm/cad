#include "AcEdOsnapManager.h"
#include "DbEntity.h"
#include "OdRound.h"

#include "GiViewportGeometry.h"
#include "GiViewport.h"
#include "GiViewportDraw.h"
#include "GiPathNode.h"

#include "AcGsViewImpl.h"

namespace {
  const int kHitRadius = 15;

  const int kSnapSize  = 5;
}

AcEdOsnapManager::AcEdOsnapManager() {
}

AcEdOsnapManager::~AcEdOsnapManager() {
}

bool AcEdOsnapManager::Snap(AcGsView* view, AcGePoint3d& pick_point, const AcGePoint3d* last_point) {
  AcGePoint3d osnap_prev = osnap_point_;

  view_ = view;
  pick_point_  = pick_point;
  last_point_  = last_point;
  osnap_point_ = pick_point_;
  osnap_mode_  = static_cast<OdDb::OsnapMode>(0);

  AcGePoint2d pt = (view->worldToDeviceMatrix()*pick_point).convert2d();
  AcGsDCPoint pts[2];
  pts[0].x = OdRoundToLong(pt.x) - kHitRadius;
  pts[1].x = pts[0].x + kHitRadius * 2;
  pts[0].y = OdRoundToLong(pt.y) - kHitRadius;
  pts[1].y = pts[0].y + kHitRadius * 2;

  //Select entity
  if (!last_point) {
    //SETBIT(snap_mode, (1<<OdDb::kOsModePerp)|(1<<OdDb::kOsModeTan)), 0);
  }

  AcGsViewImpl* view_impl = static_cast<AcGsViewImpl*>(view);
  view_impl->select(pts, 2, this, AcGsView::kWindow);

  pick_point = osnap_point_;

  bool ret = true;
  if (osnap_point_ == osnap_prev)
    ret = false;
  else {
    if (osnap_prev.x < 1E100)
      invalidateViewport(osnap_prev);
    if (osnap_point_.x < 1E100)
      invalidateViewport(osnap_point_);
  }

  return ret;
}

bool AcEdOsnapManager::selected(const OdGiDrawableDesc& pDrawableDesc) {
  return false;
}

OdUInt32 AcEdOsnapManager::selected(const OdGiPathNode& path_node, const OdGiViewport& view_port) {
  OdDbEntityPtr entity = OdDbEntity::cast(OdDbObjectId(path_node.persistentDrawableId()).openObject());
  if (entity.isNull())
    return OdUInt32(kSkipDrawable);

  AcGeMatrix3d xModelToWorld = view_port.getEyeToWorldTransform() * view_port.getModelToEyeTransform();
  AcGeMatrix3d xWorldToModel = view_port.getEyeToModelTransform() * view_port.getWorldToEyeTransform();

  unsigned snap_mdoes = 0XFFFFFF;
  if (!last_point_)
    SETBIT(snap_mdoes, (1<<OdDb::kOsModePerp)|(1<<OdDb::kOsModeTan), 0);

  SnapPoints(entity, snap_mdoes, 1, xWorldToModel);

  return OdUInt32(kContinue);
}

void AcEdOsnapManager::SnapPoints(const AcDbEntity* entity, 
                                  unsigned snap_mode, 
                                  OdGsMarker gs_marker, 
                                  const AcGeMatrix3d& world_eye) {
  AcGePoint3d last_point;
  if (last_point_)
    last_point = *last_point_;

  OdGePoint3dArray snap_points;
  for (OdDb::OsnapMode mode = OdDb::kOsModeEnd; mode <= OdDb::kOsModeNear; mode = OdDb::OsnapMode(mode + 1)) {
    if ((mode & snap_mode) == 0)
      continue;

    if (entity->getOsnapPoints(mode, gs_marker, pick_point_, last_point, world_eye, snap_points) != OdResult::eOk)
      continue;

    for (size_t i = 0; i < snap_points.size(); ++i)
      CheckPoint(mode, snap_points[i]);

    snap_points.clear();
  }
}

void AcEdOsnapManager::CheckPoint(OdDb::OsnapMode mode, const AcGePoint3d& point) {
  double dist = (point - pick_point_).length();

  if (dist > kHitRadius)
    return;

  if (mode > osnap_mode_ || (mode == osnap_mode_ && dist < osnap_dist_)) {
    osnap_mode_  = mode;
    osnap_point_ = point;
    osnap_dist_  = dist;
  }
}

void AcEdOsnapManager::invalidateViewport(const AcGePoint3d& point) const {
  AcGeMatrix3d world_device = view_->worldToDeviceMatrix();

  if (osnap_mode_ > 0) {
    AcGePoint3d pt = world_device * point;
    AcGsDCRect rc;
    rc.m_min.x = OdRoundToLong(pt.x);
    rc.m_min.y = OdRoundToLong(pt.y);
    rc.m_max = rc.m_min;

    rc.m_min.x -= kSnapSize * 2;
    rc.m_min.y -= kSnapSize * 2;
    rc.m_max.x += kSnapSize * 2;
    rc.m_max.y += kSnapSize * 2;

    view_->invalidate(rc);
  }
}

void AcEdOsnapManager::ViewportDraw(OdGiViewportDraw* VpDraw) const {
  if (osnap_mode_ == 0)
    return;

  OdGePoint3d pts[4];
  OdGiViewportGeometry& geom = VpDraw->geometry();
  OdGiViewport& vp = VpDraw->viewport();
  AcGeMatrix3d world_eye = vp.getWorldToEyeTransform();
  vp.getNumPixelsInUnitSquare(vp.getCameraTarget(), (OdGePoint2d&)pts[0]);

  double pix = 1 / pts[0].x;
  double s = kSnapSize * pix;

  OdGiSubEntityTraits& traits = VpDraw->subEntityTraits();
  traits.setColor(2);
  traits.setFillType(kOdGiFillNever);

  pts[0] = world_eye * osnap_point_;
  vp.doPerspective(pts[0]);

  switch (osnap_mode_) {
    case OdDb::kOsModeEnd: {
      pts[1].set(pts[0].x - s, pts[0].y + s, 0.0);
      pts[2].set(pts[0].x - s, pts[0].y - s, 0.0);
      pts[3].set(pts[0].x + s, pts[0].y - s, 0.0);
      pts[0].set(pts[0].x + s, pts[0].y + s, 0.0);
      geom.polygonEye(4, pts);
      pts[1].set(pts[1].x - pix, pts[1].y + pix, 0.0);
      pts[2].set(pts[2].x - pix, pts[2].y - pix, 0.0);
      pts[3].set(pts[3].x + pix, pts[3].y - pix, 0.0);
      pts[0].set(pts[0].x + pix, pts[0].y + pix, 0.0);
      geom.polygonEye(4, pts);
      break;
    }
    case OdDb::kOsModeMid: {
      pts[1].set(pts[0].x - s * 1.2, pts[0].y - s * 0.6, 0.0);
      pts[2].set(pts[0].x, pts[0].y + s * 1.4, 0.0);
      pts[3].set(pts[0].x + s * 1.2, pts[0].y - s * 0.6, 0.0);
      geom.polygonEye(3, pts + 1);

      pts[1].set(pts[1].x - pix, pts[1].y - pix, 0.0);
      pts[2].set(pts[2].x, pts[2].y + pix, 0.0);
      pts[3].set(pts[3].x + pix, pts[3].y - pix, 0.0);
      geom.polygonEye(3, pts + 1);
      break;
    }
    case OdDb::kOsModeCen: {
      OdGiModelTransformSaver mt(geom, vp.getEyeToWorldTransform());
      geom.circle(pts[0], s * 1.4, OdGeVector3d::kZAxis);
      break;
    }
    case OdDb::kOsModeQuad: {
      pts[1].set(pts[0].x - s, pts[0].y, 0.0);
      pts[2].set(pts[0].x, pts[0].y + s, 0.0);
      pts[3].set(pts[0].x + s, pts[0].y, 0.0);
      pts[0].set(pts[0].x, pts[0].y - s, 0.0);
      geom.polygonEye(4, pts);
      pts[1].set(pts[1].x - pix, pts[1].y, 0.0);
      pts[2].set(pts[2].x, pts[2].y + pix, 0.0);
      pts[3].set(pts[3].x + pix, pts[3].y, 0.0);
      pts[0].set(pts[0].x, pts[0].y - pix, 0.0);
      geom.polygonEye(4, pts);
      break;
    }
    case OdDb::kOsModePerp: {
      pts[1].set(pts[0].x - s, pts[0].y + s + pix, 0.0);
      pts[2].set(pts[0].x - s, pts[0].y - s, 0.0);
      pts[3].set(pts[0].x + s + pix, pts[0].y - s, 0.0);
      geom.polylineEye(3, pts + 1);
      pts[1].set(pts[1].x - pix, pts[1].y, 0.0);
      pts[2].set(pts[2].x - pix, pts[2].y - pix, 0.0);
      pts[3].set(pts[3].x, pts[3].y - pix, 0.0);
      geom.polylineEye(3, pts + 1);

      pts[1].set(pts[0].x - s, pts[0].y, 0.0);
      pts[2].set(pts[0].x, pts[0].y, 0.0);
      pts[3].set(pts[0].x, pts[0].y - s, 0.0);
      geom.polylineEye(3, pts + 1);
      pts[1].set(pts[1].x - pix, pts[1].y + pix, 0.0);
      pts[2].set(pts[2].x + pix, pts[2].y + pix, 0.0);
      pts[3].set(pts[3].x + pix, pts[3].y, 0.0);
      geom.polylineEye(3, pts + 1);
      break;
    }
    case OdDb::kOsModeTan: {
      {
        OdGiModelTransformSaver mt(geom, vp.getEyeToWorldTransform());
        geom.circle(pts[0], s, OdGeVector3d::kZAxis);
      }
      pts[1].set(pts[0].x - s, pts[0].y + s, 0.0);
      pts[2].set(pts[0].x + s, pts[0].y + s, 0.0);
      geom.polylineEye(2, pts + 1);
      pts[1].set(pts[1].x, pts[1].y + pix, 0.0);
      pts[2].set(pts[2].x, pts[2].y + pix, 0.0);
      geom.polylineEye(2, pts + 1);
      break;
    }
    case OdDb::kOsModeNear: {
      pts[1].set(pts[0].x - s, pts[0].y + s, 0.0);
      pts[2].set(pts[0].x + s, pts[0].y - s, 0.0);
      pts[3].set(pts[0].x - s, pts[0].y - s, 0.0);
      pts[0].set(pts[0].x + s, pts[0].y + s, 0.0);
      geom.polygonEye(4, pts);
      pts[1].set(pts[1].x - pix, pts[1].y + pix, 0.0);
      pts[2].set(pts[2].x + pix, pts[2].y - pix, 0.0);
      pts[3].set(pts[3].x - pix, pts[3].y - pix, 0.0);
      pts[0].set(pts[0].x + pix, pts[0].y + pix, 0.0);
      geom.polygonEye(4, pts);
      break;
    }
    default: {
      pts[1].set(pts[0].x - s, pts[0].y + s, 0.0);
      pts[2].set(pts[0].x + s, pts[0].y - s, 0.0);
      geom.polygonEye(2, pts + 1);
      pts[1].set(pts[0].x - s, pts[0].y - s, 0.0);
      pts[2].set(pts[0].x + s, pts[0].y + s, 0.0);
      geom.polygonEye(2, pts + 1);
      pts[1].set(pts[0].x - s - pix, pts[0].y + s + pix, 0.0);
      pts[2].set(pts[0].x - s - pix, pts[0].y - s - pix, 0.0);
      pts[3].set(pts[0].x + s + pix, pts[0].y - s - pix, 0.0);
      pts[0].set(pts[0].x + s + pix, pts[0].y + s + pix, 0.0);
      geom.polygonEye(4, pts);
      break;
    }
  }
  /*
  OdGsMarker gsMarker = 0;
  if (m_centers.size()) {
    pVpDraw->subEntityTraits().setColor(7);
    for (OdUInt32 i = 0; i < m_centers.size(); i++, gsMarker++) {
      traits.setSelectionMarker(gsMarker);

      const OdGePoint3d& cntr = xWorldToEye * m_centers[i].m_point;
      pts[0].set(cntr.x, cntr.y + s, 0.0);
      pts[1].set(cntr.x, cntr.y - s, 0.0);
      geom.polygonEye(2, pts);
      pts[0].set(cntr.x + s, cntr.y, 0.0);
      pts[1].set(cntr.x - s, cntr.y, 0.0);
      geom.polygonEye(2, pts);
    }
  }
  */
}