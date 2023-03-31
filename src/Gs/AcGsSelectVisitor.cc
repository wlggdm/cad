#include "AcGsSelectVisitor.h"
#include "gs/GsSelectionReactor.h"
#include "GiViewport.h"
#include "AcGsEntityNode.h"
#include "AcGiVectorizer.h"

AcGsSelectVisitor::AcGsSelectVisitor(OdGsSelectionReactor* selection_reactor, AcGsDisplayContext* display_context, AcGiVectorizer* vectorizer) :
  selection_reactor_(selection_reactor),
  display_context_(display_context),
  vectorizer_(vectorizer){
}

void AcGsSelectVisitor::visit(OdSiEntity* entity, bool completelyInside) {
  AcGsEntityNode* entity_node = static_cast<AcGsEntityNode*>(entity);

  selected_ = false;

  entity_node->display(*display_context_);

  if (selected_)
    selection_reactor_->selected(path_root_node_, *vectorizer_);
}

void AcGsSelectVisitor::polyline(Adesk::Int32 count, const AcGePoint3d* vertexs) {
  selected_ = true;
}

void AcGsSelectVisitor::triangle(OdUInt32 count, const AcGePoint3d* vertexs) {

}
/*
void AcGsSelectVisitor::shell(OdUInt32 count, const OdGePoint3d* points, OdUInt32 index_count, const OdInt32* indexs) {

}
*/
void AcGsSelectVisitor::setGsMarker(const OdGsMarker& marker) {
  gs_markder_ = marker;
}

//AcGiPathNode
AcGiPathNode::AcGiPathNode() : parent_(nullptr), id_(nullptr), drawable_(nullptr), marker_(-1){
}

const OdGiPathNode* AcGiPathNode::parent() const {
  return parent_;
}

OdDbStub* AcGiPathNode::persistentDrawableId() const {
  return id_;
}

const OdGiDrawable* AcGiPathNode::transientDrawable() const {
  return drawable_;
}

OdGsMarker AcGiPathNode::selectionMarker() const {
  return marker_;
}
