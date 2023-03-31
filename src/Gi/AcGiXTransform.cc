#include "AcGiXTransform.h"

AcGiXform::AcGiXform() {

}

void AcGiXform::setTransform(const OdGeMatrix3d& form) {
  xform = form;
}

void AcGiXform::circleProc(const OdGePoint3d& center,
  double radius,
  const OdGeVector3d& normal,
  const OdGeVector3d* pExtrusion /*= 0*/) {
  if (xform.IsPerspective()) {
    OdGeCircArc3d circArc(center, normal, radius);
    sample_points.clear();
    circArc.appendSamplePoints(0., Oda2PI, 0., sample_points);
    sample_points.last() = sample_points.first();
    polylineProc(sample_points.size(), sample_points.getPtr(), &normal, pExtrusion);
    return;
  }

  if (xform.IsIdentity())
    m_pDestGeom->circleProc(center, radius, normal, pExtrusion);
  else if (OdZero(radius, OdGeContext::gZeroTol.equalPoint())) {
    OdGePoint3d pt = xform * center;
    m_pDestGeom->polylineProc(1, &pt, 0, xformExtrusion(pExtrusion));
  }
  else {
    OdGeCircArc3d& arc = CircArc3d();
    arc.set(center, normal, radius);
    EllipArc3d().set(arc);
    EllipArc3d().transformBy(xform);
    sample_points.resize(2);
    sample_points[0] = EllipArc3d().evalPoint(EllipArc3d().startAng());
    sample_points[1] = sample_points[0];
    m_pDestGeom->ellipArcProc(EllipArc3d(), sample_points.getPtr(), kOdGiArcChord, xformExtrusion(pExtrusion));
  }
}

void AcGiXform::circleProc(const OdGePoint3d& start,
  const OdGePoint3d& point,
  const OdGePoint3d& end,
  const OdGeVector3d* pExtrusion /*= 0*/) {
  if (xform.IsPerspective()) {
    OdGeCircArc3d circArc(start, point, end);
    sample_points.clear();
    circArc.appendSamplePoints(0., Oda2PI, 0., sample_points);
    sample_points.last() = sample_points.first();
    OdGeVector3d normal = circArc.normal();
    polylineProc(sample_points.size(), sample_points.getPtr(), &normal, pExtrusion);
    return;
  }

  if (xform.IsIdentity())
    m_pDestGeom->circleProc(start, point, end, pExtrusion);
  else {
    OdGeCircArc3d& arc = CircArc3d();
    OdGe::ErrorCondition err;
    arc.set(start, point, end, err);
    if (err == OdGe::kOk) {
      arc.setAngles(0., Oda2PI);
      EllipArc3d().set(arc);
      EllipArc3d().transformBy(xform);
      m_pDestGeom->ellipArcProc(EllipArc3d(), 0, kOdGiArcChord, xformExtrusion(pExtrusion));
    }
    else if (err == OdGe::kLinearlyDependentArg1Arg2Arg3){
      sample_points.resize(3);
      sample_points[0] = xform * start;
      sample_points[1] = xform * point;
      sample_points[2] = xform * end;
      m_pDestGeom->polylineProc(sample_points.size(), sample_points.getPtr(), 0, xformExtrusion(pExtrusion));
    }
    else {
      sample_points.resize(1);
      sample_points[0] = xform * start;
      m_pDestGeom->polylineProc(sample_points.size(), sample_points.getPtr(), 0, xformExtrusion(pExtrusion));
    }
  }
}

void AcGiXform::circularArcProc(const OdGePoint3d& center,
  double radius,
  const OdGeVector3d& normal,
  const OdGeVector3d& startVector,
  double sweepAngle,
  OdGiArcType arcType /*= kOdGiArcSimple*/,
  const OdGeVector3d* pExtrusion /*= 0*/) {
  if (xform.IsPerspective()) {
    OdGeCircArc3d circArc(center, normal, startVector, radius, 0., sweepAngle);
    sample_points.clear();
    circArc.appendSamplePoints(0., sweepAngle, 0., sample_points);
    polylineProc(sample_points.size(), sample_points.getPtr(), &normal, pExtrusion);
    return;
  }

  if (xform.IsIdentity())
    m_pDestGeom->circularArcProc(center, radius, normal, startVector, sweepAngle, arcType, pExtrusion);
  else if (OdZero(radius, OdGeContext::gZeroTol.equalPoint())) {
    OdGePoint3d pt = xform * center;
    m_pDestGeom->polylineProc(1, &pt, 0, xformExtrusion(pExtrusion));
  }
  else {
    CircArc3d().set(center, normal, startVector, radius, 0.0, sweepAngle);
    EllipArc3d().set(CircArc3d());
    EllipArc3d().transformBy(xform);
    m_pDestGeom->ellipArcProc(EllipArc3d(), 0, arcType, xformExtrusion(pExtrusion));
  }
}

void AcGiXform::circularArcProc(const OdGePoint3d& start,
  const OdGePoint3d& point,
  const OdGePoint3d& end,
  OdGiArcType arcType,
  const OdGeVector3d* pExtrusion/* = 0*/) {
  if (xform.IsPerspective()) {
    OdGeCircArc3d circArc(start, point, end);
    sample_points.clear();
    circArc.appendSamplePoints(circArc.startAng(), circArc.endAng(), 0., sample_points);
    OdGeVector3d normal = circArc.normal();
    polylineProc(sample_points.size(), sample_points.getPtr(), &normal, pExtrusion);
    return;
  }

  if (xform.IsIdentity())
    m_pDestGeom->circularArcProc(start, point, end, arcType, pExtrusion);
  else {
    OdGeError err;
    CircArc3d().set(start, point, end, err);
    if (err == OdGe::kOk) {
      if (OdZero(CircArc3d().radius(), OdGeContext::gZeroTol.equalPoint())) {
        sample_points.resize(1);
        sample_points[0] = xform * CircArc3d().center();
        m_pDestGeom->polylineProc(sample_points.size(), sample_points.getPtr(), 0, xformExtrusion(pExtrusion));
      }
      else {
        EllipArc3d().set(CircArc3d());
        EllipArc3d().transformBy(xform);
        sample_points.resize(2);
        sample_points[0] = xform * start;
        sample_points[1] = xform * end;
        m_pDestGeom->ellipArcProc(EllipArc3d(), sample_points.getPtr(), arcType, xformExtrusion(pExtrusion));
      }
    }
    else {
      sample_points.resize(3);
      sample_points[0] = xform * start;
      sample_points[1] = xform * point;
      sample_points[3] = xform * end;
      if (arcType == kOdGiArcSimple)
        m_pDestGeom->polylineProc(sample_points.size(), sample_points.getPtr(), 0, xformExtrusion(pExtrusion));
      else
        m_pDestGeom->polygonProc(sample_points.size(), sample_points.getPtr(), 0, xformExtrusion(pExtrusion));
    }
  }
}

void AcGiXform::ellipArcProc(
  const OdGeEllipArc3d& ellipArc,
  const OdGePoint3d* endPointsOverrides /*= 0*/,
  OdGiArcType arcType /*= kOdGiArcSimple*/,
  const OdGeVector3d* pExtrusion /*= 0*/) {
  if (xform.IsPerspective()) {
    sample_points.clear();
    ellipArc.appendSamplePoints(ellipArc.startAng(), ellipArc.endAng(), 0., sample_points);
    OdGeVector3d normal = ellipArc.normal();
    polylineProc(sample_points.size(), sample_points.getPtr(), &normal, pExtrusion);
    return;
  }

  if (xform.IsIdentity())
    m_pDestGeom->ellipArcProc(ellipArc, endPointsOverrides, arcType, pExtrusion);
  else {
    EllipArc3d() = ellipArc;
    EllipArc3d().transformBy(xform);
    if (endPointsOverrides) {
      sample_points.resize(2);
      sample_points[0] = xform * endPointsOverrides[0];
      sample_points[1] = xform * endPointsOverrides[1];
      m_pDestGeom->ellipArcProc(EllipArc3d(), sample_points.getPtr(), arcType, xformExtrusion(pExtrusion));
    }
    else
      m_pDestGeom->ellipArcProc(EllipArc3d(), endPointsOverrides, arcType, xformExtrusion(pExtrusion));
  }
}

void AcGiXform::polylineProc(OdInt32 nbPoints,
  const OdGePoint3d* pVertexList,
  const OdGeVector3d* pNormal /*= 0*/,
  const OdGeVector3d* pExtrusion /*= 0*/,
  OdGsMarker lBaseSubEntMarker /*= -1*/) {
  m_pDestGeom->polylineProc(nbPoints,
    xformPoints(nbPoints, pVertexList),
    xformNormal(pNormal),
    xformExtrusion(pExtrusion), 
    lBaseSubEntMarker);
}

void AcGiXform::polygonProc(OdInt32 nbPoints,
  const OdGePoint3d* pVertexList,
  const OdGeVector3d* pNormal,
  const OdGeVector3d* pExtrusion) {
  m_pDestGeom->polygonProc(nbPoints,
    xformPoints(nbPoints, pVertexList),
    xformNormal(pNormal), 
    xformExtrusion(pExtrusion));
}

void AcGiXform::meshProc(OdInt32 rows,
  OdInt32 columns,
  const OdGePoint3d* pVertexList,
  const OdGiEdgeData* pEdgeData,
  const OdGiFaceData* pFaceData,
  const OdGiVertexData* pVertexData) {
  xformVertexNormals(rows * columns, pVertexData);
  xformFaceNormals((rows - 1) * (columns - 1), pFaceData);
  m_pDestGeom->meshProc(rows, columns, xformPoints(rows * columns, pVertexList),
    pEdgeData, pFaceData, pVertexData);
}

void AcGiXform::shellProc(OdInt32 nbVertex,
  const OdGePoint3d* pVertexList,
  OdInt32 faceListSize,
  const OdInt32* pFaceList,
  const OdGiEdgeData* pEdgeData,
  const OdGiFaceData* pFaceData,
  const OdGiVertexData* pVertexData) {
  xformVertexNormals(nbVertex, pVertexData);
  if (pFaceData && pFaceData->normals()) {
    OdUInt32 nFaces = 0;
    for (int i = 0; i < faceListSize; ){
      if (pFaceList[i] > 0)
        ++nFaces;

      i += Od_abs((int)pFaceList[i]);
      ++i;
    }
    xformFaceNormals(nFaces, pFaceData);
  }

  if (0/*m_bXformNegativeDet*/){
    reverseFaceList(faceListSize, pFaceList);
    reverseEdgeData(faceListSize, pFaceList, pEdgeData);

    m_pDestGeom->shellProc(nbVertex, xformPoints(nbVertex, pVertexList),
      faceListSize, pFaceList, pEdgeData, pFaceData, pVertexData);
  }
  else{
    m_pDestGeom->shellProc(nbVertex, xformPoints(nbVertex, pVertexList),
      faceListSize, pFaceList, pEdgeData, pFaceData, pVertexData);
  }
}

void AcGiXform::ttfPolyDrawProc(OdInt32 nbVertex,
  const OdGePoint3d* pVertexList,
  OdInt32 faceListSize,
  const OdInt32* pFaceList,
  const OdUInt8* pBezierTypes,
  const OdGiFaceData* pFaceData /*= 0*/) {

}

void AcGiXform::textProc(const OdGePoint3d& position,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  const OdChar* msg,
  OdInt32 length,
  bool raw,
  const OdGiTextStyle* pTextStyle,
  const OdGeVector3d* pExtrusion /*= 0*/) {
  m_pDestGeom->textProc(xform * position, xform * u, xform * v, msg, length, raw, pTextStyle, xformExtrusion(pExtrusion));
}

void AcGiXform::textProc2(const OdGePoint3d& position,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  const OdChar* msg,
  OdInt32 length,
  bool raw, const
  OdGiTextStyle* pTextStyle,
  const OdGeVector3d* pExtrusion/* = 0*/,
  const OdGeExtents3d* extentsBox /*= 0*/) {
  //m_pDestGeom->textProc(xform * position, xform * u, xform * v, msg, length, raw, pTextStyle, xformExtrusion(pExtrusion), extentsBox);
}

int AcGiXform::ttfCharProcFlags() const {
  return 0;
}

bool AcGiXform::ttfCharProc(OdChar character,
  const OdGePoint3d& position,
  const OdGeBoundBlock3d* pBoundBlock) {
  return true;
}

void AcGiXform::shapeProc(const OdGePoint3d& position,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  int shapeNo,
  const OdGiTextStyle* pStyle,
  const OdGeVector3d* pExtrusion /*= 0*/) {

}

void AcGiXform::xlineProc(const OdGePoint3d&, const OdGePoint3d&) {

}

void AcGiXform::rayProc(const OdGePoint3d&, const OdGePoint3d&) {

}

void AcGiXform::xlineProc2(const OdGePoint3d& basePoint, const OdGeVector3d& direction) {

}

void AcGiXform::rayProc2(const OdGePoint3d& basePoint, const OdGeVector3d& direction) {

}

void AcGiXform::setExtentsProc(const OdGePoint3d *pPoints, bool bTransform /*= true*/) {

}

void AcGiXform::conveyorBoundaryInfoProc(const OdGeBoundBlock3d &boundBlock, OdUInt32 &outputFlags) {

}

void AcGiXform::nurbsProc(const OdGeNurbCurve3d& nurbs) {
  NurbCurve3d() = nurbs;
  NurbCurve3d().purgeFitData();
  NurbCurve3d().transformBy(xform);

  m_pDestGeom->nurbsProc(NurbCurve3d());
}

void AcGiXform::rasterImageProc(
  const OdGePoint3d& origin,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  const OdGiRasterImage* pImg, // image object
  const OdGePoint2d* uvBoundary, // may not be null
  OdUInt32 numBoundPts,
  bool transparency /*= false*/,
  double brightness /*= 50.0*/,
  double contrast /*= 50.0*/,
  double fade /*= 0.0*/) {

}

void AcGiXform::metafileProc(
  const OdGePoint3d& origin,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  const OdGiMetafile* pMetafile,
  bool bDcAligned /*= true*/,
  bool bAllowClipping /*= false*/) {

}

void AcGiXform::plineProc(const OdGiPolyline& lwBuf,
  const OdGeMatrix3d* pXform /*= 0*/,
  OdUInt32 fromIndex /*= 0*/,
  OdUInt32 numSegs /*= 0*/) {
  OdGeMatrix3d form = xform;
  if (pXform)
    form *= (*pXform);
  m_pDestGeom->plineProc(lwBuf, &form, fromIndex, numSegs);
}

void AcGiXform::polypointProc(OdInt32 numPoints,
  const OdGePoint3d* vertexList,
  const OdCmEntityColor* pColors,
  const OdCmTransparency* pTransparency /*= 0*/,
  const OdGeVector3d* pNormals /*= 0*/,
  const OdGeVector3d* pExtrusions /*= 0*/,
  const OdGsMarker* pSubEntMarkers/* = 0*/,
  OdInt32 nPointSize/* = 0*/) {
  if (pNormals)
    xformNormals((OdUInt32)numPoints, pNormals, m_vertexNormals);
  if (pExtrusions)
    xformVectors((OdUInt32)numPoints, pExtrusions, m_faceNormals);
  m_pDestGeom->polypointProc(numPoints, xformPoints((OdUInt32)numPoints, vertexList),
    pColors, pTransparency,
    (pNormals) ? m_vertexNormals.getPtr() : NULL,
    (pExtrusions) ? m_faceNormals.getPtr() : NULL,
    pSubEntMarkers, nPointSize);
}

void AcGiXform::rowOfDotsProc(
  OdInt32 numPoints,
  const OdGePoint3d& startPoint,
  const OdGeVector3d& dirToNextPoint) {
  m_pDestGeom->rowOfDotsProc(numPoints, xform * startPoint, xform * dirToNextPoint);
}

void AcGiXform::edgeProc(
  const OdGiEdge2dArray& edges,
  const OdGeMatrix3d* pXform /*= 0*/) {
  OdGeMatrix3d form = xform;
  if (pXform)
    form *= (*pXform);
  m_pDestGeom->edgeProc(edges, &form);
}

const OdGeVector3d* AcGiXform::xformExtrusion(const OdGeVector3d* pExtrusion) {
  if (pExtrusion){
    m_extrusion.setToProduct(xform, *pExtrusion);
    if (!m_extrusion.isZeroLength(OdGeContext::gZeroTol))
      return &m_extrusion;
  }

  return NULL;
}

const OdGeVector3d* AcGiXform::xformNormal(const OdGeVector3d* pNormal) {
  if (pNormal) {
    m_normal.setToProduct(xform, *pNormal);
    double lenSqrd = m_normal.lengthSqrd();
    double epsSqrd = OdGeContext::gZeroTol.equalVector()*OdGeContext::gZeroTol.equalVector();
    if (OdZero(lenSqrd, epsSqrd))
      return 0;
    if (!OdZero(lenSqrd - 1., epsSqrd))
      m_normal *= 1. / sqrt(lenSqrd);
    return &m_normal;
  }
  return NULL;
}

const OdGePoint3d* AcGiXform::xformPoints(OdUInt32 nPoints, const OdGePoint3d* p3dPoints) {
  if (xform.IsIdentity())
    return p3dPoints;

  if (m_outPoints.size() < nPoints)
    m_outPoints.resize(nPoints);

  OdGePoint3d* pPoint = m_outPoints.asArrayPtr();
  for (; nPoints--; ++pPoint, ++p3dPoints)
    *pPoint = xform * *p3dPoints;

  return m_outPoints.getPtr();
}

void AcGiXform::xformVectors(OdUInt32 nVectors, const OdGeVector3d* p3dVectors, OdGeVector3dArray& outVectors) {
  xformNormals(nVectors, p3dVectors, outVectors);
}

void AcGiXform::xformNormals(OdUInt32 nVectors, const OdGeVector3d* p3dVectors, OdGeVector3dArray& outVectors) {
  outVectors.resize(nVectors);
  OdGeVector3d* pVector = outVectors.asArrayPtr();
  for (; nVectors--; ++pVector, ++p3dVectors)
    *pVector = xform * (*p3dVectors);
}

void AcGiXform::xformVertexNormals(OdUInt32 nVertexNormals, const OdGiVertexData*& pVertexData) {
  if (pVertexData && pVertexData->normals()){
    xformNormals(nVertexNormals, pVertexData->normals(), m_vertexNormals);
    m_vertexData = *pVertexData;
    m_vertexData.setNormals(m_vertexNormals.getPtr());
    pVertexData = &m_vertexData;
  }
}

void AcGiXform::xformFaceNormals(OdUInt32 nFaceNormals, const OdGiFaceData*& pFaceData) {
  if (pFaceData && pFaceData->normals()){
    xformNormals(nFaceNormals, pFaceData->normals(), m_faceNormals);
    m_faceData = *pFaceData;
    m_faceData.setNormals(m_faceNormals.getPtr());
    pFaceData = &m_faceData;
  }
}

void AcGiXform::reverseFaceList(OdInt32 faceListSize, const OdInt32*& pFaceList) {
  int curNum(0), i;
  m_faceList.resize(faceListSize);
  for (; curNum < faceListSize; )
  {
    int num = pFaceList[curNum];
    m_faceList[curNum] = num;
    if (num < 0)
      num = -num;
    curNum++;
    m_faceList[curNum] = pFaceList[curNum];
    for (i = 1; i < num; i++) // loop started from 1, it isn't misprint
    {
      m_faceList[curNum + i] = pFaceList[curNum + (num - i)];
    }
    curNum += num;
  }
  pFaceList = m_faceList.asArrayPtr();
}

void AcGiXform::reverseEdgeData(OdInt32 faceListSize, const OdInt32* pFaceList, const OdGiEdgeData*& pEdgeData) {
  if (pEdgeData)
  {
    const OdUInt8* pVisibilities = pEdgeData->visibility();
    const OdGsMarker* pSelectionMarkers = pEdgeData->selectionMarkers();
    OdDbStub** pLinetypeIds = pEdgeData->linetypeIds();
    OdDbStub** pLayerIds = pEdgeData->layerIds();
    const OdCmEntityColor* pTrueColors = pEdgeData->trueColors();
    const OdUInt16* pColors = pEdgeData->colors();

    if (pVisibilities)
      m_edgeVisibilities.resize(faceListSize - 1);
    if (pSelectionMarkers)
      m_edgeSelectionMarkers.resize(faceListSize - 1);
    if (pLinetypeIds)
      m_edgeLinetypeIds.resize(faceListSize - 1);
    if (pLayerIds)
      m_edgeLayerIds.resize(faceListSize - 1);
    if (pTrueColors)
      m_edgeTrueColors.resize(faceListSize - 1);
    if (pColors)
      m_edgeColors.resize(faceListSize - 1);

    int curNum(0), curEdgeNum(0), i;
    for (; curNum < faceListSize; )
    {
      int num = pFaceList[curNum];
      if (num < 0)
        num = -num;
      curNum++;

      for (i = 0; i < num; i++)
      {
        int j = curEdgeNum + i;
        int k = curEdgeNum + (num - i - 1);

        if (pVisibilities)
          m_edgeVisibilities[j] = pVisibilities[k];
        if (pSelectionMarkers)
          m_edgeSelectionMarkers[j] = pSelectionMarkers[k];
        if (pLinetypeIds)
          m_edgeLinetypeIds[j] = pLinetypeIds[k];
        if (pLayerIds)
          m_edgeLayerIds[j] = pLayerIds[k];
        if (pTrueColors)
          m_edgeTrueColors[j] = pTrueColors[k];
        if (pColors)
          m_edgeColors[j] = pColors[k];
      }
      curNum += num;
      curEdgeNum += num;
    }
    m_edgeData = OdGiEdgeData(); // zero all pointers
    if (pVisibilities)
      m_edgeData.setVisibility(m_edgeVisibilities.asArrayPtr());
    if (pSelectionMarkers)
      m_edgeData.setSelectionMarkers(m_edgeSelectionMarkers.asArrayPtr());
    if (pLinetypeIds)
      m_edgeData.setLinetypes(m_edgeLinetypeIds.asArrayPtr());
    if (pLayerIds)
      m_edgeData.setLayers(m_edgeLayerIds.asArrayPtr());
    if (pTrueColors)
      m_edgeData.setTrueColors(m_edgeTrueColors.asArrayPtr());
    if (pColors)
      m_edgeData.setColors(m_edgeColors.asArrayPtr());
    pEdgeData = &m_edgeData;
  }
}

void AcGiXform::setDestGeometry(OdGiConveyorGeometry& destGeometry) {
  m_pDestGeom = &destGeometry;
}

OdGiConveyorGeometry& AcGiXform::destGeometry() const {
  return *m_pDestGeom;
}