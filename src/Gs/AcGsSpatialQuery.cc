#include "AcGsSpatialQuery.h"

#define startDumpBox(db)
#define finishBoxDumping()
#define dumpBox(box, nColor)

namespace {
  void intersectBBox(OdSi::BBox& spqbox, const OdGeExtents3d& extents) {
    if (spqbox.isBox()) {
      OdGeExtents3d extBox(spqbox.minPoint(), spqbox.maxPoint());
      if (extents.contains(extBox))
        return;//no need to intersect
      OdGeExtents3d intBox;
      if ((extBox.intersectWith(extents, &intBox) == OdGeExtents3d::kIntersectOk) ||
          intBox.isValidExtents())
        spqbox = OdSi::BBox(intBox.minPoint(), intBox.maxPoint());
    } else {
      OdGePoint3d minPt, maxPt;
      spqbox.getMinMaxPoints(minPt, maxPt);
      OdGeExtents3d extBox(minPt, maxPt);
      if (extents.contains(extBox))
        return;//no need to intersect

      OdGePoint3d origin;
      OdGeVector3d xAxis, yAxis, zAxis;
      spqbox.get(origin, xAxis, yAxis, zAxis);
      double xSqrLen = xAxis.lengthSqrd();
      if (!OdZero(xSqrLen))
        xSqrLen = 1. / xSqrLen;
      double ySqrLen = yAxis.lengthSqrd();
      if (!OdZero(ySqrLen))
        ySqrLen = 1. / ySqrLen;
      double zSqrLen = zAxis.lengthSqrd();
      if (!OdZero(zSqrLen))
        zSqrLen = 1. / zSqrLen;
      OdGePoint3d aPt[8];
      const int n = (OdSi::BBox::explodeExtent(aPt, extents) == OdSi::BBox::k2dExtents ? 4 : 8);
      OdGeExtents3d projExtents;
      for (int i = 0; i < n; ++i) {
        OdGeVector3d v = aPt[i] - origin;
        OdGePoint3d ptProj(v.dotProduct(xAxis)*xSqrLen, v.dotProduct(yAxis)*ySqrLen,
                           v.dotProduct(zAxis)*zSqrLen);
        projExtents.addPoint(ptProj);
      }
      OdGeExtents3d intBox;
      extBox.set(OdGePoint3d::kOrigin, OdGePoint3d(1, 1, 1));
      if (projExtents.contains(extBox))
        return;//no need to intersect
      if ((extBox.intersectWith(projExtents, &intBox) == OdGeExtents3d::kIntersectOk) ||
          intBox.isValidExtents()) {
        OdGeVector3d diff = intBox.maxPoint() - intBox.minPoint();
        spqbox = OdSi::BBox(
          //new origin
          origin + intBox.minPoint().x*xAxis +
          intBox.minPoint().y*yAxis + intBox.minPoint().z*zAxis,
          //new axes
          diff.x*xAxis, diff.y*yAxis, diff.z*zAxis);
      }
    }
  }

  bool isClippedOnScreen(const AcGsViewImpl& view,
                         OdGePoint2dArray& aVpScreenCornersEye,
                         int& nPoints, const OdGePoint2d*& pts) {
    AcGsDCRect viewRect;
    view.getViewport(viewRect);
    viewRect.normalize();
    AcGsDCRect outputRect = viewRect;// = view.baseDevice()->outputRect();
    outputRect.normalize();
    AcGsDCRect viewRect2 = viewRect;
    viewRect2 &= outputRect;
    if (viewRect2 == viewRect)
      return false;
    OdGeMatrix3d s2e = view.eyeToScreenMatrix().inverse();
    if (!nPoints || !pts) {
      aVpScreenCornersEye.resize(2);
      OdGePoint3d pt(viewRect2.m_min.x, viewRect2.m_max.y, 0);
      pt.transformBy(s2e);
      aVpScreenCornersEye[0] = (const OdGePoint2d&)pt;
      pt.set(viewRect2.m_max.x, viewRect2.m_min.y, 0);
      pt.transformBy(s2e);
      aVpScreenCornersEye[1] = (const OdGePoint2d&)pt;
    } else if (nPoints > 1) {
      //calc vp extents
      OdGePoint3d pt(viewRect2.m_min.x, viewRect2.m_max.y, 0);
      pt.transformBy(s2e);
      OdGeExtents2d extVp((const OdGePoint2d&)pt, (const OdGePoint2d&)pt);
      pt.set(viewRect2.m_min.x, viewRect2.m_min.y, 0);
      pt.transformBy(s2e);
      extVp.addPoint((const OdGePoint2d&)pt);
      pt.set(viewRect2.m_max.x, viewRect2.m_min.y, 0);
      pt.transformBy(s2e);
      extVp.addPoint((const OdGePoint2d&)pt);
      pt.set(viewRect2.m_max.x, viewRect2.m_max.y, 0);
      pt.transformBy(s2e);
      extVp.addPoint((const OdGePoint2d&)pt);

      //calc pts extents
      OdGeExtents2d extPts(*pts, *pts);
      for (int i = 1; i < nPoints; ++i)
        extPts.addPoint(pts[i]);

      //calc intersection
      OdGePoint2d minPt = extVp.minPoint(), maxPt = extVp.maxPoint();
      if (minPt.x < extPts.minPoint().x)
        minPt.x = extPts.minPoint().x;
      if (maxPt.x > extPts.maxPoint().x)
        maxPt.x = extPts.maxPoint().x;

      if (minPt.y < extPts.minPoint().y)
        minPt.y = extPts.minPoint().y;
      if (maxPt.y > extPts.maxPoint().y)
        maxPt.y = extPts.maxPoint().y;

      if (minPt.x > maxPt.x || minPt.y > maxPt.y)
        return false;
      aVpScreenCornersEye.resize(2);
      aVpScreenCornersEye[0] = minPt;
      aVpScreenCornersEye[1] = maxPt;
    } else
      return false;
    pts = aVpScreenCornersEye.asArrayPtr();
    nPoints = 2;
    return true;
  }
}

AcGsSpatialQuery::AcGsSpatialQuery() {

}

const OdSiShape& AcGsSpatialQuery::build(const AcGsViewImpl& view,
                                         const AcGeExtents3d& worldExtents,
                                         int   nPoints,
                                         const AcGePoint2d* pts,
                                         double x,
                                         double y) {
  if (worldExtents.isValidExtents()) {
    OdGeInterval viewDepth;
    if (view.isFrontClipped())
      viewDepth.setUpper(view.frontClip());
    if (view.isBackClipped())
      viewDepth.setLower(view.backClip());
    if (view.isPerspective()) {
      if (viewDepth.isBoundedAbove() && view.focalLength() < viewDepth.upperBound())
        viewDepth.setUpper(view.focalLength());
    }
    if (!viewDepth.isBounded() || viewDepth.upperBound() > viewDepth.lowerBound()) {
      OdGeExtents3d we = worldExtents;
      we.transformBy(view.viewingMatrix());
      OdGeInterval sceneDepth(we.minPoint().z, we.maxPoint().z);

      OdGeInterval depth;
      OdGePoint2dArray aVpScreenCornersEye;
      if (viewDepth.intersectWith(sceneDepth, depth)) {
        if (!view.isPerspective()) {
          OdGeMatrix3d xScreenToEye;
          if ((!isClippedOnScreen(view, aVpScreenCornersEye, nPoints, pts))
              && (!nPoints || !pts)) {
            double factor = (x == 1) ? 3 : 1;

            OdGePoint2d halfField(view.fieldWidth()*(.5 * factor), view.fieldHeight()*(.5 * factor));
            //OdGePoint2d halfField(view.fieldWidth()*.5, view.fieldHeight()*.5);

            OdGePoint3d origin = view.target()
              - view.xVector()   * halfField.x
              - view.upVector()  * halfField.y
              + view.eyeVector() * depth.lowerBound();

            box_shape_ = OdSi::BBox(
              origin,
              view.xVector()   * view.fieldWidth() * factor,
              view.upVector()  * view.fieldHeight() * factor,
              view.eyeVector() * depth.length()
            );

            if (view.xVector().isEqualTo(OdGeVector3d::kXAxis) &&
                view.upVector().isEqualTo(OdGeVector3d::kYAxis) &&
                view.eyeVector().isEqualTo(OdGeVector3d::kZAxis)) {
              OdGePoint3d p1, p2;
              box_shape_.getMinMaxPoints(p1, p2);

              box_shape_ = OdSi::BBox(p1, p2);
            }
          } else {
            OdGePoint3d pt(pts->x, pts->y, depth.lowerBound());

            OdGeExtents3d extents(pt, pt);
            for (pts++; --nPoints != 0; pts++) {
              pt.set(pts->x, pts->y, depth.lowerBound());
              extents.addPoint(pt);
            }

            // Extend in each direction on one pixel to avoid accuracy artifacts
            OdGePoint3d pt2;
            view.getNumPixelsInUnitSquare(view.target(), (OdGePoint2d&)pt2, false);
            double pixelSize = 1. / odmax(pt2.x, pt2.y);
            pt2 = extents.minPoint();
            pt = extents.maxPoint();
            pt.x += pixelSize;
            pt.y += pixelSize;
            pt.z = depth.upperBound();
            extents.addPoint(pt);
            pt2.x -= pixelSize;
            pt2.y -= pixelSize;
            extents.addPoint(pt2);

            box_shape_.set(extents.minPoint(), extents.maxPoint());

            box_shape_.transform(view.eyeToWorldMatrix());
          }
#ifdef ODA_GS_DIAGNOSTICS
          dumpBox(m_spqbox, 3);
#endif
        } else {
          double fLen = view.focalLength();
          double vFov;
          double hAspect;
          OdGeVector3d eyeVector;
          const OdGePoint3d target = view.target();
          const OdGeVector3d upVector = view.upVector();
          double fLenDept;
          OdGePoint3d brPoint = target + view.eyeVector() * fLen;
          if (!nPoints || !pts) {
            vFov = OdSi::Volume::fovFromPlane(view.fieldHeight(), fLen);
            hAspect = view.windowAspect();
            eyeVector = -view.eyeVector();
            fLenDept = 0.0;
          } else {
            OdGeExtents2d eyeExt(*pts, *pts);
            nPoints--; pts++;
            while (nPoints) {
              eyeExt.addPoint(*pts);
              nPoints--;
              pts++;
            }
            double fw = eyeExt.maxPoint().x - eyeExt.minPoint().x;
            double fh = eyeExt.maxPoint().y - eyeExt.minPoint().y;
            // #10094 : crash if height is null
            if (OdZero(fw) || OdZero(fh)) {
              OdGePoint3d pt2;
              view.getNumPixelsInUnitSquare(target, (OdGePoint2d&)pt2, false);
              double pixelSize = 1. / odmax(pt2.x, pt2.y);
              if (OdZero(fw)) fw = pixelSize;
              if (OdZero(fh)) fh = pixelSize;
            }
            //
            OdGePoint2d ctr = eyeExt.minPoint() + OdGeVector2d(fw, fh) * 0.5;
            OdGeVector3d xMv = (-view.eyeVector()).crossProduct(upVector).normal();
            OdGePoint3d ctrRecomp = target + xMv * ctr.x + upVector * ctr.y;
            eyeVector = ctrRecomp - (target + view.eyeVector() * fLen);
            double fLenItl = eyeVector.normalizeGetLength();
            vFov = OdSi::Volume::fovFromPlane(fh, fLenItl); // fLenItl or fLen here?
            { // fix depth length coef
              if (ctr.x < 0.)
                ctrRecomp -= xMv * (fw * 0.5);
              else
                ctrRecomp += xMv * (fw * 0.5);
              if (ctr.y < 0.)
                ctrRecomp -= upVector * (fh * 0.5);
              else
                ctrRecomp += upVector * (fh * 0.5);
              fLenDept = (ctrRecomp - brPoint).length() - fLen;
            }
            hAspect = fw / fh;
          }

          volume_shape_ = OdSi::Volume(brPoint, eyeVector, upVector,
            (-view.eyeVector()).crossProduct(upVector),
                                       hAspect, vFov,
                                       true, false,
                                       depth.isBoundedAbove(), fLen - depth.upperBound() - fLenDept,
                                       depth.isBoundedBelow(), fLen - depth.lowerBound() + fLenDept);

          if (volume_shape_.contains(worldExtents, false, 1e-8)) {
            box_shape_ = OdSi::BBox(worldExtents.minPoint(), worldExtents.maxPoint());
            dumpBox(m_spqbox, 3);
            return box_shape_;
          } else if (!volume_shape_.intersects(worldExtents)) {
            return OdSiShape::kNoSpace;
          }
          return volume_shape_;
        }
        if (box_shape_.contains(worldExtents, false, 1e-8))
          box_shape_ = OdSi::BBox(worldExtents.minPoint(), worldExtents.maxPoint());
        else if (!box_shape_.intersects(worldExtents))
          return OdSiShape::kNoSpace;
        else
          intersectBBox(box_shape_, worldExtents);
        dumpBox(box_shape_, 3);
        return box_shape_;
      }
    }
    return OdSiShape::kNoSpace;
  }
  return OdSiShape::kOverallSpace;
}

const OdSiShape& AcGsSpatialQuery::build(const AcGsViewImpl& view,
                                         const AcGeExtents3d& worldExtents,
                                         int   nPoints /*= 0*/,
                                         const AcGsDCPoint* pts /*= 0*/) {
  OdGePoint2dArray points;
  if (pts != NULL && nPoints != 0) {
    OdGeMatrix3d xDcToEye = view.eyeToScreenMatrix();
    if (view.isPerspective())
      xDcToEye(2, 3) = 0.;
    xDcToEye.invert();

    points.reserve(nPoints);
    while (nPoints--) {
      OdGePoint3d pt3d(pts->x, pts->y, 0.);
      pt3d.transformBy(xDcToEye);
      points.push_back((const OdGePoint2d&)pt3d);
      ++pts;
    }
  }
  return build(view, worldExtents, points.size(), points.size() ? points.asArrayPtr() : NULL);
}
