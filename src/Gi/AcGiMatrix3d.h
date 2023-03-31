#ifndef _GI_MATRIX3d_H_
#define _GI_MATRIX3d_H_

#include "GeMatrix3d.h"
#include "GePoint3d.h"
#include "GeVector3d.h"
#include "GeExtents3d.h"

class AcGiMatrix3d : public OdGeMatrix3d {
public:
  enum XformType {
    kIdentity    = 0,
    kTranslate   = 1,
    kScale       = 2,
    kRotate      = 4,
    kPerspective = 8,
  };

  AcGiMatrix3d();
  AcGiMatrix3d(const OdGeMatrix3d&);
  AcGiMatrix3d(const AcGiMatrix3d&);

  //operators
  const AcGiMatrix3d& operator=(const OdGeMatrix3d&);
  const AcGiMatrix3d& operator=(const AcGiMatrix3d&);

  AcGiMatrix3d operator*(const OdGeMatrix3d&) const;
  AcGiMatrix3d operator*(const AcGiMatrix3d&) const;

  AcGiMatrix3d& operator*=(const OdGeMatrix3d&);
  AcGiMatrix3d& operator*=(const AcGiMatrix3d&);

  OdGePoint3d  operator*(const OdGePoint3d&) const;
  OdGeVector3d operator*(const OdGeVector3d&) const;
  OdGeExtents3d operator*(const OdGeExtents3d&) const;

  bool IsIdentity() { return formType == kIdentity; }
  bool IsTranslate() const { return formType == kTranslate; }
  bool IsPerspective() const { return !!(formType & kPerspective); }

private:
  void Evaluate();

  int  formType;
};

OdGePoint3d operator*(const OdGePoint3d& point, const AcGiMatrix3d& matrix);
#endif