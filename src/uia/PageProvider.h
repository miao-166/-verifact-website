/* Copyright 2022 the SumatraPDF project authors (see AUTHORS file).
   License: GPLv3 */

struct DisplayModel;
class TuYueUIAutomationDocumentProvider;
class TuYueUIAutomationPageProvider : public IRawElementProviderFragment,
                                        public IRawElementProviderSimple,
                                        public IValueProvider {
    LONG refCount;
    int pageNum;
    HWND canvasHwnd;
    DisplayModel* dm;
    TuYueUIAutomationDocumentProvider* root;

    TuYueUIAutomationPageProvider* sibling_prev;
    TuYueUIAutomationPageProvider* sibling_next;

    // is dm released, and our root has released us.
    // Only UIA keeps us alive but we can't access anything
    bool released;

    friend class TuYueUIAutomationDocumentProvider; // for setting up next/prev sibling

  public:
    TuYueUIAutomationPageProvider(int pageNum, HWND canvasHwnd, DisplayModel* dm,
                                    TuYueUIAutomationDocumentProvider* root);
    ~TuYueUIAutomationPageProvider();

    int GetPageNum() const;
    TuYueUIAutomationPageProvider* GetNextPage();
    TuYueUIAutomationPageProvider* GetPreviousPage();

    // IUnknown
    HRESULT STDMETHODCALLTYPE QueryInterface(const IID&, void**);
    ULONG STDMETHODCALLTYPE AddRef();
    ULONG STDMETHODCALLTYPE Release();

    // IRawElementProviderFragment
    HRESULT STDMETHODCALLTYPE Navigate(enum NavigateDirection direction, IRawElementProviderFragment** pRetVal);
    HRESULT STDMETHODCALLTYPE GetRuntimeId(SAFEARRAY** pRetVal);
    HRESULT STDMETHODCALLTYPE GetEmbeddedFragmentRoots(SAFEARRAY** pRetVal);
    HRESULT STDMETHODCALLTYPE SetFocus();
    HRESULT STDMETHODCALLTYPE get_BoundingRectangle(struct UiaRect* pRetVal);
    HRESULT STDMETHODCALLTYPE get_FragmentRoot(IRawElementProviderFragmentRoot** pRetVal);

    // IRawElementProviderSimple
    HRESULT STDMETHODCALLTYPE GetPatternProvider(PATTERNID patternId, IUnknown** pRetVal);
    HRESULT STDMETHODCALLTYPE GetPropertyValue(PROPERTYID propertyId, VARIANT* pRetVal);
    HRESULT STDMETHODCALLTYPE get_HostRawElementProvider(IRawElementProviderSimple** pRetVal);
    HRESULT STDMETHODCALLTYPE get_ProviderOptions(ProviderOptions* pRetVal);

    // IValueProvider
    HRESULT STDMETHODCALLTYPE SetValue(LPCWSTR val);
    HRESULT STDMETHODCALLTYPE get_Value(BSTR* pRetVal);
    HRESULT STDMETHODCALLTYPE get_IsReadOnly(BOOL* pRetVal);
};
