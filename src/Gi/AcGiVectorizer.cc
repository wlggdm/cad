#include "OdaCommon.h"
#include "AcGiVectorizer.h"


AcGiVectorizer::AcGiVectorizer() {
  model_transform_.push(OdGeMatrix3d::kIdentity);
  gi_context = nullptr;

  block_trait_data_ = 0;

  drawable_attribute_ = 0;

  xform.setDestGeometry(*this);

  view_ = nullptr;

  setDeviation(this);
  /*
  OdGePoint3d devAt;
  double val = ::ldexp(.5, int(OD_LOGB(deviation(kOdGiMaxDevForCircle, devAt))) + 1);
  deviation_[kOdGiMaxDevForCircle] = val;
  val = ::ldexp(.5, int(OD_LOGB(deviation(kOdGiMaxDevForCurve, devAt))) + 1);
  deviation_[kOdGiMaxDevForCurve   ] = val;
  deviation_[kOdGiMaxDevForBoundary] = val;
  deviation_[kOdGiMaxDevForIsoline ] = val;
  deviation_[kOdGiMaxDevForFacet   ] = val;
  */

  shell_recorder_ = nullptr;

  generate_wires_ = true;

  generate_shell_ = true;
}

AcGiVectorizer::~AcGiVectorizer() {
}

void AcGiVectorizer::setContext(OdGiContext* pUserContext) {
  gi_context = pUserContext;
}

//OdGiWorldGeometry
void AcGiVectorizer::setExtents(const OdGePoint3d *newExtents) {

}

//OdGiCommonDraw
OdGiRegenType AcGiVectorizer::regenType() const {
  return kOdGiStandardDisplay;
}

bool AcGiVectorizer::regenAbort() const {
  return false;
}

OdGiSubEntityTraits& AcGiVectorizer::subEntityTraits() const {
  return *const_cast<AcGiVectorizer*>(this);
}

OdGiGeometry& AcGiVectorizer::rawGeometry() const {
  return *static_cast<OdGiWorldGeometry*>(const_cast<AcGiVectorizer*>(this));
}

bool AcGiVectorizer::isDragging() const {
  return false;
}

double AcGiVectorizer::deviation(const OdGiDeviationType deviationType, const OdGePoint3d& pointOnCurve) const {
    OdGePoint2d pixelsPerUnit;
    getNumPixelsInUnitSquare(pointOnCurve, pixelsPerUnit, false);
    pixelsPerUnit.x = fabs(pixelsPerUnit.x);
    pixelsPerUnit.y = fabs(pixelsPerUnit.y);

    if (deviationType == kOdGiMaxDevForCircle) {
      unsigned int viewres = context()->circleZoomPercent(viewportObjectId());
      if (viewres < 1 || viewres > 20000) 
        viewres = 100;
      double cft = double(viewres) / 100;
      pixelsPerUnit.x *= cft;
      pixelsPerUnit.y *= cft;
    }
    double deviation = (0.5 / odmax(pixelsPerUnit.x, pixelsPerUnit.y));
#if 0
    if (isPerspective()) {
      OdGePoint3d eyePt(pointOnCurve);
      eyePt.transformBy(view_->viewingMatrix());
      deviation *= 1.0 - eyePt.z / focalLength();
      if (deviation < 0.0)
        deviation = -deviation;
    }
#endif
    return deviation;
}

OdUInt32 AcGiVectorizer::numberOfIsolines() const {
  if (context() && context()->database())
    return ((OdDbDatabase*)context()->database())->getISOLINES();

  return 4;
}

OdGiContext* AcGiVectorizer::context() const {
  return gi_context;
}

const OdGiPathNode* AcGiVectorizer::currentGiPath() const {
  return nullptr;
}

//OdGiViewportDraw
OdGiViewport& AcGiVectorizer::viewport() const {
  return const_cast<AcGiVectorizer&>(*this);
}

OdUInt32 AcGiVectorizer::sequenceNumber() const {
  return 0;
}

bool AcGiVectorizer::isValidId(const OdUInt32 viewportId) const {
  const AcGsClientViewInfo& client_info = view_->getClientViewInfo();
  return viewportId == client_info.viewportId;
}

OdDbStub* AcGiVectorizer::viewportObjectId() const {
  const AcGsClientViewInfo& client_info = view_->getClientViewInfo();
  return reinterpret_cast<OdDbStub*>(client_info.viewportObjectId);
}

//OdGiViewportGeometry
void AcGiVectorizer::polylineEye(OdUInt32 numVertices, const OdGePoint3d* vertexList) {
  if (effectivelyVisible() && !regenAbort() && numVertices && vertexList) {
    onTraitsModified();

   
  }
}

void AcGiVectorizer::polygonEye(OdUInt32 numVertices, const OdGePoint3d* vertexList) {
  if (effectivelyVisible() && !regenAbort() && numVertices && vertexList) {
    onTraitsModified();

  }
}

void AcGiVectorizer::polylineDc(OdUInt32 numVertices, const OdGePoint3d* vertexList) {
  if (effectivelyVisible() && !regenAbort() && numVertices && vertexList) {
    onTraitsModified();


  }
}

void AcGiVectorizer::polygonDc(OdUInt32 numVertices, const OdGePoint3d* vertexList) {
  if (effectivelyVisible() && !regenAbort() && numVertices && vertexList) {
    onTraitsModified();

    OdGeMatrix3d eye_world = getEyeToWorldTransform();
    OdGePoint3d point;


  }
}

void AcGiVectorizer::rasterImageDc(
  const OdGePoint3d& origin,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  const OdGiRasterImage* pImage,
  const OdGePoint2d* uvBoundary,
  OdUInt32 numBoundPts,
  bool transparency /*= false*/,
  double brightness /*= 50.0*/,
  double contrast /*= 50.0*/,
  double fade/* = 0.0*/) {

}

void  AcGiVectorizer::metafileDc(
  const OdGePoint3d& origin,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  const OdGiMetafile* pMetafile,
  bool dcAligned /*= true*/,
  bool allowClipping /*= false*/) {

}

void AcGiVectorizer::ownerDrawDc(
  const OdGePoint3d& origin,
  const OdGeVector3d& u,
  const OdGeVector3d& v,
  const OdGiSelfGdiDrawable* pDrawable,
  bool dcAligned /*= true*/,
  bool allowClipping /*= false*/) {

}