#include "dbjig.h"
#include "AcEdContext.h"
#include "AcEdImpJig.h"

AcEdJig::AcEdJig() {
  mpImpJig = AcEdContext::JigHost();
}

AcEdJig::~AcEdJig() {
}

AcEdJig::DragStatus AcEdJig::drag() {
  return mpImpJig->drag(this);
}

AcEdJig::DragStatus AcEdJig::sampler() {
  return kCancel;
}

Adesk::Boolean AcEdJig::update() {
  return false;
}

AcDbObjectId AcEdJig::append() {
  return mpImpJig->append(entity());
}

const ACHAR* AcEdJig::keywordList() {
  return mpImpJig->keywordList();
}

void AcEdJig::setKeywordList(const ACHAR* keywords) {
  mpImpJig->setKeywordList(keywords);
}

const ACHAR* AcEdJig::dispPrompt() {
  return mpImpJig->dispPrompt();
}

void AcEdJig::setDispPrompt(const ACHAR* disp_prompt, ...) {
  mpImpJig->setDispPrompt(disp_prompt);
}

AcEdJig::DragStatus AcEdJig::acquireString(ACHAR(&str)[2049]) {
  return mpImpJig->acquireString(str);
}

AcEdJig::DragStatus AcEdJig::acquireAngle(double &ang) {
  return mpImpJig->acquireAngle(ang);
}

AcEdJig::DragStatus AcEdJig::acquireAngle(double &ang, const AcGePoint3d& basePnt) {
  return mpImpJig->acquireAngle(ang, basePnt);
}

AcEdJig::DragStatus AcEdJig::acquireDist(double &dist) {
  return mpImpJig->acquireDist(dist);
}

AcEdJig::DragStatus AcEdJig::acquireDist(double &dist, const AcGePoint3d& basePnt) {
  return mpImpJig->acquireDist(dist, basePnt);
}

AcEdJig::DragStatus AcEdJig::acquirePoint(AcGePoint3d& point) {
  return mpImpJig->acquirePoint(point);
}

AcEdJig::DragStatus AcEdJig::acquirePoint(AcGePoint3d& point, const AcGePoint3d&basePnt) {
  return mpImpJig->acquirePoint(point, basePnt);
}

AcEdJig::CursorType AcEdJig::specialCursorType() {
  return mpImpJig->specialCursorType();
}

void AcEdJig::setSpecialCursorType(CursorType cursor_type) {
  mpImpJig->setSpecialCursorType(cursor_type);
}

AcEdJig::UserInputControls AcEdJig::userInputControls() {
  return mpImpJig->userInputControls();
}

void AcEdJig::setUserInputControls(AcEdJig::UserInputControls user_controls) {
  mpImpJig->setUserInputControls(user_controls);
}

AcDbEntity* AcEdJig::entity() const {
  return nullptr;
}
