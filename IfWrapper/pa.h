/**
 * @file pa.h
 * @brief PA(PassiveAnalog)���b�p�[�̃N���X�ł��B(MOBC�p)
 *
 * PA(PassiveAnalog)���b�p�[�́A
 * PA�̃C���^�[�t�F�[�X���������A
 * PA�|�[�g�̏������A�d���l�擾���s���A
 * PA���b�p�[�̃N���X�ł��B
 * �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp��?�����Ďg�p���܂��B
 */

#ifndef PA_H_
#define PA_H_

#include <src_user/Library/stdint.h>

/**
 * @class PA_Config
 * @brief PA�|�[�g�̏������A�d���l�擾�̍ۂɕK�v�ƂȂ�ݒ�����i�[����\���̂ł��B
 */
typedef struct
{
  uint16_t  ch; /**< @protected @brief  �`���l���w���bit0���|�[�g1�ɁAbit15���|�[�g16�ɑΉ����A1��ݒ肵���|�[�g�̓d���l�������擾���܂��B*/
} PA_Config;


int PA_init(void* my_pa_v);
int PA_rx(void* my_pa_v, void* data_v, int buffer_size);
int PA_tx(void* my_pa_v, void* data_v, int data_size);

#endif
