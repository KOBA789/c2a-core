#pragma section REPRO
/**
 * @file
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
#include "divided_cmd_utility.h"
#include "../CmdTlm/packet_handler.h"
#include "../CmdTlm/common_tlm_cmd_packet_util.h"
#include "../System/TimeManager/time_manager.h"
#include "../System/EventManager/event_logger.h"

/**
 * @brief  App�������֐�
 * @param  void
 * @return void
 */
static void DCU_init_(void);

/**
 * @brief  ���O�N���A
 * @param  void
 * @return void
 */
static void DCU_clear_log_(void);

/**
 * @brief  ���O�N���A�i�P�v�f�j
 * @param[in]  log_idx:  �������O
 * @param[in]  cmd_code: �ݒ肷��R�}���hID
 * @return void
 */
static void DCU_clear_log_element_(uint8_t log_idx, CMD_CODE cmd_code);

/**
 * @brief  �Y���R�}���h�̃��O���������擾����
 * @param[in]  cmd_code: �T������R�}���h
 * @param[out] log_idx:  exec_logs �ɂ����郍�O�̏ꏊ
 * @param[out] sort_key: exec_log_order �ɂ����郍�O�̏ꏊ
 * @retval DCU_LOG_ACK_OK:        ��������
 * @retval DCU_LOG_ACK_NOT_FOUND: �����炸
 */
static DCU_LOG_ACK DCU_search_log_(CMD_CODE cmd_code, uint8_t* log_idx, uint8_t* sort_key);

/**
 * @brief  �Y���R�}���h�̃��O���C���O�̐擪�ɏo��
 * @note   ���O���Ȃ���� insert ����
 * @param[in]  cmd_code: �擪�ɏo���R�}���h
 * @return void
 */
static void DCU_move_to_front_in_log_(CMD_CODE cmd_code);

/**
 * @brief  �擪�ɊY���R�}���h�̃��O������
 * @note   �ł��Â����O���j������邱�Ƃɒ���
 * @note   ���łɃ��O���������ꍇ���`�F�b�N������ insert ����D���łɃ`�F�b�N����Ă��邱�Ƃ�O��Ƃ���I�I�I
 * @param[in]  cmd_code: �擪�ɍ��R�}���h
 * @return void
 */
static void DCU_create_log_on_front_(CMD_CODE cmd_code);


static DividedCmdUtility divided_cmd_utility_;
const DividedCmdUtility* const divided_cmd_utility = &divided_cmd_utility_;

static CTCP DCU_packet_;


AppInfo DCU_create_app(void)
{
  return create_app_info("divided_cmd_utility", DCU_init_, NULL);
}


static void DCU_init_(void)
{
  DCU_clear_log_();
}


static void DCU_clear_log_(void)
{
  uint8_t i;
  for (i = 0; i < DCU_LOG_MAX; ++i)
  {
    DCU_clear_log_element_(i, Cmd_CODE_MAX);
    divided_cmd_utility_.exec_log_order[i] = i;
  }
}


static void DCU_clear_log_element_(uint8_t log_idx, CMD_CODE cmd_code)
{
  divided_cmd_utility_.exec_logs[log_idx].cmd_code = cmd_code;
  divided_cmd_utility_.exec_logs[log_idx].status = DCU_STATUS_FINISHED;
  divided_cmd_utility_.exec_logs[log_idx].exec_counter = 0;
  divided_cmd_utility_.exec_logs[log_idx].last_exec_sts = CCP_EXEC_SUCCESS;
  divided_cmd_utility_.exec_logs[log_idx].last_exec_time = OBCT_create(0, 0, 0);
}


static DCU_LOG_ACK DCU_search_log_(CMD_CODE cmd_code, uint8_t* log_idx, uint8_t* sort_key)
{
  uint8_t i;
  *log_idx = 0;
  *sort_key = 0;

  for (i = 0; i < DCU_LOG_MAX; ++i)
  {
    uint8_t idx = divided_cmd_utility_.exec_log_order[i];
    if (divided_cmd_utility_.exec_logs[idx].cmd_code == cmd_code)
    {
      *log_idx  = idx;
      *sort_key = i;
      return DCU_LOG_ACK_OK;
    }
  }

  return DCU_LOG_ACK_NOT_FOUND;
}


static void DCU_move_to_front_in_log_(CMD_CODE cmd_code)
{
  uint8_t i;
  uint8_t log_idx;
  uint8_t sort_key;
  DCU_LOG_ACK log_ret = DCU_search_log_(cmd_code, &log_idx, &sort_key);

  if (log_ret == DCU_LOG_ACK_NOT_FOUND)
  {
    // ���O���Ȃ��̂ō��
    DCU_create_log_on_front_(cmd_code);
    return;
  }

  for (i = sort_key; i > 0; --i)
  {
    divided_cmd_utility_.exec_log_order[i] = divided_cmd_utility_.exec_log_order[i - 1];
  }
  divided_cmd_utility_.exec_log_order[0] = log_idx;
}


static void DCU_create_log_on_front_(CMD_CODE cmd_code)
{
  uint8_t i;
  uint8_t idx;

  // �ł��Â����O���擾
  idx = divided_cmd_utility_.exec_log_order[DCU_LOG_MAX - 1];
  if (divided_cmd_utility_.exec_logs[idx].status == DCU_STATUS_PROGRESS)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_DCU,
                    (uint32_t)divided_cmd_utility_.exec_logs[idx].cmd_code,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)divided_cmd_utility_.exec_logs[idx].exec_counter);
  }

  for (i = (DCU_LOG_MAX - 1); i > 0; --i)
  {
    divided_cmd_utility_.exec_log_order[i] = divided_cmd_utility_.exec_log_order[i - 1];
  }
  divided_cmd_utility_.exec_log_order[0] = idx;

  DCU_clear_log_element_(idx, cmd_code);
}


DCU_STATUS DCU_check_in(CMD_CODE cmd_code, uint16_t* exec_counter)
{
  uint8_t idx;
  DCU_STATUS status;

  DCU_move_to_front_in_log_(cmd_code);
  idx = divided_cmd_utility_.exec_log_order[0];  // ���g�̃��Oidx�C�܂�ł��V�������Oidx���E��
  status = divided_cmd_utility_.exec_logs[idx].status;

  if (status == DCU_STATUS_FINISHED)
  {
    DCU_clear_log_element_(idx, cmd_code);
  }
  else if (status == DCU_STATUS_PROGRESS)
  {
    divided_cmd_utility_.exec_logs[idx].exec_counter++;
  }
  else
  {
    // �Ȃɂ����Ȃ�
  }

  divided_cmd_utility_.exec_logs[idx].last_exec_time = TMGR_get_master_clock();
  *exec_counter = divided_cmd_utility_.exec_logs[idx].exec_counter;
  return status;
}


DCU_ACK DCU_register_next(CMD_CODE cmd_code, const uint8_t* param, uint16_t len)
{
  uint8_t idx;
  CTCP_UTIL_ACK ret;

  DCU_move_to_front_in_log_(cmd_code);
  idx = divided_cmd_utility_.exec_log_order[0];
  divided_cmd_utility_.exec_logs[idx].status = DCU_STATUS_PROGRESS;

  ret = CCP_form_rtc(&DCU_packet_, cmd_code, param, len);
  if (ret != CTCP_UTIL_ACK_OK) return DCU_ACK_ERR;
  if (PH_analyze_packet(&DCU_packet_) != PH_REGISTERED)
  {
    return DCU_ACK_ERR;
  }

  return DCU_ACK_OK;
}


void DCU_report_finish(CMD_CODE cmd_code, CCP_EXEC_STS last_exec_sts)
{
  uint8_t idx;

  DCU_move_to_front_in_log_(cmd_code);
  idx = divided_cmd_utility_.exec_log_order[0];

  divided_cmd_utility_.exec_logs[idx].status = DCU_STATUS_FINISHED;
  divided_cmd_utility_.exec_logs[idx].last_exec_sts = last_exec_sts;
}


void DCU_report_err(CMD_CODE cmd_code, CCP_EXEC_STS last_exec_sts)
{
  uint8_t idx;

  DCU_move_to_front_in_log_(cmd_code);
  idx = divided_cmd_utility_.exec_log_order[0];

  divided_cmd_utility_.exec_logs[idx].status = DCU_STATUS_ABORTED_BY_ERR;
  divided_cmd_utility_.exec_logs[idx].last_exec_sts = last_exec_sts;
}


void DCU_donw_abort_flag(CMD_CODE cmd_code)
{
  uint8_t idx;

  DCU_move_to_front_in_log_(cmd_code);
  idx = divided_cmd_utility_.exec_log_order[0];

  DCU_clear_log_element_(idx, cmd_code);
}


void DCU_abort_cmd(CMD_CODE cmd_code)
{
  uint8_t idx;

  DCU_move_to_front_in_log_(cmd_code);
  idx = divided_cmd_utility_.exec_log_order[0];

  divided_cmd_utility_.exec_logs[idx].status = DCU_STATUS_ABORTED_BY_CMD;
}


DCU_LOG_ACK DCU_search_and_get_log(CMD_CODE cmd_code, const DCU_ExecStatus* exec_log)
{
  uint8_t log_idx;
  uint8_t sort_key;
  DCU_LOG_ACK log_ret = DCU_search_log_(cmd_code, &log_idx, &sort_key);

  if (log_ret == DCU_LOG_ACK_NOT_FOUND)
  {
    return DCU_LOG_ACK_NOT_FOUND;
  }

  exec_log = &divided_cmd_utility_.exec_logs[log_idx];
  return DCU_LOG_ACK_OK;
}


CCP_EXEC_STS Cmd_DCU_ABORT_CMD(const CTCP* packet)
{
  // CMD_CODE �� u16 �Ƒz�肷��
  CMD_CODE target_cmd = (CMD_CODE)CCP_get_param_from_packet(packet, 0, uint16_t);

  DCU_abort_cmd(target_cmd);

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_DCU_DOWN_ABORT_FLAG(const CTCP* packet)
{
  // CMD_CODE �� u16 �Ƒz�肷��
  CMD_CODE target_cmd = (CMD_CODE)CCP_get_param_from_packet(packet, 0, uint16_t);

  DCU_donw_abort_flag(target_cmd);

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_DCU_CLEAR_LOG(const CTCP* packet)
{
  (void)packet;

  DCU_clear_log_();

  return CCP_EXEC_SUCCESS;
}

#pragma section
