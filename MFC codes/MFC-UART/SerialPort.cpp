#include "pch.h"
#include "SerialPort.h"


CSerialPort::CSerialPort()
{
}

CSerialPort::~CSerialPort()
{
}

bool CSerialPort::OpenPort(CString portname)
{
	m_hComm = CreateFile(L"//./" + portname,
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		0,
		0);
	if (m_hComm == INVALID_HANDLE_VALUE)
	{
		return false;
	}
	else
		return true;
}

bool CSerialPort::ConfigurePort(DWORD BaudRate, BYTE ByteSize, DWORD fParity,
	BYTE Parity, BYTE StopBits)
{
	if ((m_bPortReady = GetCommState(m_hComm, &m_dcb)) == 0)
	{
		TRACE("GetCommState Error");
		//MessageBox(L"GetCommState Error", L"Error", MB_OK + MB_ICONERROR);
		CloseHandle(m_hComm);
		return false;
	}

	m_dcb.BaudRate = BaudRate;
	m_dcb.ByteSize = ByteSize;
	m_dcb.Parity = Parity;
	m_dcb.StopBits = StopBits;
	m_dcb.fBinary = true;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fParity = fParity;
	m_dcb.fOutX = false;
	m_dcb.fInX = false;
	m_dcb.fNull = false;
	m_dcb.fAbortOnError = true;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxDsrFlow = false;
	m_dcb.fDtrControl = DTR_CONTROL_DISABLE;
	m_dcb.fDsrSensitivity = false;
	m_dcb.fRtsControl = RTS_CONTROL_DISABLE;
	m_dcb.fOutxCtsFlow = false;
	m_dcb.fOutxCtsFlow = false;

	m_bPortReady = SetCommState(m_hComm, &m_dcb);

	if (m_bPortReady == 0)
	{
		TRACE("SetCommState Error\n");
		//MessageBox("SetCommState Error");
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool CSerialPort::SetCommunicationTimeouts(DWORD ReadIntervalTimeout,
	DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant,
	DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant)
{
	if ((m_bPortReady = GetCommTimeouts(m_hComm, &m_CommTimeouts)) == 0)
		return false;

	m_CommTimeouts.ReadIntervalTimeout = ReadIntervalTimeout;
	m_CommTimeouts.ReadTotalTimeoutConstant = ReadTotalTimeoutConstant;
	m_CommTimeouts.ReadTotalTimeoutMultiplier = ReadTotalTimeoutMultiplier;
	m_CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
	m_CommTimeouts.WriteTotalTimeoutMultiplier = WriteTotalTimeoutMultiplier;

	m_bPortReady = SetCommTimeouts(m_hComm, &m_CommTimeouts);

	if (m_bPortReady == 0)
	{
		TRACE("StCommTimeouts function failed\n");
		//MessageBox(L"StCommTimeouts function failed", L"Com Port Error", MB_OK + MB_ICONERROR);
		CloseHandle(m_hComm);
		return false;
	}

	return true;
}

bool CSerialPort::WriteByte(BYTE bybyte)
{
	m_iBytesWritten = 0;
	if (WriteFile(m_hComm, &bybyte, 1, &m_iBytesWritten, NULL) == 0)
		return false;
	else
		return true;
}

bool CSerialPort::ReadByte(BYTE& resp)
{
	BYTE rx;
	resp = 0;

	DWORD dwBytesTransferred = 0;

	if (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0))
	{
		if (dwBytesTransferred == 1)
		{
			resp = rx;
			return true;
		}
	}

	return false;
}

bool CSerialPort::ReadByte(BYTE*& resp, UINT size)
{
	UINT cnt = 0;
	BYTE rx;
	DWORD dwBytesTransferred = 0;
	DWORD dwLength;

	DWORD dwErrorFlags;
	COMSTAT ComStat;


	// 큐에서 읽어야할 데이터 크기를 가져옴
	ClearCommError(m_hComm, &dwErrorFlags, &ComStat);
	dwLength = min((DWORD)size, ComStat.cbInQue);

	//TRACE("dwlen : %d\n", dwLength);

    if(dwLength == 0) //아직 버퍼가 채워지지 않는 경우
	{
		ClearCommError(m_hComm, &dwErrorFlags, &ComStat);
		dwLength = min((DWORD)size, ComStat.cbInQue);	
		return false;
	}
	else if(dwLength == size)
	{
		TRACE("dwlen : %d\n", dwLength);
		while (ReadFile(m_hComm, &rx, 1, &dwBytesTransferred, 0))	// 데이터의 크기만큼 한 라인 읽어오기
		{

			if (dwBytesTransferred == 1)
			{
				resp[cnt] = rx;
				//TRACE("rx: %c, cnt: %d\n", rx, cnt);
				if (rx == '\n')
				{
					//TRACE("resp: %s, cnt: %d\n", resp, cnt);
					resp[cnt] = '\0';
					break;
				}
				cnt++;
			}
		}
		return true;
	}
	else // size 이하 data line가 들어오면 skip
	{
		return false;
	}

	/*
	if (ReadFile(m_hComm, resp, size, &dwBytesTransferred, 0))
	{
		if (dwBytesTransferred == size)
			return true;
	}*/
}


void CSerialPort::ClosePort()
{
	CloseHandle(m_hComm);
	return;
}