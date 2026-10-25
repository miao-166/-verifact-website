/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

#include "utils/BaseUtil.h"
#include <UIAutomationCore.h>
#include <UIAutomationCoreApi.h>
#include "uia/StartPageProvider.h"
#include "uia/Constants.h"
#include "uia/Provider.h"

TuYueUIAutomationStartPageProvider::TuYueUIAutomationStartPageProvider(HWND canvasHwnd,
                                                                           TuYueUIAutomationProvider* root)
    : refCount(1), canvasHwnd(canvasHwnd), root(root) {
    // root->AddRef(); Don't add refs to our parent & owner.
}

TuYueUIAutomationStartPageProvider::~TuYueUIAutomationStartPageProvider() = default;

// IUnknown
HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::QueryInterface(REFIID riid, void** ppv) {
    static const QITAB qit[] = {QITABENT(TuYueUIAutomationStartPageProvider, IRawElementProviderSimple),
                                QITABENT(TuYueUIAutomationStartPageProvider, IRawElementProviderFragment),
                                {nullptr}};
    return QISearch(this, qit, riid, ppv);
}

ULONG STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::AddRef() {
    return InterlockedIncrement(&refCount);
}

ULONG STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::Release() {
    LONG res = InterlockedDecrement(&refCount);
    ReportIf(res < 0);
    if (0 == res) {
        delete this;
    }
    return res;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::Navigate(enum NavigateDirection direction,
                                                                         IRawElementProviderFragment** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    *pRetVal = nullptr;
    // no siblings, no children
    if (direction == NavigateDirection_NextSibling || direction == NavigateDirection_PreviousSibling ||
        direction == NavigateDirection_FirstChild || direction == NavigateDirection_LastChild) {
        return S_OK;
    } else if (direction == NavigateDirection_Parent) {
        *pRetVal = root;
        (*pRetVal)->AddRef();
        return S_OK;
    } else {
        return E_INVALIDARG;
    }
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::GetRuntimeId(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    SAFEARRAY* psa = SafeArrayCreateVector(VT_I4, 0, 2);
    if (!psa) {
        return E_OUTOFMEMORY;
    }

    // RuntimeID magic, use hwnd to differentiate providers of different windows
    int rId[] = {(int)canvasHwnd, TUYUE_UIA_STARTPAGE_RUNTIME_ID};
    for (LONG i = 0; i < 2; i++) {
        HRESULT hr = SafeArrayPutElement(psa, &i, (void*)&(rId[i]));
        ReportIf(FAILED(hr));
    }

    *pRetVal = psa;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::GetEmbeddedFragmentRoots(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    // no other roots => return nullptr
    *pRetVal = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::SetFocus() {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::get_BoundingRectangle(struct UiaRect* pRetVal) {
    // share area with the canvas uia provider
    return root->get_BoundingRectangle(pRetVal);
}

HRESULT STDMETHODCALLTYPE
TuYueUIAutomationStartPageProvider::get_FragmentRoot(IRawElementProviderFragmentRoot** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    *pRetVal = root;
    root->AddRef();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::GetPatternProvider(__unused PATTERNID patternId,
                                                                                   IUnknown** pRetVal) {
    *pRetVal = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::GetPropertyValue(PROPERTYID propertyId,
                                                                                 VARIANT* pRetVal) {
    if (propertyId == UIA_NamePropertyId) {
        pRetVal->vt = VT_BSTR;
        pRetVal->bstrVal = SysAllocString(L"Start Page");
        return S_OK;
    }

    pRetVal->vt = VT_EMPTY;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
TuYueUIAutomationStartPageProvider::get_HostRawElementProvider(IRawElementProviderSimple** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    *pRetVal = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationStartPageProvider::get_ProviderOptions(ProviderOptions* pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    *pRetVal = ProviderOptions_ServerSideProvider;
    return S_OK;
}
