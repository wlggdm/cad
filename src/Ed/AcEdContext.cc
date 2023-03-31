#include "AcEdContext.h"

#include <windows.h>

namespace {
  AcEdContext* g_context = nullptr;

  void*     fiber_context = nullptr;

  void WINAPI FiberRoutine(LPVOID lpFiberParameter) {
    AcEdContextSwitch* jig = reinterpret_cast<AcEdContextSwitch*>(lpFiberParameter);

    jig->Run();
  }
}

AcEdImpJig* AcEdContext::JigHost() {
  return g_context->jig_host_;
}

void AcEdContext::SetJigHost(AcEdImpJig* jig) {
  g_context->jig_host_ = jig;
}

AcEdContext::AcEdContext() :
  jig_host_(nullptr) {
  g_context = this;

  fiber_context = ::ConvertThreadToFiber(nullptr);
}

AcEdContext::~AcEdContext() {
  ::ConvertFiberToThread();
}

AcEdContextSwitch::AcEdContextSwitch() {
  context_ = ::CreateFiber(0, FiberRoutine, this);
}

AcEdContextSwitch::~AcEdContextSwitch() {
  if (context_) {
    ::DeleteFiber(context_);
    context_ = nullptr;
  }
}

void AcEdContextSwitch::SwitchToUI() {
  ::SwitchToFiber(fiber_context);
}

void AcEdContextSwitch::SwitchToSelf() {
  ::SwitchToFiber(context_);
}
