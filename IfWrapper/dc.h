/**
 * @file   dc.h
 * @brief  DC (Discrete, GPIO) ���b�p�[�̃N���X
 *
 *         DriverSuper Class ��
 *         DC�̃C���^�[�t�F�[�X���������C
 *          DC�|�[�g�̏������CDC�M�����o�͂��s���D
 */
#ifndef DC_H_
#define DC_H_

#include <src_user/Library/stdint.h>

/**
 * @class DC_Config
 * @brief DC�|�[�g�̏������ADC�M�����o�͂̍ۂɕK�v�ƂȂ�ݒ�����i�[����\���̂ł��B
 */
typedef struct
{
  uint16_t  ch_in;  /**< @protected @brief  �`���l���w���bit0���|�[�g1�ɁAbit15���|�[�g16�ɑΉ����A1��ݒ肵���|�[�g��DC�M���������擾���܂��B*/
  uint16_t  ch_out; /**< @protected @brief  �`���l���w���bit0���|�[�g1�ɁAbit15���|�[�g16�ɑΉ����A1��ݒ肵���|�[�g��DC�M���������o�͂��܂��B*/
} DC_Config;


/**
 * @brief  DC�̏�����
 *
 *         DC_Config�\���̂��O���[�o���C���X�^���X�Ƃ��Ē�`���C�|�C���^��n�����ƂŃ|�[�g������������D
 * @param  *my_dc_v : ����������DC_Config�\����
 * @retval 0     : ����I��
 * @retval 0�ȊO : �G���[
 * @note   ���̊֐������s����O��DC_Config�\���̓��̐ݒ�l��ݒ肵�Ă����K�v������
 */
int DC_init(void* my_dc_v);

/**
 * @brief  DC�̓ǂݎ��
 * @param  *my_dc_v    : DC_Config�\����
 * @param  *data_v     : DC�M���l�i�[��ւ̃|�C���^
 * @param  buffer_size : DC�M���l�i�[��̃f�[�^�T�C�Y
 * @retval 0     : ��M�f�[�^�Ȃ�
 * @retval 1�ȏ� : ��M�f�[�^�o�C�g��
 * @retval -1    : �G���[
 */
int DC_rx(void* my_dc_v, void* data_v, int buffer_size);

/**
 * @brief  DC�̏o�͐ݒ�
 * @param  *my_dc_v  : DC_Config�\����
 * @param  *data_v   : DC�M���l�i�[��ւ̃|�C���^
 * @param  data_size : DC�M���l�i�[��̃f�[�^�T�C�Y
 * @retval 0     : ����I��
 * @retval -1    : �G���[
 */
int DC_tx(void* my_dc_v, void* data_v, int data_size);

// �ǉ� on 2019/04/21
// ���J�֐�
// int (*IF_reopen[])(void* my_if, int reason); �ɓo�^
int DC_reopen_dummy(void* my_dc_v, int reason);

#endif
