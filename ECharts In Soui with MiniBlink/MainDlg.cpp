// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MainDlg.h"
#include "controls/SMiniBlink.h"
#include <fstream>
	
#ifdef DWMBLUR	//win7毛玻璃开关
#include <dwmapi.h>
#pragma comment(lib,"dwmapi.lib")
#endif
	
CMainDlg::CMainDlg() : SHostWnd(_T("LAYOUT:XML_MAINWND"))
{
	m_bLayoutInited = FALSE;
}

CMainDlg::~CMainDlg()
{
}

int CMainDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	#ifdef DWMBLUR	//win7毛玻璃开关
	MARGINS mar = {5,5,30,5};
	DwmExtendFrameIntoClientArea ( m_hWnd, &mar );
	#endif

	SetMsgHandled(FALSE);
	return 0;
}


BOOL CMainDlg::OnInitDialog(HWND hWnd, LPARAM lParam)
{
	m_bLayoutInited = TRUE;
	SOUI::SWkeWebkit *pWkeWebkit = FindChildByName2<SOUI::SWkeWebkit>(L"wke_test");
	SASSERT(pWkeWebkit);

	if (pWkeWebkit)
	{
		SStringT strPath;
		GetCurrentDirectory(MAX_PATH, strPath.GetBuffer(MAX_PATH));
		strPath.ReleaseBuffer();

		strPath += L"\\echarts\\Demo1.htm";
		pWkeWebkit->SetAttribute(L"url", strPath);
	}
	// 初始化饼图的值
	_GetCurrentValue();
	return 0;
}
//TODO:消息映射
void CMainDlg::OnClose()
{
	CSimpleWnd::DestroyWindow();
}

void CMainDlg::OnMaximize()
{
	SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE);
}
void CMainDlg::OnRestore()
{
	SendMessage(WM_SYSCOMMAND, SC_RESTORE);
}
void CMainDlg::OnMinimize()
{
	SendMessage(WM_SYSCOMMAND, SC_MINIMIZE);
}

// 控件响应：C++ 控制图表变化显示
void CMainDlg::OnBtnChangePie1()
{
	_CallJsChangeValue(++m_uValue1, m_uValue2, m_uValue3, m_uValue4);
}

void CMainDlg::OnBtnChangePie2()
{
	_CallJsChangeValue(m_uValue1, ++m_uValue2, m_uValue3, m_uValue4);
}

void CMainDlg::OnBtnChangePie3()
{
	_CallJsChangeValue(m_uValue1, m_uValue2, ++m_uValue3, m_uValue4);
}

void CMainDlg::OnBtnChangePie4()
{
	_CallJsChangeValue(m_uValue1, m_uValue2, m_uValue3, ++m_uValue4);
}

// 调用 js 函数 ChangeValue
HRESULT CMainDlg::_CallJsChangeValue(UINT uValue1, UINT uValue2, UINT uValue3, UINT uValue4)
{
	SOUI::SWkeWebkit *pWkeWebkit = FindChildByName2<SOUI::SWkeWebkit>(L"wke_test");
	if (pWkeWebkit != NULL) {
		SOUI::SStringA strCallJs;
		strCallJs.Format("changeValue(%d,%d,%d,%d);", uValue1, uValue2, uValue3, uValue4);
		wkeRunJS(pWkeWebkit->GetWebView(), strCallJs);
	}
	return S_OK;
}

// 获取当前页面数据
HRESULT CMainDlg::_GetCurrentValue()
{
	SOUI::SWkeWebkit *pWkeWebkit = FindChildByName2<SOUI::SWkeWebkit>(L"wke_test");
	if (pWkeWebkit != NULL) {
		// 方法一：使用 jsGetGlobal 函数获取值
		jsValue jsRet = jsGetGlobal(wkeGlobalExec(pWkeWebkit->GetWebView()), "values");
		// 方法二：使用 wkeRunJS 函数获取值
		// MiniBlink 要返回 js 函数的值需要加 return!!!
		// 但是不知道为什么 getValue 函数返回出来的 values 居然不是 Array 类型而是 String 类型
		// 这个问题还需要继续查看
		// jsValue jsRet = wkeRunJS(pWkeWebkit->GetWebView(), "return getValue();");
		jsType type = jsTypeOf(jsRet);
		if (jsIsArray(jsRet)) {
			m_uValue1 = jsToInt(wkeGlobalExec(pWkeWebkit->GetWebView()), jsGetAt(wkeGlobalExec(pWkeWebkit->GetWebView()), jsRet, 0));
			m_uValue2 = jsToInt(wkeGlobalExec(pWkeWebkit->GetWebView()), jsGetAt(wkeGlobalExec(pWkeWebkit->GetWebView()), jsRet, 1));
			m_uValue4 = jsToInt(wkeGlobalExec(pWkeWebkit->GetWebView()), jsGetAt(wkeGlobalExec(pWkeWebkit->GetWebView()), jsRet, 3));
		}

		int i = 1;
	}
	return S_OK;
}

void CMainDlg::OnSize(UINT nType, CSize size)
{
	SetMsgHandled(FALSE);
	if (!m_bLayoutInited) return;
	
	SWindow *pBtnMax = FindChildByName(L"btn_max");
	SWindow *pBtnRestore = FindChildByName(L"btn_restore");
	if(!pBtnMax || !pBtnRestore) return;
	
	if (nType == SIZE_MAXIMIZED)
	{
		pBtnRestore->SetVisible(TRUE);
		pBtnMax->SetVisible(FALSE);
	}
	else if (nType == SIZE_RESTORED)
	{
		pBtnRestore->SetVisible(FALSE);
		pBtnMax->SetVisible(TRUE);
	}
}

