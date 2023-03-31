#include "AcEdLineJig.h"

AsdkLineJig::AsdkLineJig() {
  prompt_counter_ = 0;
}

void AsdkLineJig::doIt() {
  OdDbLinePtr line = OdDbLine::createObject();
  line_ = line.detach();

  AcEdJig::DragStatus stat = drag();

  prompt_counter_++;
  stat = drag();

  append();
}

AcEdJig::DragStatus AsdkLineJig::sampler() {
  DragStatus stat;
  /*
  setUserInputControls((UserInputControls)
      (AcEdJig::kAccept3dCoordinates
       | AcEdJig::kNoNegativeResponseAccepted
       | AcEdJig::kNoZeroResponseAccepted));
  */
  if (prompt_counter_ == 0) {
    static AcGePoint3d axisPointTemp;
    stat = acquirePoint(start_);
    if (stat == AcEdJig::kNormal)
      return AcEdJig::kNormal;
  } else if (prompt_counter_ == 1) {
    stat = acquirePoint(end_);
    if (stat == AcEdJig::kNormal)
      return AcEdJig::kNormal;
  }
  return stat;
}

Adesk::Boolean AsdkLineJig::update() {
  switch (prompt_counter_) {
    case 0:
    end_ = start_;
    break;
    case 1:
    break;
  }

  line_->setStartPoint(start_);
  line_->setEndPoint(end_);

  return Adesk::kTrue;
}

AcDbEntity* AsdkLineJig::entity() const {
  if (prompt_counter_ == 0)
    return nullptr;

  return line_;
}

void createLine() {
  AsdkLineJig *pJig = new AsdkLineJig();

  pJig->doIt();

  delete pJig;
}
