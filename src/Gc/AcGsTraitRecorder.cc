#include "AcGsTraitRecorder.h"
#include "ColorMapping.h"

#define SWAPBYTES(a) ODRGB(ODGETBLUE(a), ODGETGREEN(a), ODGETRED(a))

AcGsTraitRecorder::AcGsTraitRecorder() : flag_(kNone){
}

AcGsTraitRecorder::~AcGsTraitRecorder() {
}

void AcGsTraitRecorder::display(AcGsDisplayContext& context) {
  AcGsRender* render = context.render();
  if (flag_ & kColor)
    render->setColor(color_);

  if (flag_ & kMarker)
    render->setGsMarker(marker_);
}

void AcGsTraitRecorder::setColor(const OdCmEntityColor& color) {
  if (color.isByACI() || color.isByDgnIndex())
    color_ = odcmAcadPalette(0XFF000000)[color.colorIndex()];
  else
    color_ = ODTOCOLORREF(color);

  color_ = 0XFF000000 | SWAPBYTES(color_);

  flag_ |= kColor;
}

void AcGsTraitRecorder::setDrawFlag(int draw_flag) {
  draw_flags_ = draw_flag;

  flag_ |= kDrawFlag;
}

void AcGsTraitRecorder::setGsMarker(const OdGsMarker& marker) {
  marker_ = marker;

  flag_ |= kMarker;
}
