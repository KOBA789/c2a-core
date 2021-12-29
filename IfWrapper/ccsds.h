/**
 * @file
 * @brief CCSDS�ʐM���b�p�[�̃N���X�ł��B(MOBC�p �o�b�N�O���E���h��荞��Ver)
 *
 *        CCSDS�ʐM���b�p�[�́A
 *        CCSDS�̃C���^�[�t�F�[�X���������A
 *        CCSDS�|�[�g�̏������A�f�[�^���M�A�f�[�^��M���s���A
 *        CCSDS�ʐM���b�p�[�̃N���X�ł��B
 *        �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp��?�����Ďg�p���܂��B
 */

#ifndef CCSDS_H_
#define CCSDS_H_

#include <src_user/Library/stdint.h>

/**
 * @struct CCSDS_Config
 * @brief  CCSDS �|�[�g�̏�����, �f�[�^���M, �f�[�^��M�̍ۂɕK�v�ƂȂ�ݒ�����i�[����\����
 */
typedef struct
{
  uint8_t  ch;      //!< �����I��ch���ǉ�����邱�Ƃ�z�肵��
  uint32_t bitrate; //!< �p����̋@��̃r�b�g���[�g�l (bps)
} CCSDS_Config;

/**
 * @brief CCSDS �� init
 * @note  ���ۂɂ͏����� API �͖����̂� bps �̐ݒ�̂ݍs���Ă���
 * @param[in] my_ccsds_v: CCSDS config
 * @return int: ���������� (OK)
 */
int CCSDS_init(void* my_ccsds_v);

/**
 * @brief CCSDS ��M. API ���Ăяo��, buffer �Ɏ�M���ʂ��R�s�[����
 * @param[in] my_ccsds_v: CCSDS config
 * @param[out] data_v: buffer
 * @param[in] buffer_size: buffer size
 * @return int: �񕉂̏ꍇ�͎�M�f�[�^�T�C�Y, ���̏ꍇ�� CCSDS_ERR_CODE �ɏ]��
 */
int CCSDS_rx(void* my_ccsds_v, void* data_v, int buffer_size);

/**
 * @brief CCSDS ���M. API ���Ăяo��, ���M���s��
 * @param[in] my_ccsds_v: CCSDS config
 * @param[out] data_v: ���M �f�[�^
 * @param[in] data_size: ���M size
 * @return CCSDS_ERR_CODE
 */
int CCSDS_tx(void* my_ccsds_v, void* data_v, int data_size);

/**
 * @brief CCSDS reopen
 * @note  ���ۂɂ� reopen API �͖����̂ŉ������Ȃ�
 * @param[in] my_ccsds_v: CCSDS config
 * @param[in] reason: reopen ���R
 * @return 0
 */
int CCSDS_reopen(void* my_ccsds_v, int reason);

#endif
