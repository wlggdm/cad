#include "OdaCommon.h"
#include "AcGiVectorizer.h"

OdGeMatrix3d AcGiVectorizer::getModelToEyeTransform() const {
  return model_view_transform_;
}

OdGeMatrix3d AcGiVectorizer::getEyeToModelTransform() const {
  return model_view_transform_.inverse();
}

OdGeMatrix3d AcGiVectorizer::getWorldToEyeTransform() const {
  return view_->viewingMatrix();
}

OdGeMatrix3d AcGiVectorizer::getEyeToWorldTransform() const {
  return view_->viewingMatrix().inverse();
}

bool AcGiVectorizer::isPerspective() const {
  return view_->isPerspective();
}

bool AcGiVectorizer::doPerspective(OdGePoint3d& point) const {
  return true;
}

bool AcGiVectorizer::doInversePerspective(OdGePoint3d& point) const {
  return true;
}

void AcGiVectorizer::getNumPixelsInUnitSquare(
  const OdGePoint3d& point,
  OdGePoint2d& pixelDensity,
  bool includePerspective /*= true*/) const {
  if (view_)
    view_->getNumPixelsInUnitSquare(point, pixelDensity, includePerspective);
}

OdGePoint3d AcGiVectorizer::getCameraLocation() const {
  return view_->position();
}

OdGePoint3d AcGiVectorizer::getCameraTarget() const {
  return view_->target();
}

OdGeVector3d AcGiVectorizer::getCameraUpVector() const {
  return view_->upVector();
}

OdGeVector3d AcGiVectorizer::viewDir() const {
  return view_->target() - view_->position();
}

OdUInt32 AcGiVectorizer::viewportId() const {
  return 1;
}

OdInt16 AcGiVectorizer::acadWindowId() const {
  return 1;
}

void AcGiVectorizer::getViewportDcCorners(
  OdGePoint2d& lowerLeft,
  OdGePoint2d& upperRight) const {
  return view_->getViewport(lowerLeft, upperRight);
}

bool AcGiVectorizer::getFrontAndBackClipValues(
  bool& clipFront,
  bool& clipBack,
  double& front,
  double& back) const {
  clipFront = view_->isFrontClipped();
  clipBack = view_->isBackClipped();
  front = view_->frontClip();
  back = view_->backClip();
  return true;
}

double AcGiVectorizer::linetypeScaleMultiplier() const {
  return 1.0;
}

double AcGiVectorizer::linetypeGenerationCriteria() const {
  return 1.0;
}

bool AcGiVectorizer::layerVisible(OdDbStub* layerId) const {
  return true;
}

const OdGiContextualColors *AcGiVectorizer::contextualColors() const {
  return nullptr;
}