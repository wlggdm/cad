#ifndef _ACGS_VIEW_IMPL_H_
#define _ACGS_VIEW_IMPL_H_

#include "acgs.h"
#include "GePoint2dArray.h"
#include "GeExtents3d.h"
#include "UInt8Array.h"
#include "IntArray.h"

#include "GiLayerTraitsData.h"
#include <vector>
#include <map>

class OdGsSelectionReactor;

class AcGsViewImpl : public AcGsView {
public:
  AcGsViewImpl();
  ~AcGsViewImpl() override;

  void setDevice(AcGsDevice*);

  void setGraphicsKernel(AcGsGraphicsKernel* graphics_kernel);
  AcGsGraphicsKernel & graphicsKernel(void) override;

  void setViewport(const AcGePoint2d & lowerLeft, const AcGePoint2d & upperRight) override;
  void setViewport(const AcGsDCRect & screen_rect) override;
  void getViewport(AcGePoint2d & lowerLeft, AcGePoint2d & upperRight) const override;
  void getViewport(AcGsDCRect & screen_rect) const override;

  void setViewportClipRegion(int contours, int const * counts, AcGsDCPoint const * points) override;
  void removeViewportClipRegion(void) override;

  void setViewportBorderProperties(AcGsColor const & color, int weight) override;
  void getViewportBorderProperties(AcGsColor & color, int & weight) const override;

  void setViewportBorderVisibility(bool bVisible) override;
  bool isViewportBorderVisible(void) const override;

  void setView(const AcGePoint3d & position, const AcGePoint3d & target, const AcGeVector3d& upVector, double fieldWidth, double fieldHeight, Projection projection = kParallel) override;
  AcGePoint3d position(void) const override;
  AcGePoint3d target(void) const override;
  AcGeVector3d upVector(void) const override;

  bool isPerspective(void) const override;

  double fieldWidth(void) const override;
  double fieldHeight(void) const override;

  void setEnableFrontClip(bool enable) override;
  bool isFrontClipped(void) const override;

  void setFrontClip(double distance) override;
  double frontClip(void) const override;

  void setEnableBackClip(bool enable) override;
  bool isBackClipped(void) const override;

  void setBackClip(double distance) override;
  double backClip(void) const override;

  AcGeMatrix3d viewingMatrix(void) const override;
  AcGeMatrix3d projectionMatrix(void) const override;
  AcGeMatrix3d screenMatrix(void) const override;
  AcGeMatrix3d worldToDeviceMatrix(void) const override;
  AcGeMatrix3d objectToDeviceMatrix(void) const override;

  bool add(AcGiDrawable * drawable, AcGsModel * model) override;
  bool erase(AcGiDrawable * drawable) override;
  void eraseAll(void) override;

  void invalidate(void) override;
  void invalidate(const AcGsDCRect &rect) override;

  bool isValid(void) const override;

  void update(void) override;

  void beginInteractivity(double fFrameRateInHz) override;
  void endInteractivity(void) override;
  bool isInteractive(void) const override;

  void flush(void) override;

  void setMaximumUpgrade(int step) override;
  int  maximumUpgrade(void) override;

  void setMinimumDegrade(int step) override;
  int  minimumDegrade(void) override;

  void hide(void) override;
  void show(void) override;
  bool isVisible(void) override;

  void freezeLayer(Adesk::IntDbId layerID) override;
  void thawLayer(Adesk::IntDbId layerID) override;
  void clearFrozenLayers(void) override;

  void invalidateCachedViewportGeometry(void) override;
  void invalidateCachedViewportGeometry(AcGiDrawable* pDrawable) override;

  void dolly(const AcGeVector3d & vector) override;
  void dolly(double x, double y, double z) override;
  void roll(double angle) override;
  void orbit(double x, double y) override;
  void zoom(double factor) override;
  void zoomExtents(const AcGePoint3d& minPoint, const AcGePoint3d& maxPoint) override;
  void zoomWindow(const AcGePoint2d& lowerLeft, const AcGePoint2d& upperRight) override;
  void pan(double x, double y) override;

  bool pointInView(const AcGePoint3d& pnt) override;
  bool extentsInView(const AcGePoint3d& minPoint, const AcGePoint3d& maxPoint) override;

  AcGsView * clone(bool bCloneViewParameters = true, bool bCloneGeometry = false) override;
  bool exceededBounds(void) override;

  void enableStereo(bool bEnable) override;
  bool isStereoEnabled(void) const override;

  void setStereoParameters(double magnitude, double parallax) override;
  void getStereoParameters(double & magnitude, double & parallax) const override;

  void getSnapShot(Atil::Image * pOutput, AcGsDCPoint const & offset) override;
  bool renderToImage(Atil::Image* pOutput, AcGiDrawable* pSettings, AcGsRenderProgressMonitor* pProgressMonitor, const AcGsDCRect & rectScreen, bool bReuseScene = false) override;

  AcGsDevice * getDevice(void) const override;

  void setVisualStyle(const AcDbObjectId visualStyleId) override;
  AcDbObjectId visualStyle(void) const override;

  void setVisualStyle(const AcGiVisualStyle &visualStyle) override;
  bool visualStyle(AcGiVisualStyle &visualStyle) const override;

  void setBackground(const AcDbObjectId backgroundId) override;
  AcDbObjectId background(void) const override;

  void enableDefaultLighting(bool bEnable, DefaultLightingType type = kTwoLights) override;
  void getNumPixelsInUnitSquare(const AcGePoint3d& givenWorldpt, AcGePoint2d& pixelArea, bool includePerspective = true) const override;

  AcGsModel* getModel(AcGiDrawable*) const override;
  void getModelList(AcArray<AcGsModel *>&) const override;

  const AcGsClientViewInfo& getClientViewInfo() const override;
  void update(GS::SyncBehavior sync) override;

  bool isPointInViewport(const Adesk::Int32 x, const Adesk::Int32 y) override;

  AcGsView * clone(bool bCloneViewParameters, ModelCloneOption cloneOption) override;

  Atil::Image * createSnapshot(const AcGsDCPoint& offset, const AcGsDCRect& imageDim, GS::ImageDataFormat format, GS::ImageOrientation orientation, bool bOnlyCaptureRenderedImage = false) override;

  EnableIRResult beginInteractiveRender(AcGsRenderProgressMonitor* pProgressMonitor) override;
  void endInteractiveRender(void) override;

  bool isRendering(void) const override;

  void beginFastInteractivity(void) override;
  bool endFastInteractivity(void) override;
  bool isFastInteractivity(void) override;

public:
  void setClientViewInfo(const AcGsClientViewInfo& client_view_info);
  void updateLayer(OdDbStub* layer_table);
  OdGiLayerTraitsData* layerNode(OdDbStub* layer_id);

  const OdGeVector3d& eyeVector() const { return eye_vector_; }
  const OdGeVector3d& xVector() const { return x_vector_; }
  double focalLength() const;
  AcGeMatrix3d eyeToScreenMatrix() const;
  const AcGeMatrix3d& eyeToWorldMatrix() const;
  double windowAspect() const;

  void select(const AcGsDCPoint* dcpts, int num_points, OdGsSelectionReactor* reactor, AcGsView::SelectionMode mode);
private:
  void onWorldToEyeChanged();

  double lensLength() const;
  
  const AcGsDCPoint& dcScreenMin() const  { return screen_min_; }
  const AcGsDCPoint& dcScreenMax() const  { return screen_max_; }
  const OdGePoint2d& dcLowerLeft() const  { return lower_left_; }
  const OdGePoint2d& dcUpperRight() const { return upper_right_; }

  AcGeMatrix3d perspectiveMatrix() const;

  void setPerspectiveEnabled(bool bValue) const {
    SETBIT(view_flags_, kPerspectiveEnabled, bValue);
  }

  void screenRect(AcGsDCPoint &min, AcGsDCPoint &max) const;
  void screenRect(OdGePoint2d &ll, OdGePoint2d &ur) const;

  bool doPerspectivePt(OdGePoint3d& pointToTransform) const;

  enum GsViewFlags{
    kFirstViewImplFlag = 1,

    kInvalid = kFirstViewImplFlag << 0, // device surface entirely invalid
    kCheckValid = kFirstViewImplFlag << 1, // device surface entirely valid
    kBorderVisible = kFirstViewImplFlag << 2,
    kDoFrontClip = kFirstViewImplFlag << 3,
    kDoBackClip = kFirstViewImplFlag << 4,
    kPerspectiveEnabled = kFirstViewImplFlag << 5,
    kHidden = kFirstViewImplFlag << 6, // view is hidden
    kTransparentClearColor = kFirstViewImplFlag << 7,
    kDevBgClearColor = kFirstViewImplFlag << 8,
    kEyeToWorldValid = kFirstViewImplFlag << 9,
    kWorldToEyeValid = kFirstViewImplFlag << 10,
    kWorldToDeviceValid = kFirstViewImplFlag << 11,
    kSnapping = kFirstViewImplFlag << 12,
    kForbidDrawableRegenDraw = kFirstViewImplFlag << 13,
    kSceneDeptInvalid = kFirstViewImplFlag << 14,
    kDepViewInside = kFirstViewImplFlag << 15,
    kLegacyWireframe = kFirstViewImplFlag << 16,
    kLegacyHidden = kFirstViewImplFlag << 17,
    kPlotTransparency = kFirstViewImplFlag << 18,
    kAccumulateVpChanges = kFirstViewImplFlag << 19
  };
private:
  AcGsGraphicsKernel*   graphics_kernel_;

  AcGsClientViewInfo    client_view_info_;

  AcGsDevice*           device_;

  bool                  rendering_;

  mutable OdUInt32      view_flags_;

  AcGePoint3d           position_;
  AcGePoint3d           target_;
  AcGeVector3d          up_vector_;
  double                field_width_;
  double                field_height_;
  Projection            projection_;

  double                lens_length_;

  OdGeVector3d          eye_vector_;
  double                eye_length_;
  OdGeVector3d          x_vector_;

  double                front_dist_;
  double                back_dist_;

  OdGePoint2d           lower_left_;
  OdGePoint2d           upper_right_;

  AcGsDCPoint           screen_min_;
  AcGsDCPoint           screen_max_;

  OdIntArray            clip_counts_;
  OdGePoint2dArray      clip_points_;

  AcGsColor             border_color_;
  int                   border_weight_;

  OdGeExtents3d         extents_;

  mutable AcGeMatrix3d  world_device_;
  mutable AcGeMatrix3d  world_eye_;
  mutable AcGeMatrix3d  eye_world_;

  bool                  fast_interactivity_;

  struct DrawableHolder {
    DrawableHolder(OdGiDrawable*, AcGsModel*);

    OdDbStub*       drawable_id;
    OdGiDrawablePtr drawable;
    AcGsModel*      model;
  };
  OdGiDrawablePtr drawableAt(DrawableHolder& hodler);

  std::vector<DrawableHolder> drawable_holders_;

  std::map<OdDbStub*, OdGiLayerTraitsData> layer_map_;
};
#endif //_ACGS_VIEW_IMPL_H_