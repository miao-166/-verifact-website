/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

#include "utils/BaseUtil.h"
#include <UIAutomationCore.h>
#include <UIAutomationCoreApi.h>
#include <OleAcc.h>
#ifdef __GNUC__
// mingw needs explicit UUID declaration for IAccIdentity
__CRT_UUID_DECL(IAccIdentity, 0x7852B78D, 0x1CFD, 0x41C1, 0xA6, 0x15, 0x9C, 0x0C, 0x85, 0x96, 0x0B, 0x5F)
#endif
#include "utils/ScopedWin.h"
#include "utils/WinUtil.h"

#include "wingui/UIModels.h"

#include "Settings.h"
#include "DocController.h"
#include "EngineBase.h"
#include "EngineAll.h"
#include "DisplayModel.h"
#include "utils/FileUtil.h"
#include "uia/DocumentProvider.h"
#include "uia/Constants.h"
#include "uia/PageProvider.h"
#include "uia/Provider.h"
#include "uia/TextRange.h"

TuYueUIAutomationDocumentProvider::TuYueUIAutomationDocumentProvider(HWND canvasHwnd,
                                                                         TuYueUIAutomationProvider* root)
    : refCount(1),
      canvasHwnd(canvasHwnd),
      root(root),
      released(true),
      child_first(nullptr),
      child_last(nullptr),
      dm(nullptr) {
    // root->AddRef(); Don't add refs to our parent & owner.
}

TuYueUIAutomationDocumentProvider::~TuYueUIAutomationDocumentProvider() {
    this->FreeDocument();
}

void TuYueUIAutomationDocumentProvider::LoadDocument(DisplayModel* newDm) {
    this->FreeDocument();

    // no mutexes needed, this function is called from thread that created dm

    // create page element for each page
    TuYueUIAutomationPageProvider* prevPage = nullptr;
    for (int i = 1; i <= newDm->PageCount(); ++i) {
        TuYueUIAutomationPageProvider* currentPage = new TuYueUIAutomationPageProvider(i, canvasHwnd, newDm, this);
        currentPage->sibling_prev = prevPage;
        if (prevPage) {
            prevPage->sibling_next = currentPage;
        }
        prevPage = currentPage;

        if (i == 1) {
            child_first = currentPage;
        }
    }
    child_last = prevPage;

    dm = newDm;
    released = false;
}

void TuYueUIAutomationDocumentProvider::FreeDocument() {
    // release our refs to the page elements
    if (released) {
        return;
    }

    released = true;
    dm = nullptr;

    TuYueUIAutomationPageProvider* it = child_first;
    while (it) {
        TuYueUIAutomationPageProvider* current = it;
        it = it->sibling_next;

        current->released = true; // disallow DisplayModel access
        current->Release();
    }

    // we have released our refs from these objects
    // we are not allowed to access them anymore
    child_first = nullptr;
    child_last = nullptr;
}

bool TuYueUIAutomationDocumentProvider::IsDocumentLoaded() const {
    return !released;
}

DisplayModel* TuYueUIAutomationDocumentProvider::GetDM() {
    ReportIf(!IsDocumentLoaded());
    ReportIf(!dm);
    return dm;
}

TuYueUIAutomationPageProvider* TuYueUIAutomationDocumentProvider::GetFirstPage() {
    ReportIf(!IsDocumentLoaded());
    return child_first;
}

TuYueUIAutomationPageProvider* TuYueUIAutomationDocumentProvider::GetLastPage() {
    ReportIf(!IsDocumentLoaded());
    return child_last;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::QueryInterface(REFIID riid, void** ppv) {
    static const QITAB qit[] = {QITABENT(TuYueUIAutomationDocumentProvider, IRawElementProviderSimple),
                                QITABENT(TuYueUIAutomationDocumentProvider, IRawElementProviderFragment),
                                QITABENT(TuYueUIAutomationDocumentProvider, ITextProvider),
                                QITABENT(TuYueUIAutomationDocumentProvider, IAccIdentity),
                                {nullptr}};
    return QISearch(this, qit, riid, ppv);
}

ULONG STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::AddRef() {
    return InterlockedIncrement(&refCount);
}

ULONG STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::Release() {
    LONG res = InterlockedDecrement(&refCount);
    ReportIf(res < 0);
    if (0 == res) {
        delete this;
    }
    return res;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::Navigate(enum NavigateDirection direction,
                                                                        IRawElementProviderFragment** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    if (direction == NavigateDirection_NextSibling || direction == NavigateDirection_PreviousSibling) {
        *pRetVal = nullptr;
        return S_OK;
    } else if (direction == NavigateDirection_FirstChild || direction == NavigateDirection_LastChild) {
        // don't allow traversion to enter invalid nodes
        if (released) {
            *pRetVal = nullptr;
            return S_OK;
        }

        if (direction == NavigateDirection_FirstChild) {
            *pRetVal = child_first;
        } else {
            *pRetVal = child_last;
        }
        (*pRetVal)->AddRef();
        return S_OK;
    } else if (direction == NavigateDirection_Parent) {
        *pRetVal = root;
        (*pRetVal)->AddRef();
        return S_OK;
    } else {
        return E_INVALIDARG;
    }
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::GetRuntimeId(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    SAFEARRAY* psa = SafeArrayCreateVector(VT_I4, 0, 2);
    if (!psa) {
        return E_OUTOFMEMORY;
    }

    // RuntimeID magic, use hwnd to differentiate providers of different windows
    LONG rId[] = {(LONG)canvasHwnd, TUYUE_UIA_DOCUMENT_RUNTIME_ID};
    for (LONG i = 0; i < 2; i++) {
        HRESULT hr = SafeArrayPutElement(psa, &i, (void*)&(rId[i]));
        ReportIf(FAILED(hr));
    }

    *pRetVal = psa;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::GetEmbeddedFragmentRoots(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    // no other roots => return nullptr
    *pRetVal = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::SetFocus() {
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::get_BoundingRectangle(struct UiaRect* pRetVal) {
    // share area with the canvas uia provider
    return root->get_BoundingRectangle(pRetVal);
}

HRESULT STDMETHODCALLTYPE
TuYueUIAutomationDocumentProvider::get_FragmentRoot(IRawElementProviderFragmentRoot** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    // return the root node
    *pRetVal = root;
    root->AddRef();
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::GetPatternProvider(PATTERNID patternId,
                                                                                  IUnknown** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }

    if (patternId == UIA_TextPatternId) {
        *pRetVal = static_cast<ITextProvider*>(this);
        AddRef();
        return S_OK;
    }

    *pRetVal = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::GetPropertyValue(PROPERTYID propertyId,
                                                                                VARIANT* pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    if (released) {
        return E_FAIL;
    }

    if (propertyId == UIA_NamePropertyId) {
        // typically filename
        pRetVal->vt = VT_BSTR;
        TempStr s = path::GetBaseNameTemp(dm->GetEngine()->FilePath());
        pRetVal->bstrVal = SysAllocString(ToWStrTemp(s));
        return S_OK;
    } else if (propertyId == UIA_IsTextPatternAvailablePropertyId) {
        pRetVal->vt = VT_BOOL;
        pRetVal->boolVal = TRUE;
        return S_OK;
    } else if (propertyId == UIA_ControlTypePropertyId) {
        pRetVal->vt = VT_I4;
        pRetVal->lVal = UIA_DocumentControlTypeId;
        return S_OK;
    } else if (propertyId == UIA_IsContentElementPropertyId || propertyId == UIA_IsControlElementPropertyId) {
        pRetVal->vt = VT_BOOL;
        pRetVal->boolVal = TRUE;
        return S_OK;
    } else if (propertyId == UIA_NativeWindowHandlePropertyId) {
        pRetVal->vt = VT_I4;
        pRetVal->lVal = 0;
        return S_OK;
    } else if (propertyId == UIA_AutomationIdPropertyId) {
        pRetVal->vt = VT_BSTR;
        pRetVal->bstrVal = SysAllocString(L"Document");
        return S_OK;
    }

    pRetVal->vt = VT_EMPTY;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
TuYueUIAutomationDocumentProvider::get_HostRawElementProvider(IRawElementProviderSimple** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    *pRetVal = nullptr;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::get_ProviderOptions(ProviderOptions* pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    *pRetVal = ProviderOptions_ServerSideProvider;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::GetSelection(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    if (released) {
        return E_FAIL;
    }

    SAFEARRAY* psa = SafeArrayCreateVector(VT_UNKNOWN, 0, 1);
    if (!psa) {
        return E_OUTOFMEMORY;
    }

    // TODO: this selection is leaked - why?
    TuYueUIAutomationTextRange* selection = new TuYueUIAutomationTextRange(this, dm->textSelection);

    LONG index = 0;
    HRESULT hr = SafeArrayPutElement(psa, &index, selection);
    ReportIf(FAILED(hr));
    // the array now owns the selection
    selection->Release();

    *pRetVal = psa;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::GetVisibleRanges(SAFEARRAY** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    if (released) {
        return E_FAIL;
    }

    // return all pages' ranges that are even partially visible
    Vec<TuYueUIAutomationTextRange*> rangeArray;
    TuYueUIAutomationPageProvider* it = child_first;
    while (it && rangeArray.size() < (ULONG_MAX / 2)) {
        if (it->dm->GetPageInfo(it->pageNum) && it->dm->GetPageInfo(it->pageNum)->shown &&
            it->dm->GetPageInfo(it->pageNum)->visibleRatio > 0.0f) {
            rangeArray.Append(new TuYueUIAutomationTextRange(this, it->pageNum));
        }
        it = it->sibling_next;
    }
    ReportIf((ULONG_MAX / 2) == rangeArray.size());

    SAFEARRAY* psa = SafeArrayCreateVector(VT_UNKNOWN, 0, (ULONG)rangeArray.size());
    if (!psa) {
        for (size_t i = 0; i < rangeArray.size(); i++) {
            rangeArray[i]->Release();
        }
        return E_OUTOFMEMORY;
    }

    for (LONG i = 0; i < (LONG)rangeArray.size(); i++) {
        HRESULT hr = SafeArrayPutElement(psa, &i, rangeArray[i]);
        ReportIf(FAILED(hr));
        rangeArray[i]->Release();
    }

    *pRetVal = psa;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::RangeFromChild(IRawElementProviderSimple* childElement,
                                                                              ITextRangeProvider** pRetVal) {
    if (pRetVal == nullptr || childElement == nullptr) {
        return E_POINTER;
    }
    if (released) {
        return E_FAIL;
    }

    // get page range
    // TODO: is childElement guaranteed to be a TuYueUIAutomationPageProvider?
    *pRetVal = new TuYueUIAutomationTextRange(this, ((TuYueUIAutomationPageProvider*)childElement)->pageNum);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::RangeFromPoint(__unused struct UiaPoint point,
                                                                              __unused ITextRangeProvider** pRetVal) {
    // TODO: Is this even used? We wont support editing either way
    // so there won't be even a caret visible. Hence empty ranges are useless?
    return E_NOTIMPL;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::get_DocumentRange(ITextRangeProvider** pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    if (released) {
        return E_FAIL;
    }

    TuYueUIAutomationTextRange* documentRange = new TuYueUIAutomationTextRange(this);
    documentRange->SetToDocumentRange();

    *pRetVal = documentRange;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE
TuYueUIAutomationDocumentProvider::get_SupportedTextSelection(enum SupportedTextSelection* pRetVal) {
    if (pRetVal == nullptr) {
        return E_POINTER;
    }
    *pRetVal = SupportedTextSelection_Single;
    return S_OK;
}

HRESULT STDMETHODCALLTYPE TuYueUIAutomationDocumentProvider::GetIdentityString(DWORD dwIDChild, BYTE** ppIDString,
                                                                                 DWORD* pdwIDStringLen) {
    if (ppIDString == nullptr || pdwIDStringLen == nullptr) {
        return E_POINTER;
    }
    if (released) {
        return E_FAIL;
    }

    for (TuYueUIAutomationPageProvider* it = child_first; it; it = it->sibling_next) {
        if (it->pageNum == (int)dwIDChild + 1) {
            // Use memory address as identification. Use 8 bytes just in case
            *ppIDString = (BYTE*)CoTaskMemAlloc(8);
            if (!(*ppIDString)) {
                return E_OUTOFMEMORY;
            }

            memset(*ppIDString, 0, 8);
            memcpy(*ppIDString, &it, sizeof(void*)); // copy the pointer to the allocated array
            *pdwIDStringLen = 8;
            return S_OK;
        }
    }

    return E_FAIL;
}
