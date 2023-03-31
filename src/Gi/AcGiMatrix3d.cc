#include "AcGiMatrix3d.h"

AcGiMatrix3d::AcGiMatrix3d() : formType(kIdentity){
}

AcGiMatrix3d::AcGiMatrix3d(const OdGeMatrix3d& matrix) :
  OdGeMatrix3d(matrix) {
  Evaluate();
}

AcGiMatrix3d::AcGiMatrix3d(const AcGiMatrix3d& matrix) : 
  OdGeMatrix3d(matrix),
  formType(matrix.formType){
}

const AcGiMatrix3d& AcGiMatrix3d::operator=(const OdGeMatrix3d& matrix) {
  OdGeMatrix3d::operator=(matrix);
  Evaluate();

  return *this;
}

const AcGiMatrix3d& AcGiMatrix3d::operator=(const AcGiMatrix3d& matrix) {
  OdGeMatrix3d::operator=(matrix);
  formType = matrix.formType;

  return *this;
}

AcGiMatrix3d& AcGiMatrix3d::operator*=(const OdGeMatrix3d& matrix) {
  OdGeMatrix3d::operator*=(matrix);
  Evaluate();

  return *this;
}

AcGiMatrix3d& AcGiMatrix3d::operator*=(const AcGiMatrix3d& matrix) {
  OdGeMatrix3d::operator*=(matrix);
  formType = matrix.formType;

  return *this;
}

AcGiMatrix3d AcGiMatrix3d::operator*(const OdGeMatrix3d& matrix) const{
  AcGiMatrix3d mat;
  mat *= matrix;
  return mat;
}

AcGiMatrix3d AcGiMatrix3d::operator*(const AcGiMatrix3d& matrix) const{
  AcGiMatrix3d mat;
  mat *= matrix;
  return mat;
}

OdGeExtents3d AcGiMatrix3d::operator*(const OdGeExtents3d& extent) const {
  //copy from OdGeExtents3d::transformBy
  OdGeExtents3d ext;
  OdGePoint3d origin = this->operator*(extent.minPoint());
  ext.set(origin, origin);

  OdGeVector3d vec = extent.maxPoint() - extent.minPoint();
  if (OdNonZero(vec.x, 1.e-200))
    ext.expandBy(this->operator* (OdGeVector3d::kXAxis * vec.x));
  if (OdNonZero(vec.y, 1.e-200))
    ext.expandBy(this->operator* (OdGeVector3d::kYAxis * vec.y));
  if (OdNonZero(vec.z, 1.e-200))
    ext.expandBy(this->operator* (OdGeVector3d::kZAxis * vec.z));

  return ext;
}

void AcGiMatrix3d::Evaluate() {
  formType = kIdentity;

  double tolerance = std::numeric_limits<double>::epsilon();

  if (fabs(entry[0][3]) > tolerance ||
    fabs(entry[1][3]) > tolerance ||
    fabs(entry[2][3]) > tolerance)
    formType |= kTranslate;

  if (fabs(entry[0][0] - 1.0) > tolerance ||
    fabs(entry[1][1] - 1.0) > tolerance ||
    fabs(entry[2][2] - 1.0) > tolerance)
    formType |= kScale;

  if (fabs(entry[0][1]) > tolerance || fabs(entry[0][2]) > tolerance ||
     fabs(entry[1][0]) > tolerance || fabs(entry[1][2]) > tolerance ||
     fabs(entry[3][0]) > tolerance || fabs(entry[3][1]) > tolerance)
    formType |= kRotate;

  if (fabs(entry[3][0]) > tolerance || 
    fabs(entry[3][1]) > tolerance ||
    fabs(entry[3][2]) > tolerance ||
    fabs(entry[3][3] - 1.0) > tolerance)
    formType |= kPerspective;
}

OdGePoint3d AcGiMatrix3d::operator*(const OdGePoint3d& point) const{
  OdGePoint3d	result;

  if (formType == kIdentity)
    return point;
  else if (formType == kTranslate) {
    result.x = point.x + entry[0][3];
    result.y = point.y + entry[1][3];
    result.z = point.z + entry[2][3];
  }
  else if (formType == kScale || formType == (kScale | kTranslate)) {
    result.x = entry[0][0] * point.x;
    result.y = entry[1][1] * point.y;
    result.z = entry[2][2] * point.z;
    if (formType & kTranslate) {
      result.x += entry[0][3];
      result.y += entry[1][3];
      result.z += entry[2][3];
    }
  }
  else if ((formType & kPerspective) == 0) {
    result.x = entry[0][0] * point.x + entry[0][1] * point.y + entry[0][2] * point.z;
    result.y = entry[1][0] * point.x + entry[1][1] * point.y + entry[1][2] * point.z;
    result.z = entry[2][0] * point.x + entry[2][1] * point.y + entry[2][2] * point.z;

    if (formType & kTranslate) {
      result.x += entry[0][3];
      result.y += entry[1][3];
      result.z += entry[2][3];
    }
  }
  else {
    double divider = entry[3][0] * point.x + entry[3][1] * point.y + entry[3][2] * point.z + entry[3][3];
    if (fabs(divider) < std::numeric_limits<double>::epsilon())
      return point;

    divider = 1 / divider;
    
    result.x = (entry[0][0] * point.x + entry[0][1] * point.y + entry[0][2] * point.x + entry[0][3]) * divider;
    result.y = (entry[1][0] * point.x + entry[1][1] * point.y + entry[1][2] * point.x + entry[1][3]) * divider;
    result.z = (entry[2][0] * point.x + entry[2][1] * point.y + entry[2][2] * point.x + entry[2][3]) * divider;
  }

  return result;
}

OdGeVector3d AcGiMatrix3d::operator*(const OdGeVector3d& vec) const {
  if (formType & kRotate)
    return ((OdGeMatrix3d)*this) * vec;
  else if (formType & kScale) {
    OdGeVector3d	result(
      vec.x * entry[0][0],
      vec.y * entry[1][1],
      vec.z * entry[2][2]);

    return result;
  }

  return vec;
}

OdGePoint3d operator*(const OdGePoint3d& point, const AcGiMatrix3d& matrix) {
  return matrix * point;
}