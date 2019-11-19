
// BasicDemoDlg.cpp : 实现文件
// 

#include "stdafx.h"
#include "BasicDemoCallback.h"
#include "BasicDemoCallbackDlg.h"
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



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


// CBasicDemoDlg 对话框




CBasicDemoDlg::CBasicDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBasicDemoDlg::IDD, pParent)
    , m_pcMyCamera(NULL)
    , m_nDeviceCombo(0)
    , m_bRetStatus(STATUS_ERROR)                       
    , m_bCreateDevice(FALSE)
    , m_bOpenDevice(FALSE)
    , m_bStartGrabbing(FALSE)
    , m_nTriggerMode(0)
    , m_dExposureEdit(0)
    , m_dGainEdit(0)
    , m_dFrameRateEdit(0)
    , m_bSoftWareTriggerCheck(FALSE)
    , m_nSaveImageType(MV_Image_Undefined)
    , m_nTriggerSource(0)
    , m_pBufForSaveImage(NULL)
    , m_nBufSizeForSaveImage(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBasicDemoDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_OPEN_BUTTON, m_ctrlOpenButton);
    DDX_Control(pDX, IDC_CLOSE_BUTTON, m_ctrlCloseButton);
    DDX_Control(pDX, IDC_START_GRABBING_BUTTON, m_ctrlStartGrabbingButton);
    DDX_Control(pDX, IDC_STOP_GRABBING_BUTTON, m_ctrlStopGrabbingButton);
    DDX_Control(pDX, IDC_SOFTWARE_TRIGGER_CHECK, m_ctrlSoftwareTriggerCheck);
    DDX_Control(pDX, IDC_SOFTWARE_ONCE_BUTTON, m_ctrlSoftwareOnceButton);
    DDX_Control(pDX, IDC_SAVE_BMP_BUTTON, m_ctrlSaveBmpButton);
    DDX_Control(pDX, IDC_SAVE_JPG_BUTTON, m_ctrlSaveJpgButton);
    DDX_Control(pDX, IDC_EXPOSURE_EDIT, m_ctrlExposureEdit);
    DDX_Control(pDX, IDC_GAIN_EDIT, m_ctrlGainEdit);
    DDX_Control(pDX, IDC_FRAME_RATE_EDIT, m_ctrlFrameRateEdit);
    DDX_Control(pDX, IDC_GET_PARAMETER_BUTTON, m_ctrlGetParameterButton);
    DDX_Control(pDX, IDC_SET_PARAMETER_BUTTON, m_ctrlSetParameterButton);
    DDX_Control(pDX, IDC_CONTINUS_MODE_RADIO, m_ctrlContinusModeRadio);
    DDX_Control(pDX, IDC_TRIGGER_MODE_RADIO, m_ctrlTriggerModeRadio);
    DDX_Control(pDX, IDC_DEVICE_COMBO, m_ctrlDeviceCombo);
    DDX_CBIndex(pDX, IDC_DEVICE_COMBO, m_nDeviceCombo);
    DDX_Text(pDX, IDC_EXPOSURE_EDIT, m_dExposureEdit);
    DDX_Text(pDX, IDC_GAIN_EDIT, m_dGainEdit);
    DDX_Text(pDX, IDC_FRAME_RATE_EDIT, m_dFrameRateEdit);
    DDX_Check(pDX, IDC_SOFTWARE_TRIGGER_CHECK, m_bSoftWareTriggerCheck);
}

BEGIN_MESSAGE_MAP(CBasicDemoDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	// }}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_ENUM_BUTTON, &CBasicDemoDlg::OnBnClickedEnumButton)
    ON_BN_CLICKED(IDC_OPEN_BUTTON, &CBasicDemoDlg::OnBnClickedOpenButton)
    ON_BN_CLICKED(IDC_CLOSE_BUTTON, &CBasicDemoDlg::OnBnClickedCloseButton)
    ON_BN_CLICKED(IDC_CONTINUS_MODE_RADIO, &CBasicDemoDlg::OnBnClickedContinusModeRadio)
    ON_BN_CLICKED(IDC_TRIGGER_MODE_RADIO, &CBasicDemoDlg::OnBnClickedTriggerModeRadio)
    ON_BN_CLICKED(IDC_START_GRABBING_BUTTON, &CBasicDemoDlg::OnBnClickedStartGrabbingButton)
    ON_BN_CLICKED(IDC_STOP_GRABBING_BUTTON, &CBasicDemoDlg::OnBnClickedStopGrabbingButton)
    ON_BN_CLICKED(IDC_GET_PARAMETER_BUTTON, &CBasicDemoDlg::OnBnClickedGetParameterButton)
    ON_BN_CLICKED(IDC_SET_PARAMETER_BUTTON, &CBasicDemoDlg::OnBnClickedSetParameterButton)
    ON_BN_CLICKED(IDC_SOFTWARE_TRIGGER_CHECK, &CBasicDemoDlg::OnBnClickedSoftwareTriggerCheck)
    ON_BN_CLICKED(IDC_SOFTWARE_ONCE_BUTTON, &CBasicDemoDlg::OnBnClickedSoftwareOnceButton)
    ON_BN_CLICKED(IDC_SAVE_BMP_BUTTON, &CBasicDemoDlg::OnBnClickedSaveBmpButton)
    ON_BN_CLICKED(IDC_SAVE_JPG_BUTTON, &CBasicDemoDlg::OnBnClickedSaveJpgButton)
    ON_WM_CLOSE()
END_MESSAGE_MAP()


// CBasicDemoDlg 消息处理程序

BOOL CBasicDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
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

	DisplayWindowInitial();     // 显示框初始化

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CBasicDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBasicDemoDlg::OnPaint()
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

// 当用户拖动最小化窗口时系统调用此函数取得光标
// 显示。
HCURSOR CBasicDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*关闭设备时的窗口显示*/
Status CBasicDemoDlg::EnableWindowWhenClose(void)
{
    m_ctrlOpenButton.EnableWindow(FALSE);
    m_ctrlCloseButton.EnableWindow(FALSE);
    m_ctrlStartGrabbingButton.EnableWindow(FALSE);
    m_ctrlStopGrabbingButton.EnableWindow(FALSE);
    m_ctrlSoftwareTriggerCheck.EnableWindow(FALSE);
    m_ctrlSoftwareOnceButton.EnableWindow(FALSE);
    m_ctrlSaveBmpButton.EnableWindow(FALSE);
    m_ctrlSaveJpgButton.EnableWindow(FALSE);
    m_ctrlExposureEdit.EnableWindow(FALSE);
    m_ctrlGainEdit.EnableWindow(FALSE);
    m_ctrlFrameRateEdit.EnableWindow(FALSE);
    m_ctrlGetParameterButton.EnableWindow(FALSE);
    m_ctrlSetParameterButton.EnableWindow(FALSE);
    m_ctrlContinusModeRadio.EnableWindow(FALSE);
    m_ctrlTriggerModeRadio.EnableWindow(FALSE);
    return STATUS_OK;
}

/*打开设备但不开始抓图*/
Status CBasicDemoDlg::EnableWindowWhenOpenNotStart(void)
{
    m_ctrlOpenButton.EnableWindow(FALSE);
    m_ctrlCloseButton.EnableWindow(TRUE);
    m_ctrlStartGrabbingButton.EnableWindow(TRUE);
    m_ctrlSoftwareTriggerCheck.EnableWindow(TRUE);
    m_ctrlExposureEdit.EnableWindow(TRUE);
    m_ctrlGainEdit.EnableWindow(TRUE);
    m_ctrlFrameRateEdit.EnableWindow(TRUE);
    m_ctrlGetParameterButton.EnableWindow(TRUE);
    m_ctrlSetParameterButton.EnableWindow(TRUE);
    m_ctrlContinusModeRadio.EnableWindow(TRUE);
    m_ctrlTriggerModeRadio.EnableWindow(TRUE);
    return STATUS_OK;
}

// 按下开始采集按钮时的按钮颜色  
Status CBasicDemoDlg::EnableWindowWhenStart(void)
{
    m_ctrlStopGrabbingButton.EnableWindow(TRUE);
    m_ctrlStartGrabbingButton.EnableWindow(FALSE);
    m_ctrlSaveBmpButton.EnableWindow(TRUE);
    m_ctrlSaveJpgButton.EnableWindow(TRUE);
    if (TRUE == m_bSoftWareTriggerCheck && MV_TRIGGER_MODE_ON == m_nTriggerMode)
    {
        m_ctrlSoftwareOnceButton.EnableWindow(TRUE);
    }
    return STATUS_OK;
}

// 按下结束采集时的按钮颜色
Status CBasicDemoDlg::EnableWindowWhenStop(void)
{
    m_ctrlStopGrabbingButton.EnableWindow(FALSE);
    m_ctrlStartGrabbingButton.EnableWindow(TRUE);
    m_ctrlSaveBmpButton.EnableWindow(FALSE);
    m_ctrlSaveJpgButton.EnableWindow(FALSE);
    m_ctrlSoftwareOnceButton.EnableWindow(FALSE);
    return STATUS_OK;
}

/*最开始时的窗口初始化*/
Status CBasicDemoDlg::DisplayWindowInitial(void)
{
	CWnd *pWnd = GetDlgItem(IDC_DISPLAY_STATIC);
    if (NULL == pWnd)
    {
        return STATUS_ERROR;
    }
    m_hwndDisplay = pWnd->GetSafeHwnd();
    if (NULL == m_hwndDisplay)
    {
        return STATUS_ERROR;
    }

    EnableWindowWhenClose();
    return STATUS_OK;
}

// 打开设备
Status CBasicDemoDlg::OpenDevice(void)
{
    if (TRUE == m_bOpenDevice)
    {
        return STATUS_ERROR;
    }
    UpdateData(TRUE);
    if(TRUE == m_bCreateDevice)
    {
        return STATUS_ERROR;
    }
    int nIndex = m_nDeviceCombo;
    if ((nIndex < 0) | (nIndex >= MV_MAX_DEVICE_NUM))
    {
        return STATUS_ERROR;
    }

    // 由设备信息创建设备实例
    if (NULL == m_stDevList.pDeviceInfo[nIndex])
    {
        return STATUS_ERROR;
    }

    if (NULL != m_pcMyCamera)
    {
        return STATUS_ERROR;
    }

    m_pcMyCamera = new CMyCamera;
    /*创建设备失败*/
    if (NULL == m_pcMyCamera)
    {
        m_bRetStatus = STATUS_ERROR;
        return STATUS_ERROR;
    }

    int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
    /*创建设备失败*/
    if (MV_OK != nRet)
    {
        delete m_pcMyCamera;
        m_pcMyCamera = NULL;
        m_bRetStatus = STATUS_ERROR;
        return STATUS_ERROR;
    }

    m_bCreateDevice = TRUE;

    if (FALSE == m_bCreateDevice || TRUE == m_bOpenDevice)
    {        
        return STATUS_ERROR;
    }
    m_bRetStatus = STATUS_OK;

    m_bOpenDevice = TRUE;
    return STATUS_OK;
}

// 关闭设备
Status CBasicDemoDlg::CloseDevice(void)
{   
    int nRet = MV_OK;
    if (FALSE == m_bCreateDevice || FALSE == m_bOpenDevice)
    {
        return STATUS_ERROR;
    }
    m_bRetStatus = STATUS_OK;

    if (m_pcMyCamera)
    {
        m_pcMyCamera->Close();
        delete m_pcMyCamera;
        m_pcMyCamera = NULL;
    }

    if (m_pBufForSaveImage)
    {
        free(m_pBufForSaveImage);
        m_pBufForSaveImage = NULL;
    }
    m_nBufSizeForSaveImage  = 0;

    m_bOpenDevice = FALSE;
    m_bStartGrabbing = FALSE;
    return STATUS_OK;
}

// 销毁设备
Status CBasicDemoDlg::DestroyHandle(void)
{
    if (FALSE == m_bCreateDevice)
    {
        // return STATUS_ERROR; 
    }
    m_bRetStatus = STATUS_OK;
    int nRet = MV_OK;

    if (m_pcMyCamera)
    {
        m_pcMyCamera->Close();
        delete m_pcMyCamera;
        m_pcMyCamera = NULL;
    }

    m_bCreateDevice = FALSE;
    m_bOpenDevice = FALSE;
    m_bStartGrabbing = FALSE;
    return STATUS_OK;
}

// 获取触发模式
Status CBasicDemoDlg::GetTriggerMode(void)
{
    unsigned int nEnumValue = 0;

    int nRet = m_pcMyCamera->GetEnumValue("TriggerMode", &nEnumValue);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    m_nTriggerMode = nEnumValue;
    if (MV_TRIGGER_MODE_ON ==  m_nTriggerMode)
    {
        OnBnClickedTriggerModeRadio();
    }
    else if (MV_TRIGGER_MODE_OFF == m_nTriggerMode)
    {
        OnBnClickedContinusModeRadio();
    }
    else
    {
        return STATUS_ERROR;
    }
    UpdateData(FALSE);

    return STATUS_OK;
}

// 设置触发模式
Status CBasicDemoDlg::SetTriggerMode(void)
{
    int nRet = m_pcMyCamera->SetEnumValue("TriggerMode", m_nTriggerMode);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

// 获取曝光时间
Status CBasicDemoDlg::GetExposureTime(void)
{
    float  fFloatValue = 0.0;
    int nRet = m_pcMyCamera->GetFloatValue("ExposureTime", &fFloatValue);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    m_dExposureEdit = fFloatValue;
    UpdateData(FALSE);
    return STATUS_OK;
}

// 设置曝光时间
Status CBasicDemoDlg::SetExposureTime(void)
{
    float  dfFloatValue = 0.0;

    /*调节这两个曝光模式，才能让曝光时间生效*/
    int nRet = m_pcMyCamera->SetEnumValue("ExposureMode", 0);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    nRet = m_pcMyCamera->SetEnumValue("ExposureAuto", 0);
    if (MV_OK != nRet)
    {
        // return STATUS_ERROR;      // 设置失败有可能是开了触发模式，所以无需返回
    }

    nRet = m_pcMyCamera->SetFloatValue("ExposureTime", m_dExposureEdit);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    UpdateData(FALSE);

    return STATUS_OK;
}

// 获取增益
Status CBasicDemoDlg::GetGain(void)
{
    float  fFloatValue = 0.0;

    int nRet = m_pcMyCamera->GetFloatValue("Gain", &fFloatValue);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }
    fFloatValue = (int)fFloatValue;
    
    m_dGainEdit = fFloatValue;
    UpdateData(FALSE);

    return STATUS_OK;
}

// 设置增益
Status CBasicDemoDlg::SetGain(void)
{
    int nRet = m_pcMyCamera->SetEnumValue("GainAuto", 0);   // 设置增益前先把自动增益关闭，失败无需返回

    nRet = m_pcMyCamera->SetFloatValue("Gain", m_dGainEdit);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    UpdateData(FALSE);

    return STATUS_OK;
}

// 获取帧率
Status CBasicDemoDlg::GetFrameRate(void)
{
    float  fFloatValue = 0.0;

    int nRet = m_pcMyCamera->GetFloatValue("ResultingFrameRate", &fFloatValue);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    m_dFrameRateEdit = fFloatValue;
    UpdateData(FALSE);

    return STATUS_OK;
}

// 设置帧率
Status CBasicDemoDlg::SetFrameRate(void)
{
    CString strFeature;

    int nRet = m_pcMyCamera->SetBoolValue("AcquisitionFrameRateEnable", TRUE);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    nRet = m_pcMyCamera->SetFloatValue("AcquisitionFrameRate", m_dFrameRateEdit);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    UpdateData(FALSE);

    return STATUS_OK;
}

// 获取触发源
Status CBasicDemoDlg::GetTriggerSource(void)
{
    unsigned int nEnumValue = 0;

    int nRet = m_pcMyCamera->GetEnumValue("TriggerSource", &nEnumValue);
    if (MV_OK != nRet)
    {
        return STATUS_ERROR;
    }

    if ((unsigned int)MV_TRIGGER_SOURCE_SOFTWARE == nEnumValue)
    {
        m_bSoftWareTriggerCheck = TRUE;
    }
    else
    {
        m_bSoftWareTriggerCheck = FALSE;
    }
    UpdateData(FALSE);

    return STATUS_OK;
}

// 设置触发源
Status CBasicDemoDlg::SetTriggerSource(void)
{
    UpdateData(TRUE);

    if (m_bSoftWareTriggerCheck)
    {
        m_nTriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
        int nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
        if (MV_OK != nRet)
        {
            MessageBox(TEXT("设置软触发失败"), TEXT("警告"), MB_OK | MB_ICONWARNING);
            return STATUS_ERROR;
        }
        m_ctrlSoftwareOnceButton.EnableWindow(TRUE);
    }
    else
    {
        m_nTriggerSource = MV_TRIGGER_SOURCE_LINE0;
        int nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
        if (MV_OK != nRet)
        {
            MessageBox(TEXT("设置硬触发失败"), TEXT("警告"), MB_OK | MB_ICONWARNING);
            return STATUS_ERROR;
        }
        m_ctrlSoftwareOnceButton.EnableWindow(FALSE);
    }

    return STATUS_OK;
}

// 相机抓图回调, 主要用来获取图片
void   __stdcall CBasicDemoDlg::ImageCallBack(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	if (pFrameInfo)
    {
        CBasicDemoDlg *pBasicDemo = (CBasicDemoDlg*)pUser;
		pBasicDemo->MessageBox(TEXT("image callback called!!!!!"));
        if (MV_Image_Undefined == pBasicDemo->m_nSaveImageType || NULL == pData)
        {
            return;
        }

        // 设置对应的相机参数

        // 仅在第一次保存图像时申请缓存，在 CloseDevice 时释放
        if (NULL == pBasicDemo->m_pBufForSaveImage)
        {
            // BMP图片大小：width * height * 3 + 2048(预留BMP头大小)
            pBasicDemo->m_nBufSizeForSaveImage = pFrameInfo->nWidth * pFrameInfo->nHeight * 3 + 2048;

            pBasicDemo->m_pBufForSaveImage = (unsigned char*)malloc(pBasicDemo->m_nBufSizeForSaveImage);
            if (NULL == pBasicDemo->m_pBufForSaveImage)
            {
                return;
            }
        }

        MV_SAVE_IMAGE_PARAM_EX stParam = {0};
        stParam.enImageType = pBasicDemo->m_nSaveImageType; // 需要保存的图像类型
        stParam.enPixelType = pFrameInfo->enPixelType;  // 相机对应的像素格式
        stParam.nBufferSize = pBasicDemo->m_nBufSizeForSaveImage;  // 存储节点的大小
        stParam.nWidth      = pFrameInfo->nWidth;         // 相机对应的宽
        stParam.nHeight     = pFrameInfo->nHeight;          // 相机对应的高
        stParam.nDataLen    = pFrameInfo->nFrameLen;
        stParam.pData       = pData;
        stParam.pImageBuffer = pBasicDemo->m_pBufForSaveImage;
        stParam.nJpgQuality = 80;

        int nRet = pBasicDemo->m_pcMyCamera->SaveImage(&stParam);
        if(MV_OK != nRet)
        {
        }
        char pImageName[32] = {0};
        if (NULL == pImageName)
        {
            return;
        }
        if (MV_Image_Bmp == stParam.enImageType)
        {
            sprintf_s(pImageName, 20, "%03d.bmp", pFrameInfo->nFrameNum);
        }
        else if (MV_Image_Jpeg == stParam.enImageType)
        {
            sprintf_s(pImageName, 20, "%03d.jpg", pFrameInfo->nFrameNum);
        }

        pBasicDemo->m_nSaveImageType = MV_Image_Undefined;
        FILE* fp = fopen(pImageName, "wb");
        if (NULL == fp)
        {
            pBasicDemo->MessageBox(TEXT("fopen failed"));
            return;
        }
        fwrite(pBasicDemo->m_pBufForSaveImage, 1, stParam.nImageLen, fp);
        fclose(fp);
        pBasicDemo->MessageBox(TEXT("保存成功"), TEXT("成功"), MB_OK | MB_ICONWARNING);
    }
}

// 按下查找设备按钮:枚举
void CBasicDemoDlg::OnBnClickedEnumButton()
{
    m_bRetStatus = STATUS_OK;
    CString strMsg;

    // 1. 清除设备列表框中的信息
    m_ctrlDeviceCombo.ResetContent();

    // 2. 初始化设备信息列表
    memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

    // 3. 枚举子网内所有设备
    int nRet = CMyCamera::EnumDevices(&m_stDevList);

    if (MV_OK != nRet)
    {
        // 枚举设备失败 
        m_bRetStatus = STATUS_ERROR;
        return;
    }
    else
    {
    }

    /*将值加入到信息列表框中并显示出来*/
    unsigned int i;
    int nIp1, nIp2, nIp3, nIp4;
    for (i = 0; i < m_stDevList.nDeviceNum; i++)
    {
        MV_CC_DEVICE_INFO* pDeviceInfo = m_stDevList.pDeviceInfo[i];
        if (NULL == pDeviceInfo)
        {
            continue;
        }
        if (pDeviceInfo->nTLayerType == MV_GIGE_DEVICE)
        {
            nIp1 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0xff000000) >> 24);
            nIp2 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x00ff0000) >> 16);
            nIp3 = ((m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x0000ff00) >> 8);
            nIp4 = (m_stDevList.pDeviceInfo[i]->SpecialInfo.stGigEInfo.nCurrentIp & 0x000000ff);

            wchar_t* pUserName = NULL;
            if (strcmp("", (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName)) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                // DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName), -1, NULL, 0);

                char strUserName[256];
                sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stGigEInfo.chManufacturerName,
                    pDeviceInfo->SpecialInfo.stGigEInfo.chModelName,
                    pDeviceInfo->SpecialInfo.stGigEInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }
            strMsg.Format(_T("[%d]GigE:    %s  (%d.%d.%d.%d)"), i, 
                pUserName, nIp1, nIp2, nIp3, nIp4);
            if (NULL != pUserName)
            {
                delete(pUserName);
                pUserName = NULL;
            }

        }
        else if (pDeviceInfo->nTLayerType == MV_USB_DEVICE)
        {
            wchar_t* pUserName = NULL;

            if (strcmp("", (char*)pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName) != 0)
            {
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(pDeviceInfo->SpecialInfo.stUsb3VInfo.chUserDefinedName), -1, pUserName, dwLenUserName);
            }
            else
            {
                char strUserName[256];
                sprintf_s(strUserName, "%s %s (%s)", pDeviceInfo->SpecialInfo.stUsb3VInfo.chManufacturerName,
                    pDeviceInfo->SpecialInfo.stUsb3VInfo.chModelName,
                    pDeviceInfo->SpecialInfo.stUsb3VInfo.chSerialNumber);
                DWORD dwLenUserName = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, NULL, 0);
                pUserName = new wchar_t[dwLenUserName];
                MultiByteToWideChar(CP_ACP, 0, (LPCSTR)(strUserName), -1, pUserName, dwLenUserName);
            }
            strMsg.Format(_T("[%d]UsbV3:  %s"), i, pUserName);
            if (NULL != pUserName)
            {
                delete(pUserName);
                pUserName = NULL;
            }

        }
        else
        {
        }
        m_ctrlDeviceCombo.AddString(strMsg);
    }
    m_ctrlDeviceCombo.SetCurSel(CB_ERR);

    /*枚举到设备之后要显示出来*/
    UpdateData(FALSE);
    
    if (0 == m_stDevList.nDeviceNum)
    {
        MessageBox(TEXT("无设备"));
        return;
    }
    /*将打开设备的按钮显现出来*/
    m_ctrlOpenButton.EnableWindow(TRUE);
    return;
}

// 按下打开设备按钮：打开设备
void CBasicDemoDlg::OnBnClickedOpenButton()
{
    int nRet = OpenDevice();
    if (STATUS_ERROR == nRet)
    {
        MessageBox(TEXT("打开失败"), TEXT("警告"), MB_OK | MB_ICONWARNING);
        return;
    }

    nRet = m_pcMyCamera->RegisterImageCallBack(ImageCallBack, this);

    OnBnClickedGetParameterButton(); // 获取参数
    EnableWindowWhenOpenNotStart();
    return;
}

// 按下关闭设备按钮：关闭设备
void CBasicDemoDlg::OnBnClickedCloseButton()
{
    CloseDevice();
    DestroyHandle();
    EnableWindowWhenClose();
    m_ctrlOpenButton.EnableWindow(TRUE);
}

// 按下连续模式按钮
void CBasicDemoDlg::OnBnClickedContinusModeRadio()
{
    ((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(TRUE);
    ((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(FALSE);
    m_nTriggerMode = MV_TRIGGER_MODE_OFF;
    int nRet = SetTriggerMode();
    if (STATUS_OK != nRet)
    {
        return;
    }
    m_ctrlSoftwareOnceButton.EnableWindow(FALSE);
    return;
}

// 按下触发模式按钮
void CBasicDemoDlg::OnBnClickedTriggerModeRadio()
{
    ((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(FALSE);
    ((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(TRUE);
    m_nTriggerMode = MV_TRIGGER_MODE_ON;
    int nRet = SetTriggerMode();
    if (STATUS_OK != nRet)
    {
        MessageBox(TEXT("设置触发模式失败"), TEXT("警告"), MB_OK | MB_ICONWARNING);
        return;
    }


    if (m_bStartGrabbing == TRUE)
    {
        UpdateData(TRUE);
        if (TRUE == m_bSoftWareTriggerCheck)
        {
            m_ctrlSoftwareOnceButton.EnableWindow(TRUE);
        }
    }
    
    // UpdateData(FALSE);

    return;
}

// 按下开始采集按钮
void CBasicDemoDlg::OnBnClickedStartGrabbingButton()
{
    if (FALSE == m_bCreateDevice || FALSE == m_bOpenDevice || TRUE == m_bStartGrabbing)
    {
        return;
    }

    int nRet = MV_OK;
    if (NULL != m_pcMyCamera)
    {
        nRet = m_pcMyCamera->StartGrabbing();
        if (nRet == MV_OK)
        {

            m_pcMyCamera->Display(m_hwndDisplay);
        }
    }
    else
    {
        m_bRetStatus = STATUS_ERROR;
        return ;
    }

    if (MV_OK != nRet)
    {
        return ;
    }
    m_bStartGrabbing = TRUE;
    EnableWindowWhenStart();
    return;
}

// 按下结束采集按钮
void CBasicDemoDlg::OnBnClickedStopGrabbingButton()
{
    if (FALSE == m_bCreateDevice || FALSE == m_bOpenDevice || FALSE == m_bStartGrabbing)
    {
        return;
    }

    int nRet = MV_OK;
    if (NULL != m_pcMyCamera)
    {
        nRet = m_pcMyCamera->StopGrabbing();
    }
    else
    {
        m_bRetStatus = STATUS_ERROR;
        return ;
    }

    if (MV_OK != nRet)
    {
        return ;
    }
    m_bStartGrabbing = FALSE;
    EnableWindowWhenStop();
    return;
}

// 按下获取参数按钮
void CBasicDemoDlg::OnBnClickedGetParameterButton()
{
    int nRet = GetTriggerMode();
    if (nRet != STATUS_OK)
    {
        return;
    }

    nRet = GetExposureTime();
    if (nRet != STATUS_OK)
    {
        return;
    }

    nRet = GetGain();
    if (nRet != STATUS_OK)
    {
        return;
    }

    nRet = GetFrameRate();
    if (nRet != STATUS_OK)
    {
        return;
    }

    nRet = GetTriggerSource();
    if (nRet != STATUS_OK)
    {
        return;
    }

    return;
}

// 按下设置参数按钮
void CBasicDemoDlg::OnBnClickedSetParameterButton()
{
    UpdateData(TRUE);

    int nRet = SetExposureTime();
    if (nRet != STATUS_OK)
    {
        MessageBox(TEXT("设置曝光时间失败"), TEXT("警告"), MB_OK | MB_ICONWARNING);
    }
    nRet = SetGain();
    if (nRet != STATUS_OK)
    {
        MessageBox(TEXT("设置增益失败"), TEXT("警告"), MB_OK | MB_ICONWARNING);
    }
    nRet = SetFrameRate();
    if (nRet != STATUS_OK)
    {
        MessageBox(TEXT("设置帧率失败"), TEXT("警告"), MB_OK | MB_ICONWARNING);
    }    

    MessageBox(TEXT("设置参数成功"), TEXT("警告"), MB_OK | MB_ICONWARNING);
    return;
}

// 按下软触发按钮
void CBasicDemoDlg::OnBnClickedSoftwareTriggerCheck()
{
    int nRet = SetTriggerSource();
    if (nRet != STATUS_OK)
    {
        return;
    }
    return;
}

// 按下软触发一次按钮
void CBasicDemoDlg::OnBnClickedSoftwareOnceButton()
{
    if (TRUE != m_bStartGrabbing)
    {
        return;
    }

    int nRet = m_pcMyCamera->CommandExecute("TriggerSoftware");
    if (MV_OK != nRet)
    {
        return;
    }

    return;
}

// 按下保存bmp图片按钮
void CBasicDemoDlg::OnBnClickedSaveBmpButton()
{
    m_nSaveImageType = MV_Image_Bmp;
}

// 按下保存jpg图片按钮
void CBasicDemoDlg::OnBnClickedSaveJpgButton()
{
    m_nSaveImageType = MV_Image_Jpeg;
}

// 右上角退出
void CBasicDemoDlg::OnClose()
{
    PostQuitMessage(0);
    DestroyHandle();
    CDialog::OnClose();
}

BOOL CBasicDemoDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO:  在此添加专用代码和/或调用基类
    if (pMsg->message == WM_KEYDOWN&&pMsg->wParam == VK_ESCAPE)
    {
        // 如果消息是键盘按下事件，且是Esc键，执行以下代码（什么都不做，你可以自己添加想要的代码）
        return TRUE;
    }
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        // 如果消息是键盘按下事件，且是Entert键，执行以下代码（什么都不做，你可以自己添加想要的代码）
        return TRUE;
    }


    return CDialog::PreTranslateMessage(pMsg);
}
