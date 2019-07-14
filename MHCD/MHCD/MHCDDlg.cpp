// MHCDDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MHCD.h"
#include "MHCDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define MYWM_NOTIFYICON WM_USER+100
#define   WM_MYMESSAGE   WM_USER+101 
BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip) ;
BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID) ;
int EnableDebugPriv(const char * name);
DWORD FindTarget( LPCTSTR lpszProcess );
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CMHCDDlg 对话框




CMHCDDlg::CMHCDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMHCDDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMHCDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, CListMsg);
}

BEGIN_MESSAGE_MAP(CMHCDDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_LISTCLEAR, &CMHCDDlg::OnBnClickedButtonListclear)
	ON_BN_CLICKED(IDC_BUTTON_START, &CMHCDDlg::OnBnClickedButtonStart)
//	ON_WM_TIMER()
	ON_WM_COPYDATA()
	ON_BN_CLICKED(IDC_BUTTON_SHOWHELP, &CMHCDDlg::OnBnClickedButtonShowhelp)
	ON_BN_CLICKED(IDC_BUTTON_SHOWABOUT, &CMHCDDlg::OnBnClickedButtonShowabout)
	ON_MESSAGE(MYWM_NOTIFYICON, &CMHCDDlg::OnMyIconNotify)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CMHCDDlg 消息处理程序

BOOL CMHCDDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	CreateMutex(NULL,false,"MHCD by suwuhao");     
	EnableDebugPriv(SE_DEBUG_NAME);
	MyTaskBarAddIcon(this->GetSafeHwnd(),IDR_MAINFRAME,m_hIcon,"MapHackClickDetector by suwuhao");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMHCDDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else if ( nID==SC_MINIMIZE  )
	{
		CDialog::OnSysCommand(nID, lParam);
		ShowWindow( SW_HIDE );
	}
	else if(nID == SC_CLOSE)
	{
		if(FindTarget("war3.exe"))
			AfxMessageBox("先关闭WAR3");
		else CDialog::OnSysCommand(nID, lParam);
	}
	else CDialog::OnSysCommand(nID, lParam);
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMHCDDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMHCDDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CMHCDDlg::OnBnClickedButtonListclear()
{
	CListMsg.ResetContent();
}


int EnableDebugPriv(const char * name)//提提权函数
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tp;
    LUID luid;
    //打开进程令牌环
    if(!OpenProcessToken(GetCurrentProcess(),
    TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,
        &hToken))
    {
        MessageBox(NULL,(LPCSTR)"OpenProcessToken Error!",(LPCSTR)"Error!",MB_OK);
            return 1;
    }
    //获得进程本地唯一ID
    if(!LookupPrivilegeValue(NULL,name,&luid))
    {
        MessageBox(NULL,(LPCSTR)"LookupPrivivlegeValue Error!",(LPCSTR)"Error",MB_OK);
    }
    tp.PrivilegeCount=1;
    tp.Privileges[0].Attributes=SE_PRIVILEGE_ENABLED;
    tp.Privileges[0].Luid=luid;
    //调整权限
    if(!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
    {
        MessageBox(NULL,(LPCSTR)"AdjustTokenPrivileges Error!",(LPCSTR)"Error",MB_OK);
        return 1;
    }
    return 0;
}

DWORD FindTarget( LPCTSTR lpszProcess )
{
    DWORD dwRet = 0;
    HANDLE hSnapshot = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof( PROCESSENTRY32 );
    Process32First( hSnapshot, &pe32 );
    do
    {
        if ( lstrcmpi( pe32.szExeFile, lpszProcess ) == 0 )
        {
            dwRet = pe32.th32ProcessID;
            break;
        }
    } while ( Process32Next( hSnapshot, &pe32 ) );
    CloseHandle( hSnapshot );
    return dwRet;
}

BOOL RemoteLoadLibrary( DWORD dwProcessID, LPCSTR lpszDll )
{
    // 打开目标进程
    HANDLE hProcess = OpenProcess( PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION | PROCESS_VM_WRITE, FALSE, dwProcessID );
    
	if(!hProcess)
	{
		AfxMessageBox( (LPCTSTR)"OpenProcess失败。");
		return FALSE;
	}
	// 向目标进程地址空间写入DLL名称
    DWORD dwSize, dwWritten;
    dwSize = lstrlenA( lpszDll ) + 1;
    LPVOID lpBuf = VirtualAllocEx( hProcess, NULL, dwSize, MEM_COMMIT, PAGE_READWRITE );
    if ( NULL == lpBuf )
    {
		AfxMessageBox( (LPCTSTR)"VirtualAllocEx失败1。");
        CloseHandle( hProcess );
        return FALSE;
    }
    if ( WriteProcessMemory( hProcess, lpBuf, (LPVOID)lpszDll, dwSize, &dwWritten ) )
    {
        // 要写入字节数与实际写入字节数不相等，仍属失败
        if ( dwWritten != dwSize )
        {
			AfxMessageBox( (LPCTSTR)"VirtualAllocEx失败2。");
            VirtualFreeEx( hProcess, lpBuf, dwSize, MEM_DECOMMIT );
            CloseHandle( hProcess );
            return FALSE;
        }
    }
    else
    {
		AfxMessageBox( (LPCTSTR)"WriteProcessMemory失败。");
        CloseHandle( hProcess );
        return FALSE;
    }
    // 使目标进程调用LoadLibrary，加载DLL
    DWORD dwID;
    LPVOID pFunc = LoadLibraryA;
    HANDLE hThread = CreateRemoteThread( hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)pFunc, lpBuf, 0, &dwID );
    // 等待LoadLibrary加载完毕
    //WaitForSingleObject( hThread, INFINITE );
    // 释放目标进程中申请的空间
    //VirtualFreeEx( hProcess, lpBuf, dwSize, MEM_DECOMMIT );
    CloseHandle( hThread );
    CloseHandle( hProcess );
    return TRUE;
}

static DWORD dwProcessID;
HANDLE f_;
char * pInfo;
char lastInfo = 100;

void CMHCDDlg::OnBnClickedButtonStart()
{
	char szTarget[MAX_PATH]="war3.exe";

	dwProcessID = FindTarget( (LPCTSTR)szTarget );
	if ( 0 == dwProcessID )
	{
		AfxMessageBox( (LPCTSTR)"找不到目标进程。");
		//return;
	}
	else if ( !RemoteLoadLibrary( dwProcessID, (LPCSTR)"MHCD.dll" ) )
	{
		AfxMessageBox( (LPCTSTR)"远程DLL加载失败。");
		//return;
	}
	else
	{
		CListMsg.ResetContent();
	}

}

BOOL CMHCDDlg::OnCopyData(CWnd* pWnd, COPYDATASTRUCT* pCopyDataStruct)
{
	if(pCopyDataStruct -> dwData == 0x33)
	{
		CListMsg.AddString((char *)(pCopyDataStruct -> lpData));
		CListMsg.SetTopIndex(CListMsg.GetCount() - 1);
	}

	return CDialog::OnCopyData(pWnd, pCopyDataStruct);
}

void CMHCDDlg::OnBnClickedButtonShowhelp()
{
	CListMsg.AddString("把exe和dll拷到魔兽目录。在每次启动游戏后点初始化就可以了。");
	CListMsg.AddString("Win7/Vista的同学如果不是从平台启动魔兽的话请以管理员权限启动魔兽。");
	CListMsg.AddString("免责声明：任何个人和团体不能使用此程序盈利，作者不承担任何由使用本程序引起的一切后果");
	CListMsg.SetTopIndex(CListMsg.GetCount() - 1);
}

void CMHCDDlg::OnBnClickedButtonShowabout()
{
	CAboutDlg dlgAbout;
	dlgAbout.DoModal();
}


BOOL MyTaskBarAddIcon(HWND hwnd, UINT uID, HICON hicon, LPSTR lpszTip) 
{ 
    BOOL res; 
	NOTIFYICONDATA tnid; 
    tnid.cbSize = sizeof(NOTIFYICONDATA); 
	tnid.hWnd = hwnd; 
    tnid.uID = uID;     
	tnid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE; 
    tnid.uCallbackMessage = MYWM_NOTIFYICON; 
	tnid.hIcon = hicon; 
    if (lpszTip) 
lstrcpyn(tnid.szTip, lpszTip, sizeof(tnid.szTip)); 
    else         
		tnid.szTip[0] = '\0';  
    res = Shell_NotifyIcon(NIM_ADD, &tnid); 
	if (hicon) 
        DestroyIcon(hicon); 
	return res; 
} 

BOOL MyTaskBarDeleteIcon(HWND hwnd, UINT uID) 
{	
	BOOL res; 
    NOTIFYICONDATA tnid; 
	tnid.cbSize = sizeof(NOTIFYICONDATA); 
    tnid.hWnd = hwnd;
	tnid.uID = uID; 
    res = Shell_NotifyIcon(NIM_DELETE, &tnid); 
	return res; 
}  

afx_msg LRESULT CMHCDDlg::OnMyIconNotify(WPARAM wParam, LPARAM lParam)
{
	UINT nID;
	UINT uMouseMsg; 
	nID = (UINT)wParam;
    uMouseMsg = (UINT) lParam; 
	if (uMouseMsg == WM_LBUTTONDOWN) //点击图标
	{ 
	ShowWindow( SW_SHOWNORMAL );
	SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);
	SetWindowPos(&wndNoTopMost,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);	
	}
	return 0;
}
void CMHCDDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	MyTaskBarDeleteIcon(GetSafeHwnd(),IDR_MAINFRAME);
}
