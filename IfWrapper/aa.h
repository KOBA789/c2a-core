/**
 * @file aa.h
 * @brief AA(ActiveAnalog)���b�p�[�̃N���X�ł��B
 *
 * AA(ActiveAnalog)���b�p�[�́A
 * AA�̃C���^�[�t�F�[�X���������A
 * AA�|�[�g�̏������A�d���l�擾���s���A
 * AA���b�p�[�̃N���X�ł��B
 * �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp��?�����Ďg�p���܂��B
 */

#ifndef AA_H_
#define AA_H_

#include <src_user/Library/stdint.h>

/**
 * @class AA_Config
 * @brief AA�|�[�g�̏������A�d���l�擾�̍ۂɕK�v�ƂȂ�ݒ�����i�[����\���̂ł��B
 */
typedef struct
{
  uint8_t ch; /**< @protected @brief  �`���l���w���bit0���|�[�g1�ɁAbit7���|�[�g8�ɑΉ����A1��ݒ肵���|�[�g�̓d���l�������擾���܂��B*/
} AA_Config;


int AA_init(void* my_aa_v);
int AA_rx(void* my_aa_v, void* data_v, int buffer_size);
int AA_tx(void* my_aa_v, void* data_v, int data_size);

#endif
