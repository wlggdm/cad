#ifndef _ACGS_SELECT_VISITOR_H_
#define _ACGS_SELECT_VISITOR_H_

#include "si/SiSpatialIndex.h"
#include "acgs.h"
#include "GiPathNode.h"

#include "AcGsDisplay.h"

class OdGsSelectionReactor;
class AcGiVectorizer;

class AcGiPathNode : public OdGiPathNode {
public:
  AcGiPathNode();

  const OdGiPathNode* parent() const override;

  OdDbStub* persistentDrawableId() const override;

  const OdGiDrawable* transientDrawable() const override;

  OdGsMarker selectionMarker() const override;

private:
  AcGiPathNode* parent_;
  OdDbStub*     id_;
  OdGiDrawable* drawable_;

  OdGsMarker    marker_;
};

class AcGsSelectVisitor : public OdSiVisitor, public AcGsRender {
public:
  AcGsSelectVisitor(OdGsSelectionReactor*, AcGsDisplayContext* display_context, AcGiVectorizer* vectorizer);

  void visit( OdSiEntity* entity, bool completelyInside ) override;

  void setGsMarker(const OdGsMarker& marker) override;

  void polyline(Adesk::Int32 count, const AcGePoint3d* vertexs) override;

  void triangle(OdUInt32 count, const AcGePoint3d* vertexs) override;

  //void shell(OdUInt32 count, const OdGePoint3d* points,
  //           OdUInt32 index_count, const OdInt32* indexs) override;
private:
  OdGsSelectionReactor* selection_reactor_;

  AcGsDisplayContext*   display_context_;

  AcGiVectorizer*       vectorizer_;

  bool                  selected_;

  OdGsMarker            gs_markder_;

  AcGiPathNode          path_root_node_;
};

#endif //_ACGS_SELECT_VISITOR_H_