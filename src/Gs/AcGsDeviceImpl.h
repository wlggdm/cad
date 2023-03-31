#ifndef _ACGS_DEVICE_IMPL_H_
#define _ACGS_DEVICE_IMPL_H_

#include "acgs.h"
#include <vector>

class AcGsDeviceImpl : public AcGsDevice {
public:
  AcGsDeviceImpl();
  ~AcGsDeviceImpl() override;

  void setWindowSystemID(AcGsWindowingSystemID id);
  AcGsWindowingSystemID windowSystemID() const;
  void setGraphicsKernel(AcGsGraphicsKernel*);

  //AcGsDevice
  AcGsGraphicsKernel & graphicsKernel(void) override;

  void invalidate(void) override;
  void invalidate(const AcGsDCRect &rect) override;

  bool isValid(void) const override;

  void update(AcGsDCRect *pUpdatedRect = nullptr) override;

  void onSize(int width, int height) override;

  void onRealizeForegroundPalette(void) override;
  void onRealizeBackgroundPalette(void) override;
  void onDisplayChange(int nBitsPerPixel, int nXRes, int nYRes) override;

  bool add(AcGsView * view) override;
  bool erase(AcGsView * view) override;
  void eraseAll(void) override;

  bool setBackgroundColor(AcGsColor color) override;
  AcGsColor getBackgroundColor(void) override;

  void setLogicalPalette(const AcGsColor * palette, int nCount) override;
  void setPhysicalPalette(const AcGsColor * palette, int nCount) override;

  void getSnapShot(Atil::Image * pOutput, AcGsDCPoint const & offset) override;
  void setDeviceRenderer(RendererType type) override;

  RendererType getDeviceRenderer(void) override;

  void setRenderInterrupter(AcGsRenderInterrupter* pInterrupter) override;
  AcGsRenderInterrupter* getRenderInterrupter(void) const override;

  void setDisplayUpdateSuppressed(bool bSuppressed) override;

  void invalidate(AcGsModel::RenderType pane) override;
  void invalidate(const AcGsDCRect &rect, AcGsModel::RenderType pane) override;

  void update(AcGsDCRect* pUpdatedRect, GS::SyncBehavior sync) override;
  Atil::Image * createSnapshot(const AcGsDCPoint& offset, const AcGsDCRect& imageDim, GS::ImageDataFormat format, GS::ImageOrientation orientation) override;

  int getMaxDeviceWidth(void) override;
  int getMaxDeviceHeight(void) override;

  void setDesiredFrameRate(float frameRate) override;

  void pauseInteractiveRender(void) override;

  void resumeInteractiveRender(void) override;

  void postRenderProgressCheckingReqeust() override;

  EnableIRResult beginInteractiveRender(AcGsRenderProgressMonitor* pProgressMonitor) override;
  void endInteractiveRender(void) override;

  bool isRendering(void) const override;

  void setFontKerningDisplay(bool bDisplay) override;

private:
  AcGsGraphicsKernel* graphics_kernel_;

  AcGsColor background_color_;

  RendererType  renderer_type_;

  AcGsWindowingSystemID window_id_;

  std::vector<AcGsView*> views_;
};

#endif //_ACGS_DEVICE_IMPL_H_