#ifndef _ACGS_LAYER_NODE_H_
#define _ACGS_LAYER_NODE_H_

#include "AcGsNodeImpl.h"
#include "AcGsDisplay.h"
#include "GiLayerTraitsData.h"
#include "OdVector.h"

class AcGsLayerNode : public AcGsNodeImpl, public AcGsDisplay {
public:
  AcGsLayerNode();
  ~AcGsLayerNode() override;

  void update(AcGiVectorizer& vectorizer) override;

  const OdGiLayerTraitsData& layerTraits(OdUInt32 vp) const;

private:
  using OdGiLayerTraitsDataArray = OdVector<OdGiLayerTraitsData, OdMemoryAllocator<OdGiLayerTraitsData>>;

  OdGiLayerTraitsDataArray layer_traits_;
};


#endif //_ACGS_LAYER_NODE_H_