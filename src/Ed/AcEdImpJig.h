#ifndef   _ACED_IMP_JIG_H_
#define   _ACED_IMP_JIG_H_

#include "dbjig.h"
#include "AcEdContext.h"


struct AcEdRequest {
  enum Type {
    kNone,
    kSelect = 0X0001,
    kString = 0X0002,
    kPoint  = 0X0004, 
    kDist   = 0X0008, 
    kAngle  = 0X0010,
    kBase   = 0X0020,
    kBasePoint = kPoint | kBase,
    kBaseDist  = kDist  | kBase,
    kBaseAngle = kAngle | kBase,
  };

  Type type;
  AcGePoint3d  base_point;
  AcGePoint3d* point;
  double*      value;
};


struct AcEdResponse {
  enum Type {
    kNone,
    kString,
    kPoint,
    kDist,
    kAngle,
    kCancel,
  };
  Type        type;
  AcGePoint3d point;
  OdString    string;
  double      value;
};

class AcEdImpJig : public AcEdContextSwitch {
public: 
  AcEdImpJig();
  ~AcEdImpJig() override;

  virtual AcDbObjectId append(AcDbEntity*);

  AcEdJig::DragStatus  drag(AcEdJig* jig);

  const ACHAR* keywordList();
  void         setKeywordList(const ACHAR*);

  const ACHAR* dispPrompt();
  void         setDispPrompt(const ACHAR*, ...);

  AcEdJig::DragStatus acquireString(ACHAR(&str)[2049]);
  AcEdJig::DragStatus acquireAngle(double &ang);
  AcEdJig::DragStatus acquireAngle(double &ang, const AcGePoint3d& basePnt);
  AcEdJig::DragStatus acquireDist(double &dist);
  AcEdJig::DragStatus acquireDist(double &dist, const AcGePoint3d& basePnt);
  AcEdJig::DragStatus acquirePoint(AcGePoint3d&);
  AcEdJig::DragStatus acquirePoint(AcGePoint3d&, const AcGePoint3d&basePnt);

  AcEdJig::CursorType specialCursorType();
  void                setSpecialCursorType(AcEdJig::CursorType);

  AcEdJig::UserInputControls userInputControls();
  void                       setUserInputControls(AcEdJig::UserInputControls);

  void Run() override;

  void pickPoint(const AcGePoint3d& point);
  void trackPoint(const AcGePoint3d& point);

private:
  AcEdJig::CursorType cursor_type_;

  AcEdJig::UserInputControls user_controls_;

  void* context_;

  AcEdJig*     jig_;

  AcEdRequest  request_;

  AcEdResponse response_;
};

#endif //_ACED_IMP_JIG_H_