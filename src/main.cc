
#include "main_dlg.h"
#include "oda_app.h"

#include "ui/compositor/context_provider.h"
#include "base/at_exit.h"

#include "AcEdContext.h"

CAppModule _Module;

OdaApp oda_app;

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/,LPTSTR lpstrCmdLine, int nCmdShow){
  base::AtExitManager at_exit;
  AcEdContext         ed_context;

  HRESULT hRes = ::CoInitialize(NULL);

  AtlInitCommonControls(ICC_COOL_CLASSES | ICC_BAR_CLASSES);

  hRes = _Module.Init(NULL, hInstance);

  if (!ContextProvider::GetInstance()->initialize())
    return 1;

  oda_app.InitialInstance();
  int nRet = 0;
  {
    CMainDlg dlgMain;
    nRet = dlgMain.DoModal();
  }
  oda_app.UnInitialInstance();

  _Module.Term();
  ::CoUninitialize();
  return nRet;
}
