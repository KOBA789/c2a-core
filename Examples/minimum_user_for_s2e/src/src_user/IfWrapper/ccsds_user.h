/**
 * @file  ccsds_user.h
 * @brief CCSDS API �ˑ��̊֐���錾
 */
#ifndef CCSDS_USER_H_
#define CCSDS_USER_H_

#include <src_core/IfWrapper/ccsds.h>
#include "../Library/stdint.h"

#define CCSDS_FIFO_SIZE (8) // ���ݎg���Ă��� CCSDS API �̐݌v�㌈�܂��Ă���
#define CCSDS_SELECT_NUM (6)

/**
 * @enum  CCSDS_ERR_CODE
 * @brief CCSDS IF �֐��̕Ԃ�l
 */
typedef enum
{
  CCSDS_ERR_TX_NO_BUFFER = -6,
  CCSDS_ERR_TX_INVALID   = -5,
  CCSDS_ERR_TX_SIZE_ERR  = -4,
  CCSDS_ERR_RX_4KBPS     = -3, //!< 4kbps �ɒ����o���Ȃ� BCH �G���[
  CCSDS_ERR_RX_1KBPS     = -2, //!< 1kbps �ɒ����o���Ȃ� BCH �G���[
  CCSDS_ERR_PARAM_ERR    = -1,
  CCSDS_ERR_OK           = 0   //!< OK �� 0 �𓥏P
} CCSDS_ERR_CODE;

/**
 * @struct CCSDS_Info
 * @brief CCSDS ���ݎg���Ă��� CCSDS API �̎��s����
 */
typedef struct
{
  uint32_t uip_stat[CCSDS_SELECT_NUM]; //!< FPGA ���ێ����� CCSDS sequence counter
  uint8_t buffer_num;                  //!< ���M�o�b�t�@�̎c�萔. �ő� CCSDS_FIFO_SIZE
} CCSDS_Info;

/**
 * @brief FPGA ���ێ����� CCSDS sequence counter ��ǂ� API ���Ăяo��
 * @param[in] select ���W�X�^�ǂݏo����I��
 * @param[out] uip_stat: ���
 * @return CCSDS_ERR_CODE
 */
CCSDS_ERR_CODE CCSDS_read_sequence(uint32_t select, uint32_t* uip_stat);

/**
 * @brief CCSDS TX �̎c�� buffer ���J�E���g����API ���Ăт���
 * @return �c�� buffer ��
 */
uint8_t CCSDS_get_buffer_num(void);

/**
 * @brief CCSDS �̃��[�g��ݒ肷�� API ���Ăяo��
 * @param[in] ui_rate: 40000000u ������Ŋ��������[�g���ݒ肳���. 0xFF �ȉ��ł���K�v������ (�����Ă����� 0xFF �����ɂȂ�)
 * @param[in] config: CCSDS_Config
 */
void CCSDS_set_rate(uint32_t ui_rate, CCSDS_Config* config);

#endif
