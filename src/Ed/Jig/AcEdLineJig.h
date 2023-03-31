#ifndef   _ACED_LINE_JIG_H_
#define   _ACED_LINE_JIG_H_

#include "dbjig.h"
#include "DbLine.h"

class AsdkLineJig : public AcEdJig {
public:
  AsdkLineJig();

  void doIt();

  DragStatus     sampler() override;
  Adesk::Boolean update() override;
  AcDbEntity*    entity() const override;

private:
  OdDbLine*   line_;

  AcGePoint3d start_;

  AcGePoint3d end_;

  int         prompt_counter_;
};


void createLine();


#endif //_ACED_LINE_JIG_H_