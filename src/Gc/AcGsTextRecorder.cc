#include "AcGsTextRecorder.h"
#include "AcGsDisplay.h"
#include "AcGsDisplayContextImpl.h"

namespace {
  const double kShxFontHeight = 1.0;
  const double kTrueTypeFontHeight = 25.0; //5^2
}

AcGsTextRecorder::AcGsTextRecorder() : shx_font_(false){
}

AcGsTextRecorder::~AcGsTextRecorder() {
}

void AcGsTextRecorder::display(AcGsDisplayContext& context) {
#if 1
  const AcGeMatrix3d* matrix = static_cast<AcGsDisplayContextImpl&>(context).worldToDevice();

  AcGeVector3d v = *matrix * vheight_;
  double height_square = v.x*v.x + v.y*v.y;

  if (height_square <= kShxFontHeight) {
    degenerate1_.display(context);
    return;
  }

  if (!shx_font_ && height_square < kTrueTypeFontHeight)
    degenerate5_.display(context);
  else
#endif
    generate_.display(context);
}

void AcGsTextRecorder::setHeightVector(const AcGeVector3d& v) {
  vheight_ = v;
}

void AcGsTextRecorder::setShxFont(bool shx_font) {
  shx_font_ = shx_font;
}

bool AcGsTextRecorder::extents(OdGeExtents3d& extents) const {
  return shx_font_ ? generate_.extents(extents) : degenerate5_.extents(extents);
}