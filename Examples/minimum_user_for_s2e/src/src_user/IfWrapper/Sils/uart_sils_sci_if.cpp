#pragma section REPRO
/**
 * @file    uart_sils_sci_if.cpp
 * @brief   uart_sils_sci_if
 * @details SILS��Driver�̃e�X�g������悤�ɍ����
            ccsds_sils_sci_if.c/h�̂قڃR�s�[
 */

#include "uart_sils_sci_if.hpp"


// �ŏ��������������āA�v���O�����I�����Ƀ|�[�g�����悤�ɂ�����
static SCIComPortUart sci_com_uart_;

int SILS_SCI_UART_IF_init(void)
{
  return 0;
}

int SILS_SCI_UART_IF_TX(unsigned char* data_v, int count)
{
  sci_com_uart_.Send(data_v, 0, count);
  return 0;
}

int SILS_SCI_UART_IF_RX(unsigned char* data_v, int count)
{
  return sci_com_uart_.Receive(data_v, 0, count);
}


SCIComPortUart::SCIComPortUart(void)
{
  // �r���h�ʂ�Ȃ������̂ŁCZEUS���炿����ƕς���
  myHComPort_ = CreateFile("\\\\.\\COM13", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

  if ((int)myHComPort_ == -1)
  {
    // ����Ƀ|�[�g�I�[�v���ł��Ă��Ȃ��ꍇ�͏I��
    CloseHandle(myHComPort_);
    init_success = false;
    return;
  }

  // �ǂ���琳��|�[�gopen�ɂȂ�Ȃ����ۂ��C���ꂪ�K�v
  init_success = true;

  // �|�[�g�̃{�[���[�g�A�p���e�B����ݒ�
  config_.BaudRate = 115200;
  config_.Parity = PARITY_NONE;
  config_.ByteSize = 8;
  config_.StopBits = STOPBITS_10;

  // Parity�AStopBits�ADataBits�����l�ɐݒ�
  SetCommState(myHComPort_, &config_);
}

SCIComPortUart::~SCIComPortUart(void)
{
  if (init_success == true)
  {
    CloseHandle(myHComPort_);
  }
}

int SCIComPortUart::Send(unsigned char* buffer, size_t offset, size_t count)
{
  DWORD toWriteBytes = count; // ���M�������o�C�g��
  DWORD writeBytes;           // ���ۂɑ��M���ꂽ�o�C�g��

  if (init_success == true)
  {
    WriteFile(myHComPort_, buffer + offset, toWriteBytes, &writeBytes, NULL);

    return writeBytes;
  }
  else
  {
    return 0;
  }
}

int SCIComPortUart::Receive(unsigned char* buffer, size_t offset, size_t count)
{
  DWORD fromReadBytes = count; // ��M�������o�C�g��
  DWORD dwErrors;
  COMSTAT ComStat;
  DWORD dwCount;               // ��M�����o�C�g��

  if (init_success == true)
  {
    ClearCommError(myHComPort_, &dwErrors, &ComStat);
    dwCount = ComStat.cbInQue;

    if (dwCount > 0)
    {
      if (dwCount < count)
      {
        fromReadBytes = dwCount;
        ReadFile(myHComPort_, buffer + offset, fromReadBytes, &dwCount, NULL);
      }
      else
      {
        fromReadBytes = count; // �ǂݍ��݂�����ƃf�[�^��������̂œǂݍ��ޗʂ𐧌�
        ReadFile(myHComPort_, buffer + offset, fromReadBytes, &dwCount, NULL);
      }
    }

    return dwCount;
  }
  else
  {
    return 0;
  }
}

#pragma section
