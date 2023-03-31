#ifndef _ACGS_CONTAINDER_NOD_H_
#define _ACGS_CONTAINDER_NOD_H_

#include "acgs.h"
#include "si/SiSpatialIndex.h"
#include "base/containers/linked_list.h"

#include "AcGsEntityNode.h"

#define VP_DEPENDENT 1
class AcGsContainerNode : public AcGsNodeImpl, 
                          public AcGsDisplay,
                          public OdSiVisitor {
public:
  AcGsContainerNode();
  ~AcGsContainerNode() override;

  void add(AcGiDrawable*);
  void remove(AcGsEntityNode*);

  //AcGsDisplay
  void display(AcGsDisplayContext& context) override;
  void update(AcGiVectorizer& vectorizer) override;

  //OdSiVisitor
  void visit(OdSiEntity* entity, bool completelyInside) override;

  //Internal
  const OdSiSpatialIndex* spatialIndex(OdUInt32 vp_id) const;
private:
  base::LinkedList<AcGsEntityNode> entity_list_;

#ifdef VP_DEPENDENT
  std::vector<OdSiSpatialIndexPtr> spatial_indexs_;
#else
  OdSiSpatialIndexPtr     spatial_index_;
#endif
};

#endif //_ACGS_CONTAINDER_NOD_H_