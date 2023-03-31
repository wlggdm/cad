#ifndef _ACGS_TRAIT_RECORDER_H_
#define _ACGS_TRAIT_RECORDER_H_

#include "AcGsRecorderImpl.h"

class AcGsTraitRecorder : public AcGsRecorderImpl {
public:
  AcGsTraitRecorder();
  ~AcGsTraitRecorder() override;

  void display(AcGsDisplayContext&) override;

  bool hasExtent() const override { return false; }

  void setColor(const OdCmEntityColor& color);

  void setDrawFlag(int draw_flag);

  void setGsMarker(const OdGsMarker& marker);
private:
  enum TraitFlags {
    kNone = 0,
    kColor = 1,
    kMarker = 2,
    kDrawFlag = 4,
  };

  int              flag_;

  uint32_t         color_;
  
  int              draw_flags_;
 
  OdGsMarker       marker_;
};

#endif //_ACGS_TRAIT_RECORDER_H_