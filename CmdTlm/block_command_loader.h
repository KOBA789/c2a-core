/**
 * @file
 * @brief  �u���b�N�R�}���h�o�^�֘A�̃��[�e�B���e�B
 *
 *         �{�@�\�̓u���b�N�R�}���h�̓o�^����wrap���Cuser���Ȍ������S�Ȍ`�Ńu���b�N
 *         �R�}���h�o�^���s����悤�ɂ��邽�߂̃��[�e�B���e�B�@�\�ł��D
 */
#ifndef BLOCK_COMMAND_LOADER_H_
#define BLOCK_COMMAND_LOADER_H_

#include "./block_command_table.h"
#include "../Applications/timeline_command_dispatcher.h"
#include "../System/TimeManager/obc_time.h"


/**
 * @brief  �w�肳�ꂽ�u���b�N�R�}���hID�ւ̓o�^�����S�ɍs�����߂�wrapper�֐�
 * @note   BlockCommandDefinitions.c�ŌĂ΂�邱�Ƃ�z�肵�Ă���
 * @param  pos        �o�^��̃u���b�N�R�}���hID
 * @param  *func      ���ۂɓo�^���s���֐��ւ̃|�C���^
 *                    ���̊֐�������BCL_tool_xxx�ō\������Ă��邱�Ƃ�z�肵�Ă���
 */
void BCL_load_bc(bct_id_t pos, void (*func)(void));

// �����\��
#if 0
void BCL_load_tl(bct_id_t pos, void (*func)(void));
void BCL_load_sl(bct_id_t pos, void (*func)(void));
#endif



// register�n��

/**
 * @brief  �u���b�N�R�}���h�̍Ō�ɃR�}���h��ǉ�����
 * @note   �u���b�N�R�}���h�̒�`���Ɏg�p����
 * @param  ti         �R�}���h�����s���鑊��TI
 * @param  cmd_id     ���s����R�}���hID
 */
void BCL_tool_register_cmd(cycle_t ti, CMD_CODE cmd_id);

/**
 * @brief  �u���b�N�R�}���h�̍Ō�Ƀ��[�e�[�^�[�̎��s�R�}���h��ǉ�����
 * @note   �u���b�N�R�}���h�̒�`���Ɏg�p����
 * @param  ti         �R�}���h�����s���鑊��TI
 * @param  bct_id     ���[�e�[�g����u���b�N�R�}���hID
 */
void BCL_tool_register_rotate(cycle_t ti, bct_id_t bct_id);

/**
 * @brief  �u���b�N�R�}���h�̍Ō�ɃR���o�C�i�[�̎��s�R�}���h��ǉ�����
 * @note   �u���b�N�R�}���h�̒�`���Ɏg�p����
 * @param  ti         �R�}���h�����s���鑊��TI
 * @param  bct_id     �R���o�C������u���b�N�R�}���hID
 */
void BCL_tool_register_combine(cycle_t ti, bct_id_t bct_id);

/**
 * @brief  �u���b�N�R�}���h�̍Ō�Ƀ^�C�����~�b�g�R���o�C�i�[�̎��s�R�}���h��ǉ�����
 * @note   �u���b�N�R�}���h�̒�`���Ɏg�p����
 * @param  ti         �R�}���h�����s���鑊��TI
 * @param  bct_id     �^�C�����~�b�g�R���o�C������u���b�N�R�}���hID
 * @param  limit_step ���߂���Ǝ��s��ł��؂�X�e�b�v��
 */
void BCL_tool_register_limit_combine(cycle_t ti, bct_id_t bct_id, step_t limit_step);

/**
 * @brief  �u���b�N�R�}���h�̍Ō�Ƀu���b�N�R�}���h�̓W�J�R�}���h��ǉ�����
 * @note   BlockCommandDefinitions.c�ŌĂ΂�邱�Ƃ�z�肵�Ă���
 * @param  ti         �R�}���h�����s���鑊��TI
 * @param  bct_id     �W�J����u���b�N�R�}���hID
 * @param  tl_id      �W�J��̃^�C�����C��ID
 */
void BCL_tool_register_deploy(cycle_t ti, bct_id_t bct_id, TL_ID tl_id);

/**
 * @brief  �u���b�N�R�}���h�̍Ō�ɃA�v���̎��s�R�}���h��ǉ�����
 * @note   BlockCommandDefinitions.c�ŌĂ΂�邱�Ƃ�z�肵�Ă���
 * @param  ti         �R�}���h�����s���鑊��TI
 * @param  app_id     ���s����A�v��ID
 */
void BCL_tool_register_app(cycle_t ti, AR_APP_ID app_id);



// add cmd param�n��

/**
 * @brief  �R�}���h�o�^�O�ɃR�}���h�̈����Ƃ���uint8�̃p�����[�^��ǉ�����
 * @note   BCL_tool_register_cmd�O�ł̂ݎg�p���邱�Ƃ�z�肵�Ă���
 * @param  val        �ǉ�����p�����[�^
 */
void BCL_tool_prepare_param_uint8(uint8_t val);

/**
 * @brief  �R�}���h�o�^�O�ɃR�}���h�̈����Ƃ���int8�̃p�����[�^��ǉ�����
 * @note   BCL_tool_register_cmd�O�ł̂ݎg�p���邱�Ƃ�z�肵�Ă���
 * @param  val        �ǉ�����p�����[�^
 */
void BCL_tool_prepare_param_int8(int8_t val);

/**
 * @brief  �R�}���h�o�^�O�ɃR�}���h�̈����Ƃ���uint16�̃p�����[�^��ǉ�����
 * @note   BCL_tool_register_cmd�O�ł̂ݎg�p���邱�Ƃ�z�肵�Ă���
 * @param  val        �ǉ�����p�����[�^
 */
void BCL_tool_prepare_param_uint16(uint16_t val);

/**
 * @brief  �R�}���h�o�^�O�ɃR�}���h�̈����Ƃ���int16�̃p�����[�^��ǉ�����
 * @note   BCL_tool_register_cmd�O�ł̂ݎg�p���邱�Ƃ�z�肵�Ă���
 * @param  val        �ǉ�����p�����[�^
 */
void BCL_tool_prepare_param_int16(int16_t val);

/**
 * @brief  �R�}���h�o�^�O�ɃR�}���h�̈����Ƃ���uint32�̃p�����[�^��ǉ�����
 * @note   BCL_tool_register_cmd�O�ł̂ݎg�p���邱�Ƃ�z�肵�Ă���
 * @param  val        �ǉ�����p�����[�^
 */
void BCL_tool_prepare_param_uint32(uint32_t val);

/**
 * @brief  �R�}���h�o�^�O�ɃR�}���h�̈����Ƃ���int32�̃p�����[�^��ǉ�����
 * @note   BCL_tool_register_cmd�O�ł̂ݎg�p���邱�Ƃ�z�肵�Ă���
 * @param  val        �ǉ�����p�����[�^
 */
void BCL_tool_prepare_param_int32(int32_t val);

/**
 * @brief  �R�}���h�o�^�O�ɃR�}���h�̈����Ƃ���float�̃p�����[�^��ǉ�����
 * @note   BCL_tool_register_cmd�O�ł̂ݎg�p���邱�Ƃ�z�肵�Ă���
 * @param  val        �ǉ�����p�����[�^
 */
void BCL_tool_prepare_param_float(float val);

// �������_�ł�uint64_t���g���Ȃ������C���g���l�����Ȃ��̂ł������񖳌���
#if 0
void BCL_tool_prepare_param_double(double val);
#endif

#endif // BLOCK_COMMAND_LOADER_H_
