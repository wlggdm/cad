#include "AcGsLayerNode.h"
#include "AcGiVectorizer.h"
#include "DbLayerTableRecord.h"
#include "DbSymUtl.h"

AcGsLayerNode::AcGsLayerNode() : AcGsNodeImpl(kLayer) {

}

AcGsLayerNode::~AcGsLayerNode() {

}

void AcGsLayerNode::update(AcGiVectorizer& vectorizer) {
  OdDbLayerTableRecordPtr layer_record = object_id_.openObject();
  
  if (layer_traits_.size() < vectorizer.viewId() + 1)
    layer_traits_.resize(vectorizer.viewId() + 1);

  OdGiLayerTraitsData layer_trait;

  layer_trait.setByBlock(OdDbSymUtil::isLayerZeroName(layer_record->getName()));
  layer_trait.setFrozen(layer_record->isFrozen());
  layer_trait.setOff(layer_record->isOff());
  layer_trait.setPlottable(layer_record->isPlottable());
  layer_trait.setLocked(layer_record->isLocked());

  layer_trait.setTrueColor(layer_record->color().entityColor());
  layer_trait.m_plotStyleNameId = layer_record->plotStyleNameId();
  layer_trait.setLinetype(layer_record->linetypeObjectId());
  layer_trait.setLineweight(layer_record->lineWeight());
  layer_trait.setMaterial(layer_record->materialId());
  layer_trait.setTransparency(layer_record->transparency());

  AcDbObjectId viewport_id = vectorizer.viewportObjectId();
  if (!layer_record->worldDraw(nullptr)) {
    OdCmEntityColor col;
    col.setColor(layer_record->color(viewport_id).color());
    layer_trait.setTrueColor(col);
    layer_trait.m_plotStyleNameId = layer_record->plotStyleNameId(viewport_id);
    layer_trait.setLinetype(layer_record->linetypeObjectId(viewport_id));
    layer_trait.setLineweight(layer_record->lineWeight(viewport_id));
    layer_record->setTransparency(layer_record->transparency(viewport_id));
  }

  layer_traits_[vectorizer.viewId()] = layer_trait;
}

const OdGiLayerTraitsData& AcGsLayerNode::layerTraits(OdUInt32 vp) const {
  return layer_traits_[vp];
}
