#include "AcEdImpJig.h"

AcEdImpJig::AcEdImpJig() : 
  cursor_type_(AcEdJig::kNoSpecialCursor),
  user_controls_(AcEdJig::kGovernedByOrthoMode),
  jig_(nullptr){
}

AcEdImpJig::~AcEdImpJig() {
}

AcDbObjectId AcEdImpJig::append(AcDbEntity* entity) {
  return AcDbObjectId::kNull;
}

AcEdJig::DragStatus AcEdImpJig::drag(AcEdJig* jig) {
  jig_ = jig;

  AcEdJig::DragStatus status = jig_->sampler();
  if (status == AcEdJig::kNormal && jig_->update() == Adesk::kTrue) {

  }

  jig_ = nullptr;

  return status;
}

const ACHAR* AcEdImpJig::keywordList() {
  return nullptr;
}

void AcEdImpJig::setKeywordList(const ACHAR*) {

}

const ACHAR* AcEdImpJig::dispPrompt() {
  return nullptr;
}

void AcEdImpJig::setDispPrompt(const ACHAR*, ...) {

}

AcEdJig::DragStatus AcEdImpJig::acquireString(ACHAR(&str)[2049]) {
  return AcEdJig::kCancel;
}

AcEdJig::DragStatus AcEdImpJig::acquireAngle(double &ang) {
  request_.type = AcEdRequest::kAngle;
  request_.value = &ang;
  SwitchToUI();
  request_.type = AcEdRequest::kNone;

  if (response_.type == AcEdResponse::kCancel)
    return AcEdJig::kCancel;

  if (response_.type == AcEdResponse::kDist)
    return AcEdJig::kNormal;

  return AcEdJig::kOther;
}

AcEdJig::DragStatus AcEdImpJig::acquireAngle(double &ang, const AcGePoint3d& basePnt) {
  request_.type = AcEdRequest::kBaseAngle;
  request_.value = &ang;
  SwitchToUI();
  request_.type = AcEdRequest::kNone;

  if (response_.type == AcEdResponse::kCancel)
    return AcEdJig::kCancel;
  /*
  if (response_.type == AcEdResponse::kBaseAngle) {
   
    return AcEdJig::kNormal;
  }
  */
  return AcEdJig::kOther;
}

AcEdJig::DragStatus AcEdImpJig::acquireDist(double &dist) {
  request_.type = AcEdRequest::kDist;
  request_.value = &dist;
  SwitchToUI();
  request_.type = AcEdRequest::kNone;

  if (response_.type == AcEdResponse::kCancel)
    return AcEdJig::kCancel;

  if (response_.type == AcEdResponse::kPoint)
    return acquireDist(dist, response_.point);

  if (response_.type == AcEdResponse::kDist){
    dist = response_.value;
    return AcEdJig::kNormal;
  }
  
  return AcEdJig::kOther;
}

AcEdJig::DragStatus AcEdImpJig::acquireDist(double &dist, const AcGePoint3d& basePnt) {
  request_.type = AcEdRequest::kDist;
  request_.value = &dist;
  SwitchToUI();
  request_.type = AcEdRequest::kNone;

  if (response_.type == AcEdResponse::kCancel)
    return AcEdJig::kCancel;

  if (response_.type == AcEdResponse::kPoint) {
    dist = (response_.point - basePnt).length();
    return AcEdJig::kNormal;
  }
  
  return AcEdJig::kOther;
}

AcEdJig::DragStatus AcEdImpJig::acquirePoint(AcGePoint3d& point) {
  request_.type = AcEdRequest::kPoint;
  request_.point = &point;
  SwitchToUI();
  request_.type = AcEdRequest::kNone;

  if (response_.type == AcEdResponse::kCancel)
    return AcEdJig::kCancel;

  point = response_.point;
  response_.type = AcEdResponse::kNone;

  return AcEdJig::kNormal;
}

AcEdJig::DragStatus AcEdImpJig::acquirePoint(AcGePoint3d& point, const AcGePoint3d& basePnt) {
  request_.type = AcEdRequest::kBasePoint;
  request_.base_point = basePnt;
  request_.point = &point;
  SwitchToUI();
  request_.type = AcEdRequest::kNone;

  if (response_.type == AcEdResponse::kCancel)
    return AcEdJig::kCancel;

  point = response_.point;
  response_.type = AcEdResponse::kNone;

  return AcEdJig::kNormal;
}

AcEdJig::CursorType AcEdImpJig::specialCursorType() {
  return cursor_type_;
}

void AcEdImpJig::setSpecialCursorType(AcEdJig::CursorType cursor_type) {
  cursor_type_ = cursor_type;
}

AcEdJig::UserInputControls AcEdImpJig::userInputControls() {
  return user_controls_;
}

void AcEdImpJig::setUserInputControls(AcEdJig::UserInputControls user_controls) {
  user_controls_ = user_controls;
}

#include "AcEdLineJig.h"
void AcEdImpJig::Run() {
  while (true) {
    //pop command
    createLine();

    //switch to ui for next command
    //if (commands.emtpty())
    SwitchToUI();
  }
}

void AcEdImpJig::pickPoint(const AcGePoint3d& pt) {
  if (!jig_ || request_.type == AcEdRequest::kNone)
    return;

  AcGePoint3d point = pt;

  /*
  AcEdOsnapManager.Snap(view, point, (request_.type & AcEdRequest::kBase) ? &request_.base_point : nullptr)
  */

  switch (request_.type) {
    case AcEdRequest::kPoint:
    case AcEdRequest::kBasePoint: {
      response_.type = AcEdResponse::kPoint;
      response_.point = point;
      SwitchToSelf();
      break;
    }
    case AcEdRequest::kDist: {
      request_.type = AcEdRequest::kBaseDist;
      request_.base_point = point;
      break;
    }
    case AcEdRequest::kBaseDist: {
      response_.type = AcEdResponse::kDist;
      response_.value = (point - request_.base_point).length();
      SwitchToSelf();
      break;
    }
    case AcEdRequest::kAngle: {
      request_.type = AcEdRequest::kBaseAngle;
      request_.base_point = point;
      break;
    }
    case AcEdRequest::kBaseAngle: {
      response_.type = AcEdResponse::kAngle;
      response_.value = (point - request_.base_point).length();
      SwitchToSelf();
      break;
    }
  }
}

void AcEdImpJig::trackPoint(const AcGePoint3d& pt) {
  if (!jig_ || request_.type == AcEdResponse::kNone)
    return;

  AcGePoint3d point = pt;
  
  /*
  AcEdOsnapManager.Snap(view, point, (request_.type & AcEdRequest::kBase) ? &request_.base_point : nullptr)
  */

  switch (request_.type) {
    case AcEdRequest::kPoint:
    case AcEdRequest::kBasePoint: {
      *request_.point = point;
      jig_->update();
      break;
    }
    case AcEdRequest::kBaseDist: {
      *request_.value = (point - request_.base_point).length();
      jig_->update();
    }
    case AcEdRequest::kBaseAngle: {
      //To Do  calculate angle
      *request_.value = (point - request_.base_point).length();
      jig_->update();
    }
  }
}
