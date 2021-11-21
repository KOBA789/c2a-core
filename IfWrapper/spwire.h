/**
 * @file spwire.h
 * @brief SPWIRE(�X�y�[�X���C��)�ʐM���b�p�[�̃N���X�ł��B
 *
 * SPWIRE(�X�y�[�X���C��)�ʐM���b�p�[�́A
 * SPWIRE�̃C���^�[�t�F�[�X���������A
 * SPWIRE�|�[�g�̏������A�f�[�^���M�A�f�[�^��M���s���A
 * SPWIRE�ʐM���b�p�[�̃N���X�ł��B
 * �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp��?�����Ďg�p���܂��B
 * @note SPWIRE(�X�y�[�X���C��)�̃��[�e�B���O�͕����l�����܂����A
 * ����͐ڑ�����@�킪SOBC(EM)��CPU�{�[�h(J111�`J114�|�[�g)���m�Őڑ������ꍇ�̌o�H�ɂ̂ݑΉ����Ă��܂��B
 * �܂��A�f�[�^�̂��Ƃ��CPU�{�[�h��̃������}�b�v��ɂ̂݌��肳��Ă��܂�(�f�B�X�e�B�l�[�V�����L�[�Œ�:0x24)�B
 * @warning J111,J112�|�[�g��IF�{�[�h������API�Ŏg�p����Ă����肷��̂ŁA�Ȃ�ׂ�����o���Ȃ������ǂ��B
 * �܂��A����J111�|�[�g�ɂ�IF�{�[�h���ڑ�����Ă���͂��Ȃ̂ŁA���̐ݒ�Ŗ{�N���X���g�p���Ȃ��͂��B
 */

#ifndef SPWIRE_H_
#define SPWIRE_H_

#include <src_user/Library/stdint.h>

/**
 * @class SPWIRE_Config
 * @brief SPWIRE�|�[�g�̏������A�f�[�^���M�A�f�[�^��M�̍ۂɕK�v�ƂȂ�ݒ�����i�[����\���̂ł��B
 */
typedef struct
{
  uint8_t  own_port;   /**< @protected @brief �p����̋@��ւȂ����Ă������g�̃|�[�g�ԍ��̉�1��*/
  uint8_t  target_port;  /**< @protected @brief �p����̋@��̂Ȃ����Ă���|�[�g�ԍ��̉�1��*/
  uint32_t reg_addr;   /**< @protected @brief �p����̋@��̓ǂݏ������s�����W�X�^�A�h���X(�Ȃ�ׂ�4�̔{���A�h���X���w��)*/
} SPWIRE_Config;


int SPWIRE_init(void* my_spwire_v);
int SPWIRE_rx(void* my_spwire_v, void* data_v, int buffer_size);
int SPWIRE_tx(void* my_spwire_v, void* data_v, int data_size);

#endif
