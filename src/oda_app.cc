#include "oda_app.h"

#include "OdFileBuf.h"

#include <wrl/client.h>
#include <ShObjIdl.h>

#include "base/files/file_path.h"
#include "base/path_service.h"

//come from electron
class TaskbarHost {
public:
  TaskbarHost() {
  }

  virtual ~TaskbarHost() {
  }

  bool SetProgressBar(HWND window, double value, TBPFLAG state) {
    if (!InitializeTaskbar())
      return false;

    bool success;
    if (value > 1.0 || state == TBPF_INDETERMINATE)
      success = SUCCEEDED(taskbar_->SetProgressState(window, TBPF_INDETERMINATE));
    else if (value < 0 || state == TBPF_NOPROGRESS)
      success = SUCCEEDED(taskbar_->SetProgressState(window, TBPF_NOPROGRESS));
    else {
      success = SUCCEEDED(taskbar_->SetProgressState(window, state));

      if (success) {
        int val = static_cast<int>(value * 100);
        success = SUCCEEDED(taskbar_->SetProgressValue(window, val, 100));
      }
    }

    return success;
  }

private:
  bool InitializeTaskbar() {
    if (taskbar_)
      return true;

    if (FAILED(::CoCreateInstance(CLSID_TaskbarList, nullptr,
                                  CLSCTX_INPROC_SERVER,
                                  IID_PPV_ARGS(&taskbar_))) ||
                                  FAILED(taskbar_->HrInit())) {
      taskbar_.Reset();
      return false;
    }

    return true;
  }

  Microsoft::WRL::ComPtr<ITaskbarList3> taskbar_;
};

OdaApp::OdaApp() : taskbar_host_(new TaskbarHost){

}

OdaApp::~OdaApp() {

}

//OdDbSystemServices
OdStreamBufPtr OdaApp::createFile(
  const OdString& filename,
  Oda::FileAccessMode accessMode /*= Oda::kFileRead*/,
  Oda::FileShareMode  shareMode /*= Oda::kShareDenyNo*/,
  Oda::FileCreationDisposition creationDisposition /*= Oda::kOpenExisting*/) {
  OdSmartPtr<OdBaseFileBuf> pFile;

  if (!filename.isEmpty() && filename[0]){
    if ((accessMode & Oda::kFileWrite) != 0)
      pFile = OdWrFileBuf::createObject();
    else
      pFile = OdRdFileBuf::createObject();

    pFile->open(filename, shareMode, accessMode, creationDisposition);
  }
  else
    throw OdError(eNoFileName);

  return OdStreamBufPtr(pFile);
}

bool OdaApp::accessFile(const OdString& filename, int accessMode) {
#if !defined(_WINRT)
  int oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
#endif
  bool res = (_waccess(filename,
    (GETBIT(accessMode, Oda::kFileRead) ? 0x04 : 0x00) |
    (GETBIT(accessMode, Oda::kFileWrite) ? 0x02 : 0x00)) == 0);
#if !defined(_WINRT)
  SetErrorMode(oldErrorMode);
#endif
  if (res){
    struct _stat st = { 0 };

    if (_wstat(filename, &st) == -1)
      return false;

    if (st.st_mode & _S_IFDIR)
      return false;
  }
  return res;
}

OdInt64 OdaApp::getFileCTime(const OdString& filename) {
  struct _stat st;
  if (_wstat(filename, &st)) return -1;
  return st.st_ctime;
}

OdInt64 OdaApp::getFileMTime(const OdString& filename) {
  struct _stat st;
  if (_wstat(filename, &st)) return -1;
  return st.st_mtime;
}

OdInt64 OdaApp::getFileSize(const OdString& filename) {
  WIN32_FIND_DATA fd;
  if (HANDLE hf = ::FindFirstFile(OdString(filename), &fd))
  {
    FindClose(hf);
    return (OdInt64(fd.nFileSizeLow) | (OdInt64(fd.nFileSizeHigh) << 32));
  }
  return OdInt64(-1);
}

OdString OdaApp::formatMessage(unsigned int formatId, va_list* argList/* = 0*/) {
  return L"";
}

OdCodePageId OdaApp::systemCodePage() const {
  return code_page_id_;
}

void OdaApp::setSystemCodePage(OdCodePageId id) {
  code_page_id_ = id;
}

//OdDbHostAppServices2
OdDbHostAppProgressMeter* OdaApp::newProgressMeter() {
  return this;
}

OdHatchPatternManager* OdaApp::patternManager() {
  return nullptr;
}

bool OdaApp::ttfFileNameByDescriptor(const OdTtfDescriptor& descr, OdString& fileName) {
  return false;
}

OdString OdaApp::findFile(const OdString& filename, OdDbBaseDatabase* pDb /*= 0*/, FindFileHint hint /*= kDefault*/) {
  base::FilePath module_path;
  base::PathService::Get(base::DIR_MODULE, &module_path);

  OdString sFile = OdDbHostAppServices2::findFile(filename, pDb, hint);
  if (!sFile.isEmpty())
    return sFile;

  OdString ext = filename.right(4);
  ext.makeUpper();
  if (filename == L"SIMPLEX")
    return module_path.Append(L"simplex.shx").value().c_str();

  if (hint == kFontFile || hint == kCompiledShapeFile || ext == L".SHX")
    return module_path.Append(L"hztxt.shx").value().c_str();

  return L"";
}

//OdDbHostAppProgressMeter
void OdaApp::start(const OdString& displayString /*= OdString::kEmpty*/) {
  progress_current_ = 0;
}

void OdaApp::stop() {
  taskbar_host_->SetProgressBar(hwnd_, (double)progress_current_ / progress_max_, TBPF_NOPROGRESS);
}

void OdaApp::meterProgress() {
  progress_current_ += 1;

  taskbar_host_->SetProgressBar(hwnd_, (double)progress_current_ / progress_max_, TBPF_NORMAL);
}

void OdaApp::setLimit(int max) {
  progress_max_ = max;
}

void OdaApp::InitialInstance() {
  odInitialize(this);
  ::odrxDynamicLinker()->loadModule(OdThreadPoolModuleName);
}

void OdaApp::UnInitialInstance() {
  //odUninitialize();
  taskbar_host_.reset();
}

void OdaApp::SetHWND(HWND hwnd) {
  hwnd_ = hwnd;
}