
// BasicDemoDlg.cpp : ʵ���ļ�
// 

#include "stdafx.h"
#include "BasicDemoCallback.h"
#include "BasicDemoCallbackDlg.h"
#include "MvGigEDevice.h"

#include <iostream>
#include <fstream>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#ifdef _DEBUG
#pragma comment(lib,"opencv_world341d.lib")
#else
#pragma comment(lib,"opencv_world341.lib")
#endif
#define NUM 3
using namespace cv;
using namespace std;

double g_ratio = 72.3;
int g_threshold = 100;
int g_imgNumber = 1;
int g_measNumber = 1;

/// Global Variables
const int alpha_slider_max = 255;
int alpha_slider;

double g_output[NUM][6];

double g_sum[6];
double avr[6];
double stdvar[6];
/// Matrices to store images
Mat src;

/**
 * @function on_trackbar
 * @brief Callback for trackbar
 */
void on_trackbar(int, void*)
{
	Rect area(src.cols / 8, src.rows / 8, src.cols / 1.25, src.rows / 1.25);
	cv::Mat l_src = src(area);
	/* step 1: resize for calc efficiency */
	double scale = 0.2;
	cv::resize(l_src, l_src, cv::Size(l_src.cols*scale, l_src.rows*scale), 0, 0, cv::INTER_NEAREST);

	/* step 2: gaussian filter */
	cv::GaussianBlur(l_src, l_src, Size(9, 9), 2, 2);//��˹����
	Mat cimg;
	cv::cvtColor(l_src, cimg, COLOR_RGB2GRAY);

	/* step 3: get bi-level img */
	int temp = g_threshold;
	temp = alpha_slider;
	cv::threshold(cimg, cimg, temp, 255, CV_THRESH_BINARY);
	cv::imshow("threshold", cimg);

	/* step 4: edge extraction */
	int canny_threshold = 100;
	double canny_radio = 2;
	cv::Canny(cimg, cimg, canny_threshold, canny_radio * canny_threshold, 5);
	cv::imshow("canny", cimg);
}
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CBasicDemoDlg �Ի���

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
	DDX_Control(pDX, IDC_FRAME_RATE_EDIT2, m_ctrlFrameRateEdit2);
	DDX_Control(pDX, IDC_FRAME_RATE_EDIT3, m_ctrlFrameRateEdit3);
	DDX_Control(pDX, IDC_GET_PARAMETER_BUTTON, m_ctrlGetParameterButton);
	DDX_Control(pDX, IDC_SET_PARAMETER_BUTTON, m_ctrlSetParameterButton);
	DDX_Control(pDX, IDC_CONTINUS_MODE_RADIO, m_ctrlContinusModeRadio);
	DDX_Control(pDX, IDC_TRIGGER_MODE_RADIO, m_ctrlTriggerModeRadio);
	DDX_Control(pDX, IDC_DEVICE_COMBO, m_ctrlDeviceCombo);
	DDX_CBIndex(pDX, IDC_DEVICE_COMBO, m_nDeviceCombo);
	DDX_Text(pDX, IDC_EXPOSURE_EDIT, m_dExposureEdit);
	DDX_Text(pDX, IDC_GAIN_EDIT, m_dGainEdit);
	DDX_Text(pDX, IDC_FRAME_RATE_EDIT, m_dFrameRateEdit);
	DDX_Text(pDX, IDC_FRAME_RATE_EDIT2, m_dFrameRateEdit2);
	DDX_Text(pDX, IDC_FRAME_RATE_EDIT3, m_dFrameRateEdit3);
	DDX_Text(pDX, IDC_FRAME_RATE_EDIT4, m_dFrameRateEdit4);
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
	ON_BN_CLICKED(IDC_SAVE_JPG_BUTTON2, &CBasicDemoDlg::OnBnClickedSaveJpgButton2)
END_MESSAGE_MAP()


// CBasicDemoDlg ��Ϣ�������

BOOL CBasicDemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	DisplayWindowInitial();     // ��ʾ���ʼ��

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBasicDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// ���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
// ��ʾ��
HCURSOR CBasicDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/*�ر��豸ʱ�Ĵ�����ʾ*/
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

/*���豸������ʼץͼ*/
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

// ���¿�ʼ�ɼ���ťʱ�İ�ť��ɫ  
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

// ���½����ɼ�ʱ�İ�ť��ɫ
Status CBasicDemoDlg::EnableWindowWhenStop(void)
{
	m_ctrlStopGrabbingButton.EnableWindow(FALSE);
	m_ctrlStartGrabbingButton.EnableWindow(TRUE);
	m_ctrlSaveBmpButton.EnableWindow(FALSE);
	m_ctrlSaveJpgButton.EnableWindow(FALSE);
	m_ctrlSoftwareOnceButton.EnableWindow(FALSE);
	return STATUS_OK;
}

/*�ʼʱ�Ĵ��ڳ�ʼ��*/
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
#if _DEBUG
	/// Read image ( same size, same type )
	src = imread("001.jpg");

	Rect area(src.cols / 5, src.rows / 6, src.cols / 1.5, src.rows / 1.5);
	cv::Mat l_src = src(area);
	/* step 1: resize for calc efficiency */
	double scale = 0.2;
	cv::resize(l_src, l_src, cv::Size(l_src.cols*scale, l_src.rows*scale), 0, 0, cv::INTER_NEAREST);

	/* step 2: gaussian filter */
	GaussianBlur(l_src, l_src, Size(9, 9), 2, 2);//��˹����
	Mat cimg;
	cvtColor(l_src, cimg, COLOR_RGB2GRAY);

	/* step 3: get bi-level img */
	double l_ratio = 72.3;
	int temp = 190;
	temp = 190;
	//temp = pBasicDemo->m_pcMyCamera->_threshold;
	//l_ratio = pBasicDemo->m_pcMyCamera->_ratio;
	threshold(cimg, cimg, temp, 255, CV_THRESH_BINARY);
	imshow("threshold", cimg);

	/* step 4: edge extraction */
	int canny_threshold = 100;
	double canny_radio = 2;
	Canny(cimg, cimg, canny_threshold, canny_radio * canny_threshold, 5);
	imshow("canny", cimg);

	double sub_pixel = 2 / scale;
	cv::resize(cimg, cimg, cv::Size(cimg.cols*sub_pixel, cimg.rows*sub_pixel), 0, 0, cv::INTER_NEAREST);

	/* step 5: circle detection */
	vector<Vec3f> circles;
	HoughCircles(cimg, circles, HOUGH_GRADIENT, 1, cimg.rows / 8, canny_threshold * 2, 20, cimg.rows / 8, cimg.rows / 2.5);
	int l_radius = 0;
	Point center1(cvRound(circles[0][0] / sub_pixel), cvRound(circles[0][1] / sub_pixel));
	for (size_t i = 0; i < circles.size(); i++)
	{
		Point center(cvRound(circles[i][0] / sub_pixel), cvRound(circles[i][1] / sub_pixel));
		int radius = cvRound(circles[i][2] / sub_pixel);
		//����Բ��
		circle(l_src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
		//����Բ����
		circle(l_src, center, radius, Scalar(155, 50, 255), 3, 8, 0);
		l_radius = radius;
	}

	/* step 6: line detection */
	vector<Vec4i> lines;
	HoughLinesP(cimg, lines, 0.5, CV_PI / 360, 100, cimg.rows / 1.5, cimg.rows / 3);
	Vec4i line1 = lines[0];
	Vec4i line2;
	Vec4i line3;
	double k1 = (double)(line1[1] - line1[3]) / (double)(line1[0] - line1[2]);
	double k2 = 0, k3 = 0, length1 = 0, length2 = 0, length3 = 0, lengthi;
	length1 = sqrt(powf((lines[1][0] - lines[1][2]), 2) + powf((lines[1][1] - lines[1][3]), 2));
	for (size_t i = 0; i < lines.size(); i++)
	{
		Vec4i l = lines[i];
		line(l_src, Point(l[0] / sub_pixel, l[1] / sub_pixel), Point(l[2] / sub_pixel, l[3] / sub_pixel), Scalar(0, 0, 255), 3, CV_AA);

		/*����������*/
		double ki = (double)(lines[i][1] - lines[i][3]) / (double)(lines[i][0] - lines[i][2]);
		lengthi = sqrt(powf((lines[i][0] - lines[i][2]), 2) + powf((lines[i][1] - lines[i][3]), 2));
		/*�޸��˲�����*/
		if (abs(abs(ki / k1) - 1) < 0.1 && length1 < lengthi)
		{
			k1 = ki;
			length1 = lengthi;
			line1 = lines[i];
		}
		else if ((abs(abs(ki / k2) - 1) < 0.1 || k2 == 0) && length2 < lengthi && abs(abs(ki / k1) - 1) > 0.1)
		{
			k2 = ki;
			length2 = lengthi;
			line2 = lines[i];
		}
		else if ((abs(abs(ki / k3) - 1) < 0.1 || k3 == 0) && length3 < lengthi && abs(abs(ki / k1) - 1) > 0.1 && abs(abs(ki / k2) - 1) > 0.1)
		{
			k3 = ki;
			length3 = lengthi;
			line3 = lines[i];
		}
		/*���Ե�ѡ��б�ʲ�ͬ������ֱ�ߣ���Ҫ�Ż�*/
		//if (abs(ki / k1) < 0.9 || abs(ki / k1) > 1.1 && k2 == 0)
		//{
		//	k2 = ki;
		//	line2 = lines[i];
		//}
		//else if ((abs(ki / k1) < 0.9 || abs(ki / k1) > 1.1) && (abs(ki / k2) < 0.9 || abs(ki / k2) > 1.1))
		//{
		//	k3 = ki;
		//	line3 = lines[i];
		//}
	}

	/*���Ƶõ���������*/
	line(l_src, Point(line1[0] / sub_pixel, line1[1] / sub_pixel), Point(line1[2] / sub_pixel, line1[3] / sub_pixel), Scalar(255, 0, 0), 5, CV_AA);
	line(l_src, Point(line2[0] / sub_pixel, line2[1] / sub_pixel), Point(line2[2] / sub_pixel, line2[3] / sub_pixel), Scalar(255, 0, 0), 5, CV_AA);
	line(l_src, Point(line3[0] / sub_pixel, line3[1] / sub_pixel), Point(line3[2] / sub_pixel, line3[3] / sub_pixel), Scalar(255, 0, 0), 5, CV_AA);
	//cout << "ֱ��1����(" << line1[0] << "," << line1[1] << ") , (" << line1[2] << "," << line1[3] << ");  k=" << k1 << endl;
	//cout << "ֱ��2����(" << line2[0] << "," << line2[1] << ") , (" << line2[2] << "," << line2[3] << ");  k=" << k2 << endl;
	//cout << "ֱ��3����(" << line3[0] << "," << line3[1] << ") , (" << line3[2] << "," << line3[3] << ");  k=" << k3 << endl;

	/*���ƽ���*/
	Point2f p1, p2, p3;
	p1.x = (k1*line1[0] - line1[1] - k2 * line2[0] + line2[1]) / (k1 - k2) / sub_pixel;
	p1.y = (k1*k2*(line1[0] - line2[0]) + k1 * line2[1] - k2 * line1[1]) / (k1 - k2) / sub_pixel;
	p2.x = (k1*line1[0] - line1[1] - k3 * line3[0] + line3[1]) / (k1 - k3) / sub_pixel;
	p2.y = (k1*k3*(line1[0] - line3[0]) + k1 * line3[1] - k3 * line1[1]) / (k1 - k3) / sub_pixel;
	p3.x = (k2*line2[0] - line2[1] - k3 * line3[0] + line3[1]) / (k2 - k3) / sub_pixel;
	p3.y = (k2*k3*(line2[0] - line3[0]) + k2 * line3[1] - k3 * line2[1]) / (k2 - k3) / sub_pixel;
	circle(l_src, p1, 6, Scalar(255, 0, 0), 3);
	circle(l_src, p2, 6, Scalar(255, 0, 0), 3);
	circle(l_src, p3, 6, Scalar(255, 0, 0), 3);
	Point ha, hb, hc;
	double h1, h2, h3;
	ha.x = (circles[0][1] - line1[1] + circles[0][0] / k1 + k1 * line1[0]) / (k1 + 1 / k1);
	ha.y = (circles[0][0] - line1[0] + k1 * circles[0][1] + line1[1] / k1) / (k1 + 1 / k1);
	h1 = sqrt(powf((circles[0][0] - ha.x), 2) + powf((circles[0][1] - ha.y), 2)) / sub_pixel;
	line(l_src, Point(circles[0][0] / sub_pixel, circles[0][1] / sub_pixel), Point(ha.x / sub_pixel, ha.y / sub_pixel), Scalar(255, 0, 0), 5, CV_AA);

	hb.x = (circles[0][1] - line2[1] + circles[0][0] / k2 + k2 * line2[0]) / (k2 + 1 / k2);
	hb.y = (circles[0][0] - line2[0] + k2 * circles[0][1] + line2[1] / k2) / (k2 + 1 / k2);
	h2 = sqrt(powf((circles[0][0] - hb.x), 2) + powf((circles[0][1] - hb.y), 2)) / sub_pixel;
	line(l_src, Point(circles[0][0] / sub_pixel, circles[0][1] / sub_pixel), Point(hb.x / sub_pixel, hb.y / sub_pixel), Scalar(255, 0, 0), 5, CV_AA);

	hc.x = (circles[0][1] - line3[1] + circles[0][0] / k3 + k3 * line3[0]) / (k3 + 1 / k3);
	hc.y = (circles[0][0] - line3[0] + k3 * circles[0][1] + line3[1] / k3) / (k3 + 1 / k3);
	h3 = sqrt(powf((circles[0][0] - hc.x), 2) + powf((circles[0][1] - hc.y), 2)) / sub_pixel;
	line(l_src, Point(circles[0][0] / sub_pixel, circles[0][1] / sub_pixel), Point(hc.x / sub_pixel, hc.y / sub_pixel), Scalar(255, 0, 0), 5, CV_AA);

	cv::imshow("result", l_src);
	/* step 7: write output */

	ofstream fout;
	if (char status = cv::waitKey(0))
	{
		switch (status)
		{
		case 's':
			fout.open("result.txt", ios::out | ios::app);
			if (!fout)
			{
				std::cout << "openning failed!!!" << '\n';
			}

			fout << "measurement: " << '\n';
			fout << "--------------------------------" << '\n';
			fout << "detect " << circles.size() << " circle(s)" << '\n';
			fout << "detected " << lines.size() << " lines" << '\n';

			/*���������߳��Լ�Բ�ĵ��ߵľ���*/
			double length12, length13, length23;

			length12 = sqrt(powf((p1.x - p2.x), 2) + powf((p1.y - p2.y), 2)) / sub_pixel;
			length13 = sqrt(powf((p1.x - p3.x), 2) + powf((p1.y - p3.y), 2)) / sub_pixel;
			length23 = sqrt(powf((p3.x - p2.x), 2) + powf((p3.y - p2.y), 2)) / sub_pixel;
			//double A, B, C;
			/*h1*/
			//A = (line1[1] - line1[3]) / sub_pixel;
			//B = (line1[2] - line1[0]) / sub_pixel;
			//C = (line1[0] * line1[3] - line1[1] * line1[2]) / sub_pixel / sub_pixel;
			//h1 = ((double)abs(A*center1.x + B * center1.y +C)) / ((double)sqrt(A*A + B * B)) ;
			/*h2*/
			//A = line2[1] - line2[3];
			//B = line2[2] - line2[0];
			//C = line2[0] * line2[3] - line2[1] * line2[2];
			//h2 = ((float )abs(A*circles[0][0] + B * circles[0][1]+C)) / ((float)sqrtf(A*A + B * B)) / sub_pixel;
			/*h3*/
			//A = line3[1] - line3[3];
			//B = line3[2] - line3[0];
			//C = line3[0] * line3[3] - line3[1] * line3[2];
			//h3 = ((double)abs(A*circles[0][0] + B * circles[0][1]+C)) / ((double)sqrtf(A*A + B * B)) / sub_pixel;

			/*���������*/
			fout << "edge length is respectfully:" << length12 / l_ratio * 50 << "mm, " << length13 / l_ratio * 50 << "mm, " << length23 / l_ratio * 50 << "mm." << '\n';
			fout << "circle center to edges height are: " << h1 / l_ratio * 50 << "mm, " << h2 / l_ratio * 50 << "mm, " << h3 / l_ratio * 50 << "mm." << '\n';
			fout << "radius is: " << l_radius / l_ratio * 5 << "mm" << '\n';
			fout << "measurement finished" << '\n';
			fout << "--------------------------------" << '\n';
			fout << '\n';
			fout.close();
			cv::destroyAllWindows();
			//pBasicDemo->MessageBox(TEXT("�����Ѽ�¼"), TEXT("�ɹ�"), MB_OK | MB_ICONWARNING);
			break;
		default:
			cv::destroyAllWindows();
			//pBasicDemo->MessageBox(TEXT("����δ��¼"), TEXT("�ɹ�"), MB_OK | MB_ICONWARNING);
			break;
		}
	}
#else
#endif
	return STATUS_OK;
}

// ���豸
Status CBasicDemoDlg::OpenDevice(void)
{
	if (TRUE == m_bOpenDevice)
	{
		return STATUS_ERROR;
	}
	UpdateData(TRUE);
	if (TRUE == m_bCreateDevice)
	{
		return STATUS_ERROR;
	}
	int nIndex = m_nDeviceCombo;
	if ((nIndex < 0) | (nIndex >= MV_MAX_DEVICE_NUM))
	{
		return STATUS_ERROR;
	}

	// ���豸��Ϣ�����豸ʵ��
	if (NULL == m_stDevList.pDeviceInfo[nIndex])
	{
		return STATUS_ERROR;
	}

	if (NULL != m_pcMyCamera)
	{
		return STATUS_ERROR;
	}

	m_pcMyCamera = new CMyCamera;
	m_pcMyCamera->_threshold = 100;
	/*�����豸ʧ��*/
	if (NULL == m_pcMyCamera)
	{
		m_bRetStatus = STATUS_ERROR;
		return STATUS_ERROR;
	}

	int nRet = m_pcMyCamera->Open(m_stDevList.pDeviceInfo[nIndex]);
	/*�����豸ʧ��*/
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

// �ر��豸
Status CBasicDemoDlg::CloseDevice(void)
{
	cv::destroyAllWindows();
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
	m_nBufSizeForSaveImage = 0;

	m_bOpenDevice = FALSE;
	m_bStartGrabbing = FALSE;
	return STATUS_OK;
}

// �����豸
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

// ��ȡ����ģʽ
Status CBasicDemoDlg::GetTriggerMode(void)
{
	unsigned int nEnumValue = 0;

	int nRet = m_pcMyCamera->GetEnumValue("TriggerMode", &nEnumValue);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}

	m_nTriggerMode = nEnumValue;
	if (MV_TRIGGER_MODE_ON == m_nTriggerMode)
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

// ���ô���ģʽ
Status CBasicDemoDlg::SetTriggerMode(void)
{
	int nRet = m_pcMyCamera->SetEnumValue("TriggerMode", m_nTriggerMode);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}

	return STATUS_OK;
}

// ��ȡ�ع�ʱ��
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

// �����ع�ʱ��
Status CBasicDemoDlg::SetExposureTime(void)
{
	float  dfFloatValue = 0.0;

	/*�����������ع�ģʽ���������ع�ʱ����Ч*/
	int nRet = m_pcMyCamera->SetEnumValue("ExposureMode", 0);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}

	nRet = m_pcMyCamera->SetEnumValue("ExposureAuto", 0);
	if (MV_OK != nRet)
	{
		// return STATUS_ERROR;      // ����ʧ���п����ǿ��˴���ģʽ���������践��
	}

	nRet = m_pcMyCamera->SetFloatValue("ExposureTime", m_dExposureEdit);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}

	UpdateData(FALSE);

	return STATUS_OK;
}

Status CBasicDemoDlg::SetThreshold(void)
{
	m_pcMyCamera->_threshold = m_dFrameRateEdit3;
	UpdateData(FALSE);

	return STATUS_OK;
}

Status CBasicDemoDlg::SetRatio(void)
{
	m_pcMyCamera->_ratio = m_dFrameRateEdit2;
	UpdateData(FALSE);

	return STATUS_OK;
}

Status CBasicDemoDlg::SetCompensate(void)
{
	m_pcMyCamera->_compensate = m_dFrameRateEdit4;
	UpdateData(FALSE);

	return STATUS_OK;
}

// ��ȡ����
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

// ��������
Status CBasicDemoDlg::SetGain(void)
{
	int nRet = m_pcMyCamera->SetEnumValue("GainAuto", 0);   // ��������ǰ�Ȱ��Զ�����رգ�ʧ�����践��

	nRet = m_pcMyCamera->SetFloatValue("Gain", m_dGainEdit);
	if (MV_OK != nRet)
	{
		return STATUS_ERROR;
	}

	UpdateData(FALSE);

	return STATUS_OK;
}

// ��ȡ֡��
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

// ����֡��
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

// ��ȡ����Դ
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

// ���ô���Դ
Status CBasicDemoDlg::SetTriggerSource(void)
{
	UpdateData(TRUE);

	if (m_bSoftWareTriggerCheck)
	{
		m_nTriggerSource = MV_TRIGGER_SOURCE_SOFTWARE;
		int nRet = m_pcMyCamera->SetEnumValue("TriggerSource", m_nTriggerSource);
		if (MV_OK != nRet)
		{
			MessageBox(TEXT("��������ʧ��"), TEXT("����"), MB_OK | MB_ICONWARNING);
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
			MessageBox(TEXT("����Ӳ����ʧ��"), TEXT("����"), MB_OK | MB_ICONWARNING);
			return STATUS_ERROR;
		}
		m_ctrlSoftwareOnceButton.EnableWindow(FALSE);
	}

	return STATUS_OK;
}

// ���ץͼ�ص�, ��Ҫ������ȡͼƬ
void   __stdcall CBasicDemoDlg::ImageCallBack(unsigned char* pData, MV_FRAME_OUT_INFO_EX* pFrameInfo, void* pUser)
{
	if (pFrameInfo)
	{
		CBasicDemoDlg *pBasicDemo = (CBasicDemoDlg*)pUser;
		if (MV_Image_Undefined == pBasicDemo->m_nSaveImageType || NULL == pData)
		{
			return;
		}
		// ���ö�Ӧ���������

		//pBasicDemo->MessageBox(TEXT("imageCallBack"));

		// ���ڵ�һ�α���ͼ��ʱ���뻺�棬�� CloseDevice ʱ�ͷ�
		if (NULL == pBasicDemo->m_pBufForSaveImage)
		{
			// BMPͼƬ��С��width * height * 3 + 2048(Ԥ��BMPͷ��С)
			pBasicDemo->m_nBufSizeForSaveImage = pFrameInfo->nWidth * pFrameInfo->nHeight * 3 + 2048;

			pBasicDemo->m_pBufForSaveImage = (unsigned char*)malloc(pBasicDemo->m_nBufSizeForSaveImage);
			if (NULL == pBasicDemo->m_pBufForSaveImage)
			{
				return;
			}
		}
		/* step 0: read img data */
		src = cv::Mat(pFrameInfo->nHeight, pFrameInfo->nWidth, CV_8UC3, pData);

		MV_SAVE_IMAGE_PARAM_EX stParam = { 0 };
		stParam.enImageType = pBasicDemo->m_nSaveImageType; // ��Ҫ�����ͼ������
		stParam.enPixelType = pFrameInfo->enPixelType;  // �����Ӧ�����ظ�ʽ
		stParam.nBufferSize = pBasicDemo->m_nBufSizeForSaveImage;  // �洢�ڵ�Ĵ�С
		stParam.nWidth = pFrameInfo->nWidth;         // �����Ӧ�Ŀ�
		stParam.nHeight = pFrameInfo->nHeight;          // �����Ӧ�ĸ�
		stParam.nDataLen = pFrameInfo->nFrameLen;
		stParam.pData = pData;
		stParam.pImageBuffer = pBasicDemo->m_pBufForSaveImage;
		stParam.nJpgQuality = 80;

		int nRet = pBasicDemo->m_pcMyCamera->SaveImage(&stParam);
		if (MV_OK != nRet)
		{
		}
		char pImageName[32] = { 0 };
		if (NULL == pImageName)
		{
			return;
		}
		if (MV_Image_Bmp == stParam.enImageType)
		{
			pBasicDemo->m_nSaveImageType = MV_Image_Undefined;
			Rect area(src.cols / 8, src.rows / 8, src.cols / 1.25, src.rows / 1.25);
			cv::Mat l_src = src(area);
			/* step 1: resize for calc efficiency */
			double scale = 0.2;
			cv::resize(l_src, l_src, cv::Size(l_src.cols*scale, l_src.rows*scale), 0, 0, cv::INTER_NEAREST);

			/* step 2: gaussian filter */
			cv::GaussianBlur(l_src, l_src, Size(9, 9), 2, 2);//��˹����
			Mat cimg;
			cv::cvtColor(l_src, cimg, COLOR_RGB2GRAY);

			/* step 3: get bi-level img */
			double l_ratio = 72.3;
			int temp = 190;
			double l_compensate = 1.0;
			temp = pBasicDemo->m_pcMyCamera->_threshold;
			l_ratio = pBasicDemo->m_pcMyCamera->_ratio;
			l_compensate = pBasicDemo->m_pcMyCamera->_compensate;
			cv::threshold(cimg, cimg, temp, 255, CV_THRESH_BINARY);
			cv::imshow("threshold", cimg);

			/* step 4: edge extraction */
			int canny_threshold = 100;
			double canny_radio = 2;
			cv::Canny(cimg, cimg, canny_threshold, canny_radio * canny_threshold, 5);
			cv::imshow("canny", cimg);

			double sub_pixel = 2 / scale;
			cv::resize(cimg, cimg, cv::Size(cimg.cols*sub_pixel, cimg.rows*sub_pixel), 0, 0, cv::INTER_NEAREST);

			/* step 5: circle detection */
			vector<Vec3f> circles;
			cv::HoughCircles(cimg, circles, HOUGH_GRADIENT, 1, cimg.rows / 8, canny_threshold * 2, 20, cimg.rows / 8, cimg.rows / 4);
			int l_radius = 0;
			Point center1(cvRound(circles[0][0] / sub_pixel), cvRound(circles[0][1] / sub_pixel));
			Point center(cvRound(circles[0][0] / sub_pixel), cvRound(circles[0][1] / sub_pixel));
			int radius = cvRound(circles[0][2] / sub_pixel);
			//����Բ��
			cv::circle(l_src, center, 3, Scalar(0, 255, 0), -1, 8, 0);
			//����Բ����
			cv::circle(l_src, center, radius, Scalar(155, 50, 255), 0.1, 8, 0);
			l_radius = radius;

			/* step 6: line detection */
			vector<Vec4i> lines;
			cv::HoughLinesP(cimg, lines, 0.5, CV_PI / 360/2, 50, cimg.rows / 1.5, cimg.rows / 6);
			Vec4i line1 = lines[0];
			Vec4i line2;
			Vec4i line3;
			double k1 = (double)(line1[1] - line1[3]) / (double)(line1[0] - line1[2]);
			double k2 = 0, k3 = 0, length1 = 0, length2 = 0, length3 = 0, lengthi;
			length1 = sqrt(powf((lines[1][0] - lines[1][2]), 2) + powf((lines[1][1] - lines[1][3]), 2));
			for (size_t i = 0; i < lines.size(); i++)
			{
				Vec4i l = lines[i];
				cv::line(l_src, Point(l[0] / sub_pixel, l[1] / sub_pixel), Point(l[2] / sub_pixel, l[3] / sub_pixel), Scalar(0, 0, 255), 3, CV_AA);

				/*����������*/
				double ki = (double)(lines[i][1] - lines[i][3]) / (double)(lines[i][0] - lines[i][2]);
				lengthi = sqrt(powf((lines[i][0] - lines[i][2]), 2) + powf((lines[i][1] - lines[i][3]), 2));
				/*�޸��˲�����*/
				if (abs(abs(ki / k1) - 1) < 0.1 && length1 < lengthi)
				{
					k1 = ki;
					length1 = lengthi;
					line1 = lines[i];
				}
				else if ((abs(abs(ki / k2) - 1) < 0.1 || k2 == 0) && length2 < lengthi && abs(abs(ki / k1) - 1) > 0.1)
				{
					k2 = ki;
					length2 = lengthi;
					line2 = lines[i];
				}
				else if ((abs(abs(ki / k3) - 1) < 0.1 || k3 == 0) && length3 < lengthi && abs(abs(ki / k1) - 1) > 0.1 && abs(abs(ki / k2) - 1) > 0.1)
				{
					k3 = ki;
					length3 = lengthi;
					line3 = lines[i];
				}
				/*���Ե�ѡ��б�ʲ�ͬ������ֱ�ߣ���Ҫ�Ż�*/
				//if (abs(ki / k1) < 0.9 || abs(ki / k1) > 1.1 && k2 == 0)
				//{
				//	k2 = ki;
				//	line2 = lines[i];
				//}
				//else if ((abs(ki / k1) < 0.9 || abs(ki / k1) > 1.1) && (abs(ki / k2) < 0.9 || abs(ki / k2) > 1.1))
				//{
				//	k3 = ki;
				//	line3 = lines[i];
				//}
			}

			/*���Ƶõ���������*/
			cv::line(l_src, Point(line1[0] / sub_pixel, line1[1] / sub_pixel), Point(line1[2] / sub_pixel, line1[3] / sub_pixel), Scalar(255, 0, 0), 1, CV_AA);
			cv::line(l_src, Point(line2[0] / sub_pixel, line2[1] / sub_pixel), Point(line2[2] / sub_pixel, line2[3] / sub_pixel), Scalar(255, 0, 0), 1, CV_AA);
			cv::line(l_src, Point(line3[0] / sub_pixel, line3[1] / sub_pixel), Point(line3[2] / sub_pixel, line3[3] / sub_pixel), Scalar(255, 0, 0), 1, CV_AA);

			/*���ƽ���*/
			Point2f p1, p2, p3;
			p1.x = (k1*line1[0] - line1[1] - k2 * line2[0] + line2[1]) / (k1 - k2) / sub_pixel;
			p1.y = (k1*k2*(line1[0] - line2[0]) + k1 * line2[1] - k2 * line1[1]) / (k1 - k2) / sub_pixel;
			p2.x = (k1*line1[0] - line1[1] - k3 * line3[0] + line3[1]) / (k1 - k3) / sub_pixel;
			p2.y = (k1*k3*(line1[0] - line3[0]) + k1 * line3[1] - k3 * line1[1]) / (k1 - k3) / sub_pixel;
			p3.x = (k2*line2[0] - line2[1] - k3 * line3[0] + line3[1]) / (k2 - k3) / sub_pixel;
			p3.y = (k2*k3*(line2[0] - line3[0]) + k2 * line3[1] - k3 * line2[1]) / (k2 - k3) / sub_pixel;
			cv::circle(l_src, p1, 6, Scalar(255, 0, 0), 0.1);
			cv::circle(l_src, p2, 6, Scalar(255, 0, 0), 0.1);
			cv::circle(l_src, p3, 6, Scalar(255, 0, 0), 0.1);
			Point ha, hb, hc;
			double h1, h2, h3;
			ha.x = (circles[0][1] - line1[1] + circles[0][0] / k1 + k1 * line1[0]) / (k1 + 1 / k1) / sub_pixel;
			ha.y = (circles[0][0] - line1[0] + k1 * circles[0][1] + line1[1] / k1) / (k1 + 1 / k1) / sub_pixel;
			h1 = sqrt(powf((circles[0][0] / sub_pixel - ha.x), 2) + powf((circles[0][1] / sub_pixel - ha.y), 2)) / 10;
			cv::line(l_src, Point(circles[0][0] / sub_pixel, circles[0][1] / sub_pixel), Point(ha.x, ha.y), Scalar(255, 0, 0), 1, CV_AA);

			hb.x = (circles[0][1] - line2[1] + circles[0][0] / k2 + k2 * line2[0]) / (k2 + 1 / k2);
			hb.y = (circles[0][0] - line2[0] + k2 * circles[0][1] + line2[1] / k2) / (k2 + 1 / k2);
			h2 = sqrt(powf((circles[0][0] - hb.x), 2) + powf((circles[0][1] - hb.y), 2)) / sub_pixel / 10;
			cv::line(l_src, Point(circles[0][0] / sub_pixel, circles[0][1] / sub_pixel), Point(hb.x / sub_pixel, hb.y / sub_pixel), Scalar(255, 0, 0), 1, CV_AA);

			hc.x = (circles[0][1] - line3[1] + circles[0][0] / k3 + k3 * line3[0]) / (k3 + 1 / k3);
			hc.y = (circles[0][0] - line3[0] + k3 * circles[0][1] + line3[1] / k3) / (k3 + 1 / k3);
			h3 = sqrt(powf((circles[0][0] - hc.x), 2) + powf((circles[0][1] - hc.y), 2)) / sub_pixel / 10;
			cv::line(l_src, Point(circles[0][0] / sub_pixel, circles[0][1] / sub_pixel), Point(hc.x / sub_pixel, hc.y / sub_pixel), Scalar(255, 0, 0), 1, CV_AA);

			/*���������߳��Լ�Բ�ĵ��ߵľ���*/
			double length12, length13, length23;

			length12 = sqrt(powf((p1.x - p2.x), 2) + powf((p1.y - p2.y), 2)) / sub_pixel;
			length13 = sqrt(powf((p1.x - p3.x), 2) + powf((p1.y - p3.y), 2)) / sub_pixel;
			length23 = sqrt(powf((p3.x - p2.x), 2) + powf((p3.y - p2.y), 2)) / sub_pixel;

			cv::imshow("result", l_src);
			
			/* step 7: write output */

			double result[4][2];
			for (size_t i = 0; i < 4; i++)
			{
				for (size_t j = 0; j < 2; j++)
					result[i][j] = 0;
			}
			/*Բ�ġ��뾶*/
			//result[0][0] = circles[0][0] / sub_pixel;
			//result[0][1] = circles[0][1] / sub_pixel;
			result[0][0] = circles[0][2] / sub_pixel / 10;
			/*�߶�����123�д�С����01������Ϊ�����Լ�Բ�ĵ���*/
			if (length12 < length13 && length12 < length23)
			{
				result[1][0] = length12;
				result[1][1] = h1;
				if (length13 < length23)
				{
					result[2][0] = length13;
					result[2][1] = h2;
					result[3][0] = length23;
					result[3][1] = h3;
				}
				else
				{
					result[3][0] = length13;
					result[3][1] = h2;
					result[2][0] = length23;
					result[2][1] = h3;
				}
			}
			else if (length13 < length12 && length13 < length23)
			{
				result[1][0] = length13;
				result[1][1] = h2;
				if (length12 < length23)
				{
					result[2][0] = length12;
					result[2][1] = h1;
					result[3][0] = length23;
					result[3][1] = h3;
				}
				else
				{
					result[3][0] = length12;
					result[3][1] = h1;
					result[2][0] = length23;
					result[2][1] = h3;
				}
			}
			else {
				result[1][0] = length23;
				result[1][1] = h3;
				if (length12 < length13)
				{
					result[2][0] = length12;
					result[2][1] = h1;
					result[3][0] = length13;
					result[3][1] = h2;
				}
				else
				{
					result[3][0] = length12;
					result[3][1] = h1;
					result[2][0] = length13;
					result[2][1] = h2;
				}
			}

			/*����Ƕ�*/
			result[0][1] = acos((powf(result[1][0], 2) + powf(result[2][0], 2) - powf(result[3][0], 2)) / (2 * result[1][0] * result[2][0])) / 3.1415926 * 180;
			
			/* step 7: write output */

			ofstream fout;
			if (char status = cv::waitKey(0))
			//if (true)
			{
				//char status = 's';
				switch (status)
				{
				case 's':
					double a;
					double b;
					double c;
					double d;
					double e;
					double f;
					
					a = result[1][0] / l_ratio * 50 * l_compensate;
					b = result[2][0] / l_ratio * 50 * l_compensate;
					c = result[0][1];
					d = result[0][0] / l_ratio * 50 * 2;
					e = result[1][1] / l_ratio * 50 * l_compensate;
					f = result[2][1] / l_ratio * 50 * l_compensate;

					if (g_measNumber <= NUM) {
						g_output[g_measNumber - 1][0] = a;
						g_output[g_measNumber - 1][1] = b;
						g_output[g_measNumber - 1][2] = c;
						g_output[g_measNumber - 1][3] = d;
						g_output[g_measNumber - 1][4] = e;
						g_output[g_measNumber - 1][5] = f;
					}
					
					fout.open("result.txt", ios::out | ios::app);
					if (!fout)
					{
						std::cout << "openning failed!!!" << '\n';
					}

					fout << "measurement-" <<g_measNumber<< '\n';
					fout << "--------------------------------" << '\n';
					fout << "detect " << circles.size() << " circle(s)" << '\n';
					fout << "detected " << lines.size() << " lines" << '\n';

					/*���������*/
					for (int i = 0; i < 6; i++)
					{
						fout<<g_output[g_measNumber - 1][i]<<' ';
					}
					fout << "\nmeasurement finished"<<'\n';
					fout << "--------------------------------" << '\n';
					fout << '\n';

					if (g_measNumber == NUM) {
						/* mean */
						for (int i = 0; i < NUM; i++)
						{
							for (int j = 0; j < 6; j++)
							{
								g_sum[j] += g_output[i][j];
							}
						}
						
						for (int j = 0; j < 6; j++)
						{
							avr[j] = g_sum[j]/NUM;
						}

						for (int i = 0; i < NUM; i++)
						{
							for (int j = 0; j < 6; j++)
							{
								stdvar[j] += powf((g_output[i][j] - avr[j]), 2);
							}
						}

						for (int j = 0; j < 6; j++)
						{
							stdvar[j] /= NUM - 1;
							stdvar[j] = sqrt(stdvar[j]);
						}

						fout << "mean measurement" << '\n';
						fout << "--------------------------------" << '\n';
						fout << "detect " << circles.size() << " circle(s)" << '\n';
						fout << "detected " << lines.size() << " lines" << '\n';

						/*���������*/
						fout << "a," << avr[0] << "," << stdvar[0] << '\n';
						fout << "b," << avr[1] << "," << stdvar[1] << '\n';
						fout << "c," << avr[2] << "," << stdvar[2] << '\n';
						fout << "d," << avr[3] << "," << stdvar[3] << '\n';
						fout << "e," << avr[4] << "," << stdvar[4] << '\n';
						fout << "f," << avr[5] << "," << stdvar[5] << '\n';
						fout << "measurement finished" << '\n';
						fout << "--------------------------------" << '\n';
						fout << '\n';
						pBasicDemo->MessageBox(TEXT("�����Ѽ�¼"), TEXT("�ɹ�"), MB_OK | MB_ICONWARNING);
						
						/*global var clear*/
						g_measNumber = 0;
						for (int j = 0; j < 6; j++)
						{
							g_sum[j] = 0;
							avr[j] = 0;
							stdvar[j] = 0;
						}
					}
					g_measNumber++;
					fout.close();
					cv::destroyAllWindows();
					
					break;
				default:
					cv::destroyAllWindows();
					//pBasicDemo->MessageBox(TEXT("����δ��¼"), TEXT("�ɹ�"), MB_OK | MB_ICONWARNING);
					break;
				}
			}
		}
		else if (MV_Image_Jpeg == stParam.enImageType)
		{
			sprintf_s(pImageName, 20, "%03d.jpg", g_imgNumber);
			g_imgNumber++;
			pBasicDemo->m_nSaveImageType = MV_Image_Undefined;
			FILE* fp = fopen(pImageName, "wb");
			if (NULL == fp)
			{
				pBasicDemo->MessageBox(TEXT("fopen failed"));
				return;
			}
			fwrite(pBasicDemo->m_pBufForSaveImage, 1, stParam.nImageLen, fp);
			fclose(fp);

			pBasicDemo->MessageBox(TEXT("����ɹ�"), TEXT("�ɹ�"), MB_OK | MB_ICONWARNING);
		}
		else if (MV_Image_Png == stParam.enImageType)
		{
			/// Initialize values
			alpha_slider = 0;

			/// Create Windows
			namedWindow("Threshold for bi-level img", 1);

			/// Create Trackbars
			char TrackbarName[50];
			sprintf(TrackbarName, "Max %d", alpha_slider_max);

			createTrackbar(TrackbarName, "Threshold", &alpha_slider, alpha_slider_max, on_trackbar);

			/// Show some stuff
			on_trackbar(alpha_slider, 0);

			pBasicDemo->m_nSaveImageType = MV_Image_Undefined;
		}
	}
}

// ���²����豸��ť:ö��
void CBasicDemoDlg::OnBnClickedEnumButton()
{
	m_bRetStatus = STATUS_OK;
	CString strMsg;

	// 1. ����豸�б���е���Ϣ
	m_ctrlDeviceCombo.ResetContent();

	// 2. ��ʼ���豸��Ϣ�б�
	memset(&m_stDevList, 0, sizeof(MV_CC_DEVICE_INFO_LIST));

	// 3. ö�������������豸
	int nRet = CMyCamera::EnumDevices(&m_stDevList);

	if (MV_OK != nRet)
	{
		// ö���豸ʧ�� 
		m_bRetStatus = STATUS_ERROR;
		return;
	}
	else
	{
	}

	/*��ֵ���뵽��Ϣ�б���в���ʾ����*/
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

	/*ö�ٵ��豸֮��Ҫ��ʾ����*/
	UpdateData(FALSE);

	if (0 == m_stDevList.nDeviceNum)
	{
		MessageBox(TEXT("���豸"));
		return;
	}
	/*�����豸�İ�ť���ֳ���*/
	m_ctrlOpenButton.EnableWindow(TRUE);
	return;
}

// ���´��豸��ť�����豸
void CBasicDemoDlg::OnBnClickedOpenButton()
{
	int nRet = OpenDevice();
	if (STATUS_ERROR == nRet)
	{
		MessageBox(TEXT("��ʧ��"), TEXT("����"), MB_OK | MB_ICONWARNING);
		return;
	}

	nRet = m_pcMyCamera->RegisterImageCallBack(ImageCallBack, this);

	OnBnClickedGetParameterButton(); // ��ȡ����
	EnableWindowWhenOpenNotStart();
	return;
}

// ���¹ر��豸��ť���ر��豸
void CBasicDemoDlg::OnBnClickedCloseButton()
{
	CloseDevice();
	DestroyHandle();
	EnableWindowWhenClose();
	m_ctrlOpenButton.EnableWindow(TRUE);
}

// ��������ģʽ��ť
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

// ���´���ģʽ��ť
void CBasicDemoDlg::OnBnClickedTriggerModeRadio()
{
	((CButton *)GetDlgItem(IDC_CONTINUS_MODE_RADIO))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_TRIGGER_MODE_RADIO))->SetCheck(TRUE);
	m_nTriggerMode = MV_TRIGGER_MODE_ON;
	int nRet = SetTriggerMode();
	if (STATUS_OK != nRet)
	{
		MessageBox(TEXT("���ô���ģʽʧ��"), TEXT("����"), MB_OK | MB_ICONWARNING);
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

// ���¿�ʼ�ɼ���ť
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
		return;
	}

	if (MV_OK != nRet)
	{
		return;
	}
	m_bStartGrabbing = TRUE;
	EnableWindowWhenStart();
	return;
}

// ���½����ɼ���ť
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
		return;
	}

	if (MV_OK != nRet)
	{
		return;
	}
	m_bStartGrabbing = FALSE;
	EnableWindowWhenStop();
	return;
}

// ���»�ȡ������ť
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

// �������ò�����ť
void CBasicDemoDlg::OnBnClickedSetParameterButton()
{
	UpdateData(TRUE);

	int nRet = SetExposureTime();
	if (nRet != STATUS_OK)
	{
		MessageBox(TEXT("�����ع�ʱ��ʧ��"), TEXT("����"), MB_OK | MB_ICONWARNING);
	}
	nRet = SetGain();
	if (nRet != STATUS_OK)
	{
		MessageBox(TEXT("��������ʧ��"), TEXT("����"), MB_OK | MB_ICONWARNING);
	}
	nRet = SetFrameRate();
	if (nRet != STATUS_OK)
	{
		MessageBox(TEXT("����֡��ʧ��"), TEXT("����"), MB_OK | MB_ICONWARNING);
	}
	nRet = SetRatio();
	nRet = SetThreshold();
	nRet = SetCompensate();
	MessageBox(TEXT("���ò����ɹ�"), TEXT("����"), MB_OK | MB_ICONWARNING);
	return;
}

// ����������ť
void CBasicDemoDlg::OnBnClickedSoftwareTriggerCheck()
{
	int nRet = SetTriggerSource();
	if (nRet != STATUS_OK)
	{
		return;
	}
	return;
}

// ��������һ�ΰ�ť
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

// ���±���bmpͼƬ��ť
void CBasicDemoDlg::OnBnClickedSaveBmpButton()
{
	m_nSaveImageType = MV_Image_Bmp;
}

// ���±���jpgͼƬ��ť
void CBasicDemoDlg::OnBnClickedSaveJpgButton()
{
	m_nSaveImageType = MV_Image_Jpeg;
}

// ���Ͻ��˳�
void CBasicDemoDlg::OnClose()
{
	PostQuitMessage(0);
	DestroyHandle();
	CDialog::OnClose();
}

BOOL CBasicDemoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE)
	{
		// �����Ϣ�Ǽ��̰����¼�������Esc����ִ�����´��루ʲô��������������Լ������Ҫ�Ĵ��룩
		return TRUE;
	}
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// �����Ϣ�Ǽ��̰����¼�������Entert����ִ�����´��루ʲô��������������Լ������Ҫ�Ĵ��룩
		return TRUE;
	}


	return CDialog::PreTranslateMessage(pMsg);
}

void CBasicDemoDlg::OnBnClickedSaveJpgButton2()
{
	src = imread("001.jpg");

	// Initialize values
	alpha_slider = 0;

	/// Create Windows
	namedWindow("Linear Blend", 1);

	/// Create Trackbars
	char TrackbarName[50];
	sprintf(TrackbarName, "Alpha x %d", alpha_slider_max);

	createTrackbar(TrackbarName, "Linear Blend", &alpha_slider, alpha_slider_max, on_trackbar);

	/// Show some stuff
	on_trackbar(alpha_slider, 0);
	waitKey(0);
	destroyAllWindows();
}