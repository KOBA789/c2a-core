/**
 * @file  divided_cmd_utility.h
 * @brief �R�}���h�������T�|�[�g����Util
 * @note  HOW TO USE
 *        1. �eCmd�ɂď��߂� DCU_check_in �����s���C���݂̎��s�󋵂��擾����
 *        2. ���s���I���� �ēx�ċA������K�v������ꍇ�� DCU_register_next �œo�^����
 *        3. ���ׂĂ̏������I�������ꍇ�� DCU_report_finish �����s����
 *        4. ���s���̃G���[�� DCU_report_err �ŕ񍐂���D����ƒ��f�����
 *        5. ���f�������Ƃ��� DCU_abort_cmd �����s����
 *        6. ���f���ꂽ��Ԃ����Z�b�g����ꍇ�� DCU_donw_abort_flag �����s����
 * @note  telemetry_manager �ȂǂŎg���Ă���̂ŁC������Q�Ƃ̂���
 */
#ifndef DIVIDED_CMD_UTILITY_H_
#define DIVIDED_CMD_UTILITY_H_

#include "../System/ApplicationManager/app_info.h"
#include "../CmdTlm/common_tlm_cmd_packet.h"
#include "../System/TimeManager/obc_time.h"
#include <src_user/CmdTlm/command_definitions.h>

#define DCU_LOG_MAX    (16)    //!< �ۑ����郍�O�̍ő吔

#include <src_user/Settings/Applications/divided_cmd_utility_params.h>

// key �� uint8_t �Ȃ̂ŁC����ȉ���v������
#if DCU_LOG_MAX > 255
#error "err at divided_cmd_utility"
#endif

/**
 * @enum   DCU_ACK
 * @note   uint8_t ��z��
 * @brief  �ėp�Ԃ�l
 */
typedef enum
{
  DCU_ACK_OK = 0,       //!< ����I��
  DCU_ACK_ERR           //!< �G���[
} DCU_ACK;


/**
 * @enum   DCU_STATUS
 * @note   uint8_t ��z��
 * @brief  ���s��
 */
typedef enum
{
  DCU_STATUS_FINISHED = 0,        //!< ���s�I�� or �����s
  DCU_STATUS_PROGRESS,            //!< ���s��
  DCU_STATUS_ABORTED_BY_ERR,      //!< �G���[�ɂ�蒆�f
  DCU_STATUS_ABORTED_BY_CMD       //!< �R�}���h�ɂ�蒆�f
} DCU_STATUS;


/**
 * @enum   DCU_LOG_ACK
 * @note   uint8_t ��z��
 * @brief  ���O����̕Ԃ�l
 */
typedef enum
{
  DCU_LOG_ACK_OK = 0,           //!< ����I��
  DCU_LOG_ACK_NOT_FOUND         //!< �w�胍�O�������炸
} DCU_LOG_ACK;


/**
 * @struct DCU_ExecStatus
 * @brief  ���s��
 */
typedef struct
{
  CMD_CODE     cmd_code;          //!< ���s�R�}���h
  DCU_STATUS   status;            //!< ���s��
  uint16_t     exec_counter;      //!< ���s�J�E���^�D���x�ڂ̎��s���H
  CCP_EXEC_STS last_exec_sts;     //!< �ŏI���s����
  ObcTime      last_exec_time;    //!< �ŏI���s����
} DCU_ExecStatus;


/**
 * @struct DividedCmdUtility
 * @brief  DividedCmdUtility �� AppInfo �\����
 */
typedef struct
{
  DCU_ExecStatus exec_logs[DCU_LOG_MAX];        //!< ���s���O�D[exec_log_order[0]] ���ł��V�����D�Â����͎̂̂Ă��Ă����D
  uint8_t        exec_log_order[DCU_LOG_MAX];   //!< ���s���O�̃\�[�g�L�[�D exec_logs ���ŐV���ɕ��בւ��邽�߂Ɏg���D
} DividedCmdUtility;

extern const DividedCmdUtility* const divided_cmd_utility;

AppInfo DCU_create_app(void);


/**
 * @brief  DCU�Ƀ`�F�b�N�C������D�����R�}���h���s���C�ŏ��ɌĂяo���֐��D
 * @param[in]  cmd_code:     ���g�� CMD_CODE
 * @param[out] exec_counter: ���̎��s�񐔂�����ڂ��H
 * @retval DCU_STATUS_FINISHED:       ������s�ł���D exec_counter �� 0 ��Ԃ��D
 * @retval DCU_STATUS_PROGRESS:       ����2��ڈȍ~�̎��s�ł���
 * @retval DCU_STATUS_ABORTED_BY_ERR: �G���[���������Ă���̂ŁC���s�𒆎~���ׂ��i CCP_EXEC_ILLEGAL_CONTEXT ��Ԃ��ׂ� �j
 * @retval DCU_STATUS_ABORTED_BY_CMD: ���s���f�R�}���h����M���Ă���̂ŁC���s�𒆎~���ׂ��i CCP_EXEC_ILLEGAL_CONTEXT ��Ԃ��ׂ� �j
 */
DCU_STATUS DCU_check_in(CMD_CODE cmd_code, uint16_t* exec_counter);

/**
 * @brief  ���̕����̎��s��o�^
 * @param[in] cmd_code: ���g�� CMD_CODE
 * @param[in] param:    �R�}���h�p�����^
 * @param[in] len:      �p�����^��
 * @return DCU_ACK
 */
DCU_ACK DCU_register_next(CMD_CODE cmd_code, const uint8_t* param, uint16_t len);

/**
 * @brief  DCU�ɁC�����R�}���h�����s�I���������Ƃ�`����
 * @param[in] cmd_code: ���g�� CMD_CODE
 * @param[in] last_exec_sts: �R�}���h���s����
 * @return void
 */
void DCU_report_finish(CMD_CODE cmd_code, CCP_EXEC_STS last_exec_sts);

/**
 * @brief  DCU�ɁC�r���ŃG���[�������������Ƃ�`����
 * @note   �Ȍ�̎��s�͒��f�����
 * @param[in] cmd_code: ���g�� CMD_CODE
 * @param[in] last_exec_sts: �R�}���h���s����
 * @return void
 */
void DCU_report_err(CMD_CODE cmd_code, CCP_EXEC_STS last_exec_sts);

/**
 * @brief  �G���[�C�܂��̓R�}���h�ɂ���Ē��f�X�e�[�^�X�ƂȂ��Ă���R�}���h���C���s�\��Ԃɖ߂�
 * @param[in] cmd_code: ���s�\��Ԃɖ߂��R�}���h�� CMD_CODE
 * @return void
 */
void DCU_donw_abort_flag(CMD_CODE cmd_code);

/**
 * @brief  ���s���̕����R�}���h���O�����狭���I�ɒ��f������
 * @note   �����G���[�� DCU_report_err ���g���ׂ��ŁC��{�I�ɂ͎g��Ȃ��͂�
 * @note   abort ����̕��A�́C�����������i�r������̍ĊJ�͕s�j
 * @param[in] cmd_code: ��~������R�}���h�� CMD_CODE
 * @return void
 */
void DCU_abort_cmd(CMD_CODE cmd_code);

/**
 * @brief  ���O��T���āC�w�肵���R�}���h���O���擾����
 * @note   exec_log �� divided_cmd_utility �̊Y���̃��O�ւ̃|�C���^�ɍ����ւ���D
 * @param[in] cmd_code: �T���R�}���h
 * @param[out] exec_log: �擾�������O�Dconst�|�C���^�ŁC�Q�Ɛ�����������邽�߁CNULL�|�C���^��n���̂�OK
 * @retval DCU_LOG_ACK_OK:        ��������
 * @retval DCU_LOG_ACK_NOT_FOUND: �����炸
 */
DCU_LOG_ACK DCU_search_and_get_log(CMD_CODE cmd_code, const DCU_ExecStatus* exec_log);

/**
 * @brief ���s���̕����R�}���h���~����
 * @note  �����C�w�肵�� Cmd �����s���ł͂Ȃ��Ă� CCP_EXEC_SUCCESS ��Ԃ�
 */
CCP_EXEC_STS Cmd_DCU_ABORT_CMD(const CTCP* packet);

/**
 * @brief �G���[�C�܂��̓R�}���h�ɂ���Ē��f�X�e�[�^�X�ƂȂ��Ă���R�}���h���C���s�\��Ԃɖ߂�
 * @note  �����C�w�肵�� Cmd �����f�X�e�[�^�X�ł͂Ȃ��Ă� CCP_EXEC_SUCCESS ��Ԃ�
 */
CCP_EXEC_STS Cmd_DCU_DOWN_ABORT_FLAG(const CTCP* packet);

/**
 * @brief ���O���N���A����
 */
CCP_EXEC_STS Cmd_DCU_CLEAR_LOG(const CTCP* packet);

#endif
