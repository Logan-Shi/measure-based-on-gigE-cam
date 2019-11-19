
// BasicDemoDlg.h : ͷ�ļ�
// 

#pragma once
#include "afxwin.h" 

#include <stdio.h>

#include "MyCamera.h"

/*���Ⱥ궨��*/
#define MAX_BUF_SIZE            (2560*1920*6)
#define MAX_XML_FILE_SIZE       (1024*1024*3)

/*���������붨��*/
typedef int Status;
#define STATUS_OK               0
#define STATUS_ERROR            -1

// CBasicDemoDlg �Ի���
class CBasicDemoDlg : public CDialog
{
// ����
public:
	CBasicDemoDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BasicDemo_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

/*�ؼ���Ӧ����*/
private:
    /*��ʼ��*/
    CButton m_ctrlOpenButton;                   // ���豸
    CButton m_ctrlCloseButton;                  // �ر��豸

    /*ͼ��ɼ�*/
    CButton m_ctrlContinusModeRadio;            // ����ģʽ             
    CButton m_ctrlTriggerModeRadio;             // ����ģʽ
    CButton m_ctrlStartGrabbingButton;          // ��ʼץͼ          
    CButton m_ctrlStopGrabbingButton;           // ����ץͼ
    CButton m_ctrlSoftwareTriggerCheck;         // ����
    BOOL    m_bSoftWareTriggerCheck;
    CButton m_ctrlSoftwareOnceButton;           // ����һ��

    /*ͼ�񱣴�*/
    CButton m_ctrlSaveBmpButton;                // ����Ϊbmp
    CButton m_ctrlSaveJpgButton;                // ����Ϊjpg

    /*�������û�ȡ*/
    CEdit   m_ctrlExposureEdit;                 // �ع�ʱ��
    double  m_dExposureEdit;
    CEdit   m_ctrlGainEdit;                     // ����
    double  m_dGainEdit;
    CEdit   m_ctrlFrameRateEdit;                // ֡��
    double  m_dFrameRateEdit;   
    CButton m_ctrlGetParameterButton;           // ��ȡ����
    CButton m_ctrlSetParameterButton;           // ���ò���

    /*�豸��ʾ������*/
    CComboBox m_ctrlDeviceCombo;                // ö�ٵ����豸
    int     m_nDeviceCombo;

/*�ڲ�����*/
private:
    /*�ʼʱ�Ĵ��ڳ�ʼ��*/
    Status DisplayWindowInitial(void);          // ��ʾ���ʼ��,�ʼ�ĳ�ʼ�� 

    /*��ť������ʹ��*/
    Status EnableWindowWhenClose(void);         // �ر�ʱ�İ�ť��ɫ���൱���ʼ��ʼ��
    Status EnableWindowWhenOpenNotStart(void);  // ���´��豸��ťʱ�İ�ť��ɫ  
    Status EnableWindowWhenStart(void);         // ���¿�ʼ�ɼ�ʱ�İ�ť��ɫ 
    Status EnableWindowWhenStop(void);          // ���½����ɼ�ʱ�İ�ť��ɫ
    
    /*��ʼ���������*/
    Status OpenDevice(void);                    // �����

    /*���á���ȡ��������*/
    Status SetTriggerMode(void);                // ���ô���ģʽ
    Status GetTriggerMode(void);
    Status GetExposureTime(void);               // �����ع�ʱ��
    Status SetExposureTime(void); 
    Status GetGain(void);                       // ��������
    Status SetGain(void);
    Status GetFrameRate(void);                  // ����֡��
    Status SetFrameRate(void);
    Status GetTriggerSource(void);              // ���ô���Դ
    Status SetTriggerSource(void);

    /*���ץͼ�ص�*/
    static void   __stdcall ImageCallBack(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser);

    /*�ر����ٲ���*/
    Status CloseDevice(void);                   // �ر��豸
    Status DestroyHandle(void);                 // �����豸

/*�Լ��������*/
private:
    /*״̬*/
    BOOL  m_bRetStatus;                         // ��ʾ��������״̬
    BOOL  m_bCreateDevice;                      // �Ƿ񴴽��豸
    BOOL  m_bOpenDevice;                        // �Ƿ���豸
    BOOL  m_bStartGrabbing;                     // �Ƿ�ʼץͼ
    int   m_nTriggerMode;                       // ����ģʽ
    int   m_nTriggerSource;                     // ����Դ
    MV_SAVE_IAMGE_TYPE   m_nSaveImageType;      // ����ͼ���ʽ

    /*�豸���*/
    CMyCamera*      m_pcMyCamera;               // CMyCamera��װ�˳��ýӿ�
    HWND  m_hwndDisplay;                        // ��ʾ���
    MV_CC_DEVICE_INFO_LIST m_stDevList;         // �豸��Ϣ�б�ṹ������������洢�豸�б�

    unsigned char*  m_pBufForSaveImage;         // ���ڱ���ͼ��Ļ���
    unsigned int    m_nBufSizeForSaveImage;

/*���¿ؼ�����*/
public:
    /*��ʼ��*/
    afx_msg void OnBnClickedEnumButton();               // �����豸
    afx_msg void OnBnClickedOpenButton();               // ���豸
    afx_msg void OnBnClickedCloseButton();              // �ر��豸
   
    /*ͼ��ɼ�*/
    afx_msg void OnBnClickedContinusModeRadio();        // ����ģʽ
    afx_msg void OnBnClickedTriggerModeRadio();         // ����ģʽ
    afx_msg void OnBnClickedStartGrabbingButton();      // ��ʼ�ɼ�
    afx_msg void OnBnClickedStopGrabbingButton();       // �����ɼ�
    afx_msg void OnBnClickedSoftwareTriggerCheck();     // ����
    afx_msg void OnBnClickedSoftwareOnceButton();       // ����һ��
  
    /*ͼ�񱣴�*/
    afx_msg void OnBnClickedSaveBmpButton();            // ����bmp
    afx_msg void OnBnClickedSaveJpgButton();            // ����jpg
  
    /*�������û�ȡ*/
    afx_msg void OnBnClickedGetParameterButton();       // ��ȡ����
    afx_msg void OnBnClickedSetParameterButton();       // ���ò���

public:    
    afx_msg void OnClose();                             // ���Ͻ��˳�


public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);



};
