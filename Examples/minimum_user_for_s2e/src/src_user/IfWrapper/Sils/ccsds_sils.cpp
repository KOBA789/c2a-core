#pragma section REPRO
/**
* @file ccsds_sils.cpp
* @brief CCSDS�ʐM���b�p�[�̃N���X�ł��B
*
* CCSDS�ʐM���b�p�[�́A
* CCSDS�̃C���^�[�t�F�[�X���������A
* CCSDS�|�[�g�̏������A�f�[�^���M�A�f�[�^��M���s���A
* CCSDS�ʐM���b�p�[�̃N���X�ł��B
* �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp�������Ďg�p���܂��B
* @note OBC�͋[�R���|<->SILS�Ƃ�UART�Ɠ��l�Ƀf�[�^�ʐM���s��
*/

#include "../ccsds_user.h"

#ifdef USE_SCI_COM_WINGS
#include "ccsds_sils_sci_if.hpp"
#endif

#define CCSDS_TX_VALID_SIZE (444)

int CCSDS_init(void* my_ccsds_v)
{
  // �������K�v���� (������ API ������)
  CCSDS_Config* my_ccsds = (CCSDS_Config*)my_ccsds_v;
  CCSDS_set_rate(0xAD,  my_ccsds); // �����l 230.4 [kbps]

#ifdef USE_SCI_COM_WINGS
  SILS_SIC_IF_init();
#endif
  return CCSDS_ERR_OK;
}

int CCSDS_rx(void* my_ccsds_v, void* data_v, int buffer_size)
{
  unsigned char* data = (unsigned char*)data_v;
  (CCSDS_Config*)my_ccsds_v;

#ifdef USE_SCI_COM_WINGS
  return SILS_SIC_IF_RX(data, buffer_size);
#endif

  return 0;
}

int CCSDS_tx(void* my_ccsds_v, void* data_v, int data_size)
{
  unsigned char* data = (unsigned char*)data_v;
  int ret = 0;
  (CCSDS_Config*)my_ccsds_v;

  if (data_size % CCSDS_TX_VALID_SIZE) return CCSDS_ERR_TX_SIZE_ERR;
  if (!CCSDS_get_buffer_num()) return CCSDS_ERR_TX_NO_BUFFER;

#ifdef USE_SCI_COM_WINGS
  ret = SILS_SIC_IF_TX(data, data_size);
#endif
  if (ret == 0) return CCSDS_ERR_TX_INVALID;

  return CCSDS_ERR_OK;
}

int CCSDS_reopen(void* my_ccsds_v, int reason)
{
  return 0;
}

CCSDS_ERR_CODE CCSDS_read_sequence(uint32_t select, uint32_t* uip_stat)
{
  (void)select;
  (void)uip_stat;

  return CCSDS_ERR_OK;
}

uint8_t CCSDS_get_buffer_num(void)
{
  return 8;
}

void CCSDS_set_rate(uint32_t ui_rate, CCSDS_Config* config)
{
  if (ui_rate > 0xFF) ui_rate = 0xFF;

  config->bitrate = 40000000u / ui_rate; // [bps]
}


#pragma section
