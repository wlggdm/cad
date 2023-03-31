#ifndef _ODA_APP_H_
#define _ODA_APP_H_

#include "DbSystemServices.h"
#include "DbHostAppServices.h"
#include "DbHostAppProgressMeter.h"


class TaskbarHost;

class OdaApp : public OdDbSystemServices,
  public OdDbHostAppServices2,
  public OdDbHostAppProgressMeter
{
public:
  OdaApp();
  ~OdaApp();

  void SetHWND(HWND hwnd);

  void InitialInstance();
  void UnInitialInstance();

  void addRef() {}
  void release() {}
  //OdDbSystemServices
  OdStreamBufPtr createFile(
    const OdString& filename,
    Oda::FileAccessMode accessMode = Oda::kFileRead,
    Oda::FileShareMode  shareMode = Oda::kShareDenyNo,
    Oda::FileCreationDisposition creationDisposition = Oda::kOpenExisting) override;

  bool accessFile(const OdString& filename, int accessMode) override;

  OdInt64 getFileCTime(const OdString& filename) override;

  OdInt64 getFileMTime(const OdString& filename) override;

  OdInt64 getFileSize(const OdString& filename) override;

  OdString formatMessage(unsigned int formatId, va_list* argList = 0) override;

  OdCodePageId systemCodePage() const override;

  void setSystemCodePage(OdCodePageId id);

  //OdDbHostAppServices2
  OdDbHostAppProgressMeter* newProgressMeter() override;

  OdHatchPatternManager* patternManager() override;

  bool ttfFileNameByDescriptor(const OdTtfDescriptor& descr, OdString& fileName) override;

  OdString findFile(const OdString& filename,OdDbBaseDatabase* pDb = 0, FindFileHint hint = kDefault) override;

  //OdDbHostAppProgressMeter
  void start(const OdString& displayString = OdString::kEmpty) override;

  void stop() override;

  void meterProgress() override;

  void setLimit(int max) override;

private:
  OdCodePageId  code_page_id_;

  std::unique_ptr<TaskbarHost> taskbar_host_;

  int progress_max_;

  int progress_current_;

  HWND hwnd_;
};
#endif