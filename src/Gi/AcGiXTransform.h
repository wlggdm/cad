#ifndef _GI_TRANSFORM_H_
#define _GI_TRANSFORM_H_

#include "OdaCommon.h"
#include "AcGiMatrix3d.h"
#include "GePoint3d.h"
#include "GeVector3d.h"

#include "oda/source/gi/GiConveyorNodeImpl.h"

#include "GeCircArc3d.h"
#include "GeEllipArc3d.h"
#include "GeNurbCurve3d.h"
#include "GsMarkerArray.h"
#include "DbStubPtrArray.h"
#include "CmEntityColorArray.h"


class AcGiXform : public OdGiConveyorOutput,
                private OdGiConveyorGeometry {
public:
  AcGiXform();

  void setTransform(const OdGeMatrix3d& form);

  void circleProc(const OdGePoint3d& center, 
    double radius, 
    const OdGeVector3d& normal, 
    const OdGeVector3d* pExtrusion = 0) override;

  void circleProc(const OdGePoint3d& start, 
    const OdGePoint3d& point, 
    const OdGePoint3d& end, 
    const OdGeVector3d* pExtrusion = 0) override;

  void circularArcProc(const OdGePoint3d& center,
    double radius,
    const OdGeVector3d& normal,
    const OdGeVector3d& startVector,
    double sweepAngle,
    OdGiArcType arcType = kOdGiArcSimple,
    const OdGeVector3d* pExtrusion = 0) override;

  void circularArcProc(const OdGePoint3d& start, 
    const OdGePoint3d& point, 
    const OdGePoint3d& end, 
    OdGiArcType arcType, 
    const OdGeVector3d* pExtrusion = 0) override;

  void ellipArcProc(
    const OdGeEllipArc3d& arc,
    const OdGePoint3d* pEndPointsOverrides = 0,
    OdGiArcType arcType = kOdGiArcSimple, 
    const OdGeVector3d* pExtrusion = 0) override;

  void polylineProc(OdInt32 nbPoints, 
    const OdGePoint3d* pVertexList,
    const OdGeVector3d* pNormal = 0,
    const OdGeVector3d* pExtrusion = 0,
    OdGsMarker lBaseSubEntMarker = -1) override;

  void polygonProc(OdInt32 nbPoints, 
    const OdGePoint3d* pVertexList,
    const OdGeVector3d* pNormal = 0, 
    const OdGeVector3d* pExtrusion = 0) override;

  void meshProc(OdInt32 rows,
    OdInt32 columns,
    const OdGePoint3d* pVertexList,
    const OdGiEdgeData* pEdgeData,
    const OdGiFaceData* pFaceData,
    const OdGiVertexData* pVertexData) override;

  void shellProc(OdInt32 nbVertex,
    const OdGePoint3d* pVertexList,
    OdInt32 faceListSize,
    const OdInt32* pFaceList,
    const OdGiEdgeData* pEdgeData,
    const OdGiFaceData* pFaceData,
    const OdGiVertexData* pVertexData) override;

  void ttfPolyDrawProc(OdInt32 nbVertex, 
    const OdGePoint3d* pVertexList,
    OdInt32 faceListSize, 
    const OdInt32* pFaceList,
    const OdUInt8* pBezierTypes, 
    const OdGiFaceData* pFaceData = 0) override;

  void textProc(const OdGePoint3d& position,
    const OdGeVector3d& u, 
    const OdGeVector3d& v,
    const OdChar* msg, 
    OdInt32 length, 
    bool raw, 
    const OdGiTextStyle* pTextStyle,
    const OdGeVector3d* pExtrusion = 0) override;

  void textProc2(const OdGePoint3d& position,
    const OdGeVector3d& u, 
    const OdGeVector3d& v,
    const OdChar* msg, 
    OdInt32 length, 
    bool raw, const 
    OdGiTextStyle* pTextStyle,
    const OdGeVector3d* pExtrusion = 0, 
    const OdGeExtents3d* extentsBox = 0) override;

  int ttfCharProcFlags() const override;
  bool ttfCharProc(OdChar character, 
    const OdGePoint3d& position, 
    const OdGeBoundBlock3d* pBoundBlock) override;

  void shapeProc(const OdGePoint3d& position,
    const OdGeVector3d& u, 
    const OdGeVector3d& v,
    int shapeNo, 
    const OdGiTextStyle* pStyle,
    const OdGeVector3d* pExtrusion = 0)override;

  void xlineProc(const OdGePoint3d&, const OdGePoint3d&) override;

  void rayProc(const OdGePoint3d&, const OdGePoint3d&) override;

  void xlineProc2(const OdGePoint3d& basePoint, const OdGeVector3d& direction) override;
  void rayProc2(const OdGePoint3d& basePoint, const OdGeVector3d& direction) override;

  void setExtentsProc(const OdGePoint3d *pPoints, bool bTransform = true) override;

  void conveyorBoundaryInfoProc(const OdGeBoundBlock3d &boundBlock, OdUInt32 &outputFlags) override;

  void nurbsProc(const OdGeNurbCurve3d& nurbs) override;

  void rasterImageProc(
    const OdGePoint3d& origin,
    const OdGeVector3d& u,
    const OdGeVector3d& v,
    const OdGiRasterImage* pImg, // image object
    const OdGePoint2d* uvBoundary, // may not be null
    OdUInt32 numBoundPts,
    bool transparency = false,
    double brightness = 50.0,
    double contrast = 50.0,
    double fade = 0.0) override;

  void metafileProc(
    const OdGePoint3d& origin,
    const OdGeVector3d& u,
    const OdGeVector3d& v,
    const OdGiMetafile* pMetafile,
    bool bDcAligned = true,
    bool bAllowClipping = false) override;

  void plineProc(const OdGiPolyline& lwBuf,
    const OdGeMatrix3d* pXform = 0,
    OdUInt32 fromIndex = 0,
    OdUInt32 numSegs = 0) override;

  void polypointProc(OdInt32 numPoints,
    const OdGePoint3d* vertexList,
    const OdCmEntityColor* pColors,
    const OdCmTransparency* pTransparency = 0,
    const OdGeVector3d* pNormals = 0,
    const OdGeVector3d* pExtrusions = 0,
    const OdGsMarker* pSubEntMarkers = 0,
    OdInt32 nPointSize = 0) override;

  void rowOfDotsProc(
    OdInt32 numPoints, 
    const OdGePoint3d& startPoint, 
    const OdGeVector3d& dirToNextPoint) override;

  void edgeProc(
    const OdGiEdge2dArray& edges, 
    const OdGeMatrix3d* pXform = 0) override;

  void setDestGeometry(OdGiConveyorGeometry& destGeometry) override;

  OdGiConveyorGeometry& destGeometry() const override;
private:
  const OdGeVector3d* xformExtrusion(const OdGeVector3d* pExtrusion);
  const OdGeVector3d* xformNormal(const OdGeVector3d* pNormal);

  const OdGePoint3d* xformPoints(OdUInt32 nPoints, const OdGePoint3d* p3dPoints);
  void xformVectors(OdUInt32 nVectors, const OdGeVector3d* p3dVectors, OdGeVector3dArray& outVectors);
  void xformNormals(OdUInt32 nVectors, const OdGeVector3d* p3dVectors, OdGeVector3dArray& outVectors);
  void xformVertexNormals(OdUInt32 nVertexNormals, const OdGiVertexData*& pVertexData);
  void xformFaceNormals(OdUInt32 nFaceNormals, const OdGiFaceData*& pFaceData);
  void reverseFaceList(OdInt32 faceListSize, const OdInt32*& pFaceList);
  void reverseEdgeData(OdInt32 faceListSize, const OdInt32* pFaceList, const OdGiEdgeData*& pEdgeData);

  OdGeCircArc3d&   CircArc3d() { return circle; }
  OdGeEllipArc3d&  EllipArc3d() { return ellipse; }
  OdGeNurbCurve3d& NurbCurve3d() { return nurbCurve;}

private:
  AcGiMatrix3d  xform;
  OdGiConveyorGeometry* m_pDestGeom;

  OdGeCircArc3d    circle;
  OdGeEllipArc3d   ellipse;
  OdGeNurbCurve3d  nurbCurve;

  OdGePoint3dArray sample_points;

  OdGeVector3d m_extrusion;
  OdGeVector3d m_normal;
  OdGePoint3dArray m_outPoints;

  OdGeVector3dArray       m_faceNormals;
  OdGeVector3dArray       m_vertexNormals;
  OdInt32Array            m_faceList;

  OdUInt8Array            m_edgeVisibilities;
  OdGsMarkerArray         m_edgeSelectionMarkers;
  OdDbStubPtrArray        m_edgeLinetypeIds;
  OdDbStubPtrArray        m_edgeLayerIds;
  OdCmEntityColorArray    m_edgeTrueColors;
  OdUInt16Array           m_edgeColors;

  OdGiEdgeData            m_edgeData;
  OdGiFaceData            m_faceData;
  OdGiVertexData          m_vertexData;
};

#endif