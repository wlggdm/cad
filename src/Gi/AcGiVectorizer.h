#ifndef _WORLD_DRAW_H_
#define _WORLD_DRAW_H_

#include "GiDrawImpl.h"
#include "GiSubEntityTraitsData.h"
#include "GiDeviation.h"
#include "GiStack.h"
#include "GiLayerTraitsData.h"
#include "GePlane.h"

#include "GeCircArc3d.h"
#include "GeEllipArc3d.h"
#include "GeNurbCurve3d.h"

#include "AcGsRecorderImpl.h"
#include "AcGsEntityRecorder.h"
#include "AcGiMatrix3d.h"
#include "AcGiXTransform.h"
#include "AcGsShellRecorder.h"
#include "AcGsPolylineRecorder.h"

#include "GiGeometrySimplifier.h"
#include "GiTextStyle.h"

#include "AcGsViewImpl.h"

#include <stack>

class AcGiVectorizer : public OdGiWorldDraw_,
                   public OdGiViewportDraw_,
                   public OdGiViewport,
                   public OdGiDeviation,
                   public OdGiConveyorContext,
                   public OdGiSubEntityTraits,
                   public OdGiGeometrySimplifier {
public:
  ODRX_HEAP_OPERATORS();

  AcGiVectorizer();
  ~AcGiVectorizer();

  void setGsView(AcGsView* view) {
    view_ = view;
  }
  AcGsView* gsView() {
    return view_;
  }
  void setViewId(OdUInt32 id) {
    view_id_ = id;
  }
  OdUInt32 viewId() const { return view_id_; }
  void reset() {
    while (!display_object_stack_.empty())
      display_object_stack_.pop();
  }
  AcGsRecorderImpl* recorder() {
    AcGsRecorderImpl* top = display_object_stack_.top();
    while (top) {
      display_object_stack_.pop();
      if (display_object_stack_.top() == nullptr)
        break;
    }

    return top;
  }
  void setContext(OdGiContext* pUserContext);

  //OdGiWorldGeometry
  void setExtents(const OdGePoint3d *newExtents) override;

  //OdGiSubEntityTraits
  void setColor(OdUInt16 color) override;
  void setTrueColor(const OdCmEntityColor& color) override;
  void setLayer(OdDbStub* layerId) override;
  void setLineType(OdDbStub* lineTypeId) override;
  void setSelectionMarker(OdGsMarker selectionMarker) override;
  void setFillType(OdGiFillType fillType) override;
  //void setFillPlane(const OdGeVector3d* pNormal = 0) override;
  void setLineWeight(OdDb::LineWeight lineWeight) override;
  void setLineTypeScale(double lineTypeScale = 1.0) override;
  void setThickness(double thickness) override;
  void setPlotStyleName(OdDb::PlotStyleNameType plotStyleNameType,OdDbStub* plotStyleNameId = 0) override;
  void setMaterial(OdDbStub* materialId) override;
  void setMapper(const OdGiMapper *pMapper) override;
  void setVisualStyle(const OdDbStub* visualStyleId) override;
  void setTransparency(const OdCmTransparency &transparency) override;
  void setDrawFlags(OdUInt32 drawFlags) override;
  void setSelectionGeom(bool bSelectionFlag) override;
  void setShadowFlags(ShadowFlags shadowFlags) override;
  void setSectionable(bool bSectionableFlag) override;
  void setSelectionFlags(SelectionFlags selectionFlags) override;

  OdUInt16 color() const override;
  OdCmEntityColor trueColor() const override;
  OdDbStub* layer() const override;
  OdDbStub* lineType() const override;
  OdGiFillType fillType() const override;
  //bool fillPlane(OdGeVector3d& normal) override;
  OdDb::LineWeight lineWeight() const override;
  double lineTypeScale() const override;
  double thickness() const override;
  OdDb::PlotStyleNameType plotStyleNameType() const override;
  OdDbStub* plotStyleNameId() const override;
  OdDbStub* material() const override;
  const OdGiMapper* mapper() const override;
  OdDbStub* visualStyle() const override;
  OdCmTransparency transparency() const override;
  OdUInt32 drawFlags() const override;
  bool selectionGeom() const override;
  ShadowFlags shadowFlags() const override;
  bool sectionable() const override;
  SelectionFlags selectionFlags() const override;
  void setSecondaryTrueColor(const OdCmEntityColor& color) override;
  OdCmEntityColor secondaryTrueColor() const override;
  void setLineStyleModifiers(const OdGiDgLinetypeModifiers* pLSMod) override;
  const OdGiDgLinetypeModifiers* lineStyleModifiers() const override;
  void setFill(const OdGiFill* pFill) override;
  const OdGiFill* fill() const override;

  //OdGiViewport
  OdGeMatrix3d getModelToEyeTransform() const  override;
  OdGeMatrix3d getEyeToModelTransform() const  override;
  OdGeMatrix3d getWorldToEyeTransform() const  override;
  OdGeMatrix3d getEyeToWorldTransform() const  override;

  bool isPerspective() const  override;
  bool doPerspective(OdGePoint3d& point) const  override;
  bool doInversePerspective(OdGePoint3d& point) const  override;
  void getNumPixelsInUnitSquare(
    const OdGePoint3d& point,
    OdGePoint2d& pixelDensity,
    bool includePerspective = true) const  override;

  OdGePoint3d getCameraLocation() const  override;
  OdGePoint3d getCameraTarget() const  override;
  OdGeVector3d getCameraUpVector() const  override;
  OdGeVector3d viewDir() const  override;
  OdUInt32 viewportId() const  override;
  OdInt16 acadWindowId() const  override;
  void getViewportDcCorners(
    OdGePoint2d& lowerLeft,
    OdGePoint2d& upperRight) const  override;
  bool getFrontAndBackClipValues(
    bool& clipFront,
    bool& clipBack,
    double& front,
    double& back) const  override;
  double linetypeScaleMultiplier() const  override;
  double linetypeGenerationCriteria() const  override;
  bool layerVisible( OdDbStub* layerId) const  override;

  const OdGiContextualColors *contextualColors() const override;

  //OdGiCommonDraw
  OdGiRegenType regenType() const override;
  bool regenAbort() const override;
  OdGiSubEntityTraits& subEntityTraits() const override;

  OdGiGeometry& rawGeometry() const override;
  bool isDragging() const override;
  double deviation(const OdGiDeviationType deviationType, const OdGePoint3d& pointOnCurve) const override;
  OdUInt32 numberOfIsolines() const override;
  OdGiContext* context() const override;
  const OdGiPathNode* currentGiPath() const override;

  //OdGiConveyorContext
  OdGiContext& giContext() const override { return *gi_context; }
  void setEffectiveTraits(const OdGiSubEntityTraitsData& traits,
    const OdGeVector3d* fillNormal = NULL);
  const OdGiDrawableDesc* currentDrawableDesc() const override { return nullptr; }
  const OdGiDrawable* currentDrawable() const override { return nullptr;  }
  const OdGiViewport* giViewport() const override { return this; }
  const OdGsView* gsView() const override { return nullptr;  }
  const OdGiDeviation& worldDeviation() const override { return *this; }
  const OdGiDeviation& modelDeviation() const override { return *this; }
  const OdGiDeviation& eyeDeviation() const override { return *this; }

  //OdGiViewportDraw
  OdGiViewport& viewport() const override;
  OdUInt32 sequenceNumber() const override;
  bool isValidId(const OdUInt32 viewportId) const override;
  OdDbStub* viewportObjectId() const override;

  //OdGiViewportGeometry
  void polylineEye(OdUInt32 numVertices,const OdGePoint3d* vertexList) override;
  void polygonEye(OdUInt32 numVertices,const OdGePoint3d* vertexList) override;
  void polylineDc(OdUInt32 numVertices,const OdGePoint3d* vertexList) override;
  void polygonDc( OdUInt32 numVertices,const OdGePoint3d* vertexList) override;

  void rasterImageDc(
    const OdGePoint3d& origin,
    const OdGeVector3d& u,
    const OdGeVector3d& v,
    const OdGiRasterImage* pImage,
    const OdGePoint2d* uvBoundary,
    OdUInt32 numBoundPts,
    bool transparency = false,
    double brightness = 50.0,
    double contrast = 50.0,
    double fade = 0.0) override;

  void  metafileDc(
    const OdGePoint3d& origin,
    const OdGeVector3d& u,
    const OdGeVector3d& v,
    const OdGiMetafile* pMetafile,
    bool dcAligned = true,
    bool allowClipping = false) override;

  void ownerDrawDc(
    const OdGePoint3d& origin,
    const OdGeVector3d& u,
    const OdGeVector3d& v,
    const OdGiSelfGdiDrawable* pDrawable,
    bool dcAligned = true,
    bool allowClipping = false) override;

  //OdGiGeometry
  OdGeMatrix3d getModelToWorldTransform() const override;

  OdGeMatrix3d getWorldToModelTransform() const override;

  void pushModelTransform(const OdGeVector3d& normal) override;

  void pushModelTransform(const OdGeMatrix3d& xfm) override;

  void popModelTransform() override;

  void circle(
    const OdGePoint3d& center,
    double radius,
    const OdGeVector3d& normal) override;

  void circle(
    const OdGePoint3d& firstPoint,
    const OdGePoint3d& secondPoint,
    const OdGePoint3d& thirdPoint) override;

  void circularArc(
    const OdGePoint3d& center,
    double radius,
    const OdGeVector3d& normal,
    const OdGeVector3d& startVector,
    double sweepAngle,
    OdGiArcType arcType = kOdGiArcSimple) override;

  void circularArc(
    const OdGePoint3d& firstPoint,
    const OdGePoint3d& secondPoint,
    const OdGePoint3d& thirdPoint,
    OdGiArcType arcType = kOdGiArcSimple) override;

  void polyline(
    OdInt32 numVertices,
    const OdGePoint3d* vertexList,
    const OdGeVector3d* pNormal = 0,
    OdGsMarker baseSubEntMarker = -1) override;

  void polygon(OdInt32 numVertices, const OdGePoint3d* vertexList) override;

  void pline(
    const OdGiPolyline& polyline,
    OdUInt32 fromIndex = 0,
    OdUInt32 numSegs = 0) override;

  void mesh(
    OdInt32 numRows,
    OdInt32 numColumns,
    const OdGePoint3d* vertexList,
    const OdGiEdgeData* pEdgeData = 0,
    const OdGiFaceData* pFaceData = 0,
    const OdGiVertexData* pVertexData = 0) override;

  void shell(
    OdInt32 numVertices,
    const OdGePoint3d* vertexList,
    OdInt32 faceListSize,
    const OdInt32* faceList,
    const OdGiEdgeData* pEdgeData = 0,
    const OdGiFaceData* pFaceData = 0,
    const OdGiVertexData* pVertexData = 0) override;

  void text(
    const OdGePoint3d& position,
    const OdGeVector3d& normal,
    const OdGeVector3d& direction,
    double height,
    double width,
    double oblique,
    const OdString& msg) override;

  void text(
    const OdGePoint3d& position,
    const OdGeVector3d& normal,
    const OdGeVector3d& direction,
    const OdChar* msg,
    OdInt32 length,
    bool raw,
    const OdGiTextStyle* pTextStyle) override;

  void xline(
    const OdGePoint3d& firstPoint,
    const OdGePoint3d& secondPoint) override;

  void ray(const OdGePoint3d& basePoint, const OdGePoint3d& throughPoint) override;

  void nurbs(const OdGeNurbCurve3d& nurbsCurve) override;

  void ellipArc(
    const OdGeEllipArc3d& ellipArc,
    const OdGePoint3d* endPointsOverrides = 0,
    OdGiArcType arcType = kOdGiArcSimple) override;

  void draw(const OdGiDrawable* pDrawable) override;

  void pushClipBoundary(OdGiClipBoundary* pBoundary) override;

  void popClipBoundary() override;

  void worldLine(const OdGePoint3d points[2]) override;

  void image(
    const OdGiImageBGRA32& img,
    const OdGePoint3d& origin,
    const OdGeVector3d& uVec,
    const OdGeVector3d& vVec,
    OdGiRasterImage::TransparencyMode trpMode = OdGiRasterImage::kTransparency8Bit) override;

  void edge(const OdGiEdge2dArray& edges) override;

  void pushClipBoundary(OdGiClipBoundary* pBoundary, OdGiAbstractClipBoundary* pClipInfo) override;

  void polypoint(
    OdInt32 numPoints,
    const OdGePoint3d* vertexList,
    const OdCmEntityColor* pColors,
    const OdCmTransparency* pTransparency,
    const OdGeVector3d* pNormals = NULL,
    const OdGsMarker* pSubEntMarkers = NULL,
    OdInt32 nPointSize = 0) override;

  void rowOfDots(OdInt32 numPoints, const OdGePoint3d& startPoint, const OdGeVector3d& dirToNextPoint) override;

  AcGsEntityRecorder* internalDraw(const OdGiDrawable* pDrawable, OdUInt32 drawable_flags = 0);
private:
  enum ChangedTraits{
    kFirstChangedFlag = 1,
    kColorChanged = kFirstChangedFlag << 0,
    kLayerChanged = kFirstChangedFlag << 1,
    kLineTypeChanged = kFirstChangedFlag << 2,
    kFillTypeChanged = kFirstChangedFlag << 3,
    kLineWeightChanged = kFirstChangedFlag << 4,
    kLineTypeScaleChanged = kFirstChangedFlag << 5,
    kThicknessChanged = kFirstChangedFlag << 6,
    kPlotStyleChanged = kFirstChangedFlag << 7,
    kMaterialChanged = kFirstChangedFlag << 8,
    kMapperChanged = kFirstChangedFlag << 9,
    kVisualStyleChanged = kFirstChangedFlag << 10,
    kTransparencyChanged = kFirstChangedFlag << 11,
    kDrawFlagsChanged = kFirstChangedFlag << 12,
    kSelectionGeomChanged = kFirstChangedFlag << 13,
    kShadowFlagsChanged = kFirstChangedFlag << 14,
    kSectionableChanged = kFirstChangedFlag << 15,
    kSelectionFlagsChanged = kFirstChangedFlag << 16,
    kSecColorChanged = kFirstChangedFlag << 17,
    kLSModifiersChanged = kFirstChangedFlag << 18,
    kFillChanged = kFirstChangedFlag << 19,
    kAwareFlagChanged = kFirstChangedFlag << 20,
    kAllChanged = 0x1FFFFF,
    kSomeChanged = kAllChanged
  };

  void setEntityTraitChanged() { entity_trait_changed_ = kAllChanged; }
  void setEntityTraitChanged(ChangedTraits bit, bool value = true) {
    SETBIT(entity_trait_changed_, bit, value);
  }
  void clearEntityTraitChanged() const { entity_trait_changed_ = 0; }
  bool isEntityTraitChanged(int bit = kSomeChanged) const {
    return GETBIT(entity_trait_changed_, bit);
  }

  void affectTraits(const OdGiSubEntityTraitsData* pFrom, OdGiSubEntityTraitsData& to) const;

  bool effectivelyVisible() const;

  void onTraitsModified();

  void updateLayerTraits(OdGiSubEntityTraitsData& traits) const;

  const OdGiSubEntityTraitsData& effectiveTraits() const;
  bool needDraw(OdUInt32 drawable_flag);
  OdUInt32 setAttributes(const OdGiDrawable* pDrawable);

  //geometry
  const OdGeVector3d* extrusion(const OdGePoint3d& firstPoint, const OdGePoint3d& secondPoint, const OdGePoint3d& thirdPoint);
  const OdGeVector3d* extrusion(const OdGeVector3d* pNormal);
  const OdGeVector3d* extrusion(const OdGeVector3d& normal);

  void polylineOut(OdInt32 numPoints, const OdGePoint3d* vertexList) override;
  void polylineOut(OdInt32 numPoints, const OdInt32* vertexIndexList) override;
  void triangleOut(const OdInt32* p3Vertices, const OdGeVector3d* pNormal) override;

  void generateShellFaces(OdInt32 faceListSize,
    const OdInt32* faceList,
    const OdGiEdgeData* pEdgeData = 0,
    const OdGiFaceData* pFaceData = 0) override;

  void onSetVertexData() override;
private:
  const OdGiSubEntityTraitsData* block_trait_data_;

  OdGiSubEntityTraitsData entity_trait_data_;

  mutable OdGiSubEntityTraitsData effect_trait_data_;

  mutable OdGiLayerTraitsData  effect_layer_data_;

  mutable OdUInt32 entity_trait_changed_;

  OdGiStack<OdGeMatrix3d> model_transform_;
  OdGeMatrix3d model_view_transform_;
  //OdGiStack<OdGiMatrix3d> model_transform_;

  OdGePlane           primitive_plane_;

  //²ÉÑùµã
  OdGeVector3d        extrusion_;

  OdInt32Array        out_facelist_;

  OdUInt32            drawable_attribute_;

  AcGsView*           view_;

  double deviation_[5];

  OdGiTextStyle m_textStyle;
  //gs
  void pushDisplayObject(AcGsRecorderImpl* display_object) {
    display_object_stack_.push(display_object);
  }

  AcGsRecorderImpl* popDisplayObject() {
    AcGsRecorderImpl* display_object = display_object_stack_.top();
    display_object_stack_.pop();
    return display_object;
  }

  AcGsRecorderImpl* getDisplayObject() {
    AcGsRecorderImpl* display_object = display_object_stack_.top();
    return display_object;
  }
  
  std::stack<AcGsRecorderImpl*> display_object_stack_;

  OdGiContext* gi_context;

  AcGiXform     xform;
  public:
    std::unique_ptr<AcGsRecorderImpl> display_objects_;

    OdUInt32 view_id_;

    AcGsShellRecorder* shell_recorder_;

    bool generate_wires_;

    bool generate_shell_;
};
#endif