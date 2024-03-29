
// BasicDemoDlg.h : 头文件
// 

#pragma once
#include "afxwin.h" 

#include <stdio.h>

#include "MyCamera.h"

/*长度宏定义*/
#define MAX_BUF_SIZE            (2560*1920*6)
#define MAX_XML_FILE_SIZE       (1024*1024*3)

/*函数返回码定义*/
typedef int Status;
#define STATUS_OK               0
#define STATUS_ERROR            -1

//void onChangeTrackbar(int, void*);

// CBasicDemoDlg 对话框
class CBasicDemoDlg : public CDialog
{
// 构造
public:
	CBasicDemoDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BasicDemo_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

/*控件对应变量*/
private:
    /*初始化*/
    CButton m_ctrlOpenButton;                   // 打开设备
    CButton m_ctrlCloseButton;                  // 关闭设备

    /*图像采集*/
    CButton m_ctrlContinusModeRadio;            // 连续模式             
    CButton m_ctrlTriggerModeRadio;             // 触发模式
    CButton m_ctrlStartGrabbingButton;          // 开始抓图          
    CButton m_ctrlStopGrabbingButton;           // 结束抓图
    CButton m_ctrlSoftwareTriggerCheck;         // 软触发
    BOOL    m_bSoftWareTriggerCheck;
    CButton m_ctrlSoftwareOnceButton;           // 软触发一次

    /*图像保存*/
    CButton m_ctrlSaveBmpButton;                // 保存为bmp
    CButton m_ctrlSaveJpgButton;                // 保存为jpg

    /*参数设置获取*/
    CEdit   m_ctrlExposureEdit;                 // 曝光时间
    double  m_dExposureEdit;
    CEdit   m_ctrlGainEdit;                     // 增益
    double  m_dGainEdit;
    CEdit   m_ctrlFrameRateEdit;                // 帧率
    double  m_dFrameRateEdit;   
	CEdit   m_ctrlFrameRateEdit2;                // 帧率
	double  m_dFrameRateEdit2;
	CEdit   m_ctrlFrameRateEdit3;                // 帧率
	double  m_dFrameRateEdit3;
	double  m_dFrameRateEdit4;
    CButton m_ctrlGetParameterButton;           // 获取参数
    CButton m_ctrlSetParameterButton;           // 设置参数

    /*设备显示下拉框*/
    CComboBox m_ctrlDeviceCombo;                // 枚举到的设备
    int     m_nDeviceCombo;

/*内部函数*/
private:
    /*最开始时的窗口初始化*/
    Status DisplayWindowInitial(void);          // 显示框初始化,最开始的初始化 

    /*按钮的亮暗使能*/
    Status EnableWindowWhenClose(void);         // 关闭时的按钮颜色，相当于最开始初始化
    Status EnableWindowWhenOpenNotStart(void);  // 按下打开设备按钮时的按钮颜色  
    Status EnableWindowWhenStart(void);         // 按下开始采集时的按钮颜色 
    Status EnableWindowWhenStop(void);          // 按下结束采集时的按钮颜色
    
    /*初始化相机操作*/
    Status OpenDevice(void);                    // 打开相机

    /*设置、获取参数操作*/
    Status SetTriggerMode(void);                // 设置触发模式
    Status GetTriggerMode(void);
    Status GetExposureTime(void);               // 设置曝光时间
    Status SetExposureTime(void); 
	Status SetRatio(void);
	Status SetThreshold(void);
	Status SetCompensate(void);
    Status GetGain(void);                       // 设置增益
    Status SetGain(void);
    Status GetFrameRate(void);                  // 设置帧率
    Status SetFrameRate(void);
    Status GetTriggerSource(void);              // 设置触发源
    Status SetTriggerSource(void);

    /*相机抓图回调*/
    static void   __stdcall ImageCallBack(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser);

    /*关闭销毁操作*/
    Status CloseDevice(void);                   // 关闭设备
    Status DestroyHandle(void);                 // 销毁设备

/*自己定义变量*/
private:
    /*状态*/
    BOOL  m_bRetStatus;                         // 表示函数返回状态
    BOOL  m_bCreateDevice;                      // 是否创建设备
    BOOL  m_bOpenDevice;                        // 是否打开设备
    BOOL  m_bStartGrabbing;                     // 是否开始抓图
    int   m_nTriggerMode;                       // 触发模式
    int   m_nTriggerSource;                     // 触发源
    MV_SAVE_IAMGE_TYPE   m_nSaveImageType;      // 保存图像格式

    /*设备相关*/
    CMyCamera*      m_pcMyCamera;               // CMyCamera封装了常用接口
    HWND  m_hwndDisplay;                        // 显示句柄
    MV_CC_DEVICE_INFO_LIST m_stDevList;         // 设备信息列表结构体变量，用来存储设备列表

    unsigned char*  m_pBufForSaveImage;         // 用于保存图像的缓存
    unsigned int    m_nBufSizeForSaveImage;

/*按下控件操作*/
public:
    /*初始化*/
    afx_msg void OnBnClickedEnumButton();               // 查找设备
    afx_msg void OnBnClickedOpenButton();               // 打开设备
    afx_msg void OnBnClickedCloseButton();              // 关闭设备
   
    /*图像采集*/
    afx_msg void OnBnClickedContinusModeRadio();        // 连续模式
    afx_msg void OnBnClickedTriggerModeRadio();         // 触发模式
    afx_msg void OnBnClickedStartGrabbingButton();      // 开始采集
    afx_msg void OnBnClickedStopGrabbingButton();       // 结束采集
    afx_msg void OnBnClickedSoftwareTriggerCheck();     // 软触发
    afx_msg void OnBnClickedSoftwareOnceButton();       // 软触发一次
  
    /*图像保存*/
    afx_msg void OnBnClickedSaveBmpButton();            // 保存bmp
    afx_msg void OnBnClickedSaveJpgButton();            // 保存jpg
  
    /*参数设置获取*/
    afx_msg void OnBnClickedGetParameterButton();       // 获取参数
    afx_msg void OnBnClickedSetParameterButton();       // 设置参数

public:    
    afx_msg void OnClose();                             // 右上角退出

public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedSaveJpgButton2();
};
