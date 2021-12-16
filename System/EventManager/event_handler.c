#pragma section REPRO
/**
 * @file  event_handler.c
 * @brief �C�x���g���Ό^�������s��
 * @note  event_logger �̏������� BC ��W�J���� ( Event ���΂ɉ����đΉ����s�� )
 */
#include "event_handler.h"
#include <string.h>
#include "../../CmdTlm/common_tlm_cmd_packet_util.h"
#include "../../Applications/timeline_command_dispatcher.h"
#include "../TimeManager/time_manager.h"
#include "../../Library/endian_memcpy.h"

#ifdef EL_IS_ENABLE_TLOG

/**
 * @enum   EH_EL_LOCAL_ID
 * @brief  EL_CORE_GROUP_EVENT_HANDLER �C�x���g�� local id
 * @note   uint8_t ��z��
 */
typedef enum
{
  EH_EL_LOCAL_ID_EL_TOTAL_COUNTER_ERR = 0,  //!< EL��EH�̃J�E���^�̕s�����G���[ (counter_total)
  EH_EL_LOCAL_ID_EL_COUNTER_ERR,            //!< EL��EH�̃J�E���^�̕s�����G���[ (counters)
  EH_EL_LOCAL_ID_TOO_MANY_EVENT,            //!< �C�x���g�������������āCTLog�������Ă���
  EH_EL_LOCAL_ID_FAIL_FORM_CTCP,            //!< BC �W�J Cmd �̐����Ɏ��s
  EH_EL_LOCAL_ID_LOG_TABLE_FULL,            //!< EH_LogTable �����t�ɂȂ�C wp �� 0 �ɖ߂���
  EH_EL_LOCAL_ID_SEARCH_ERR,                //!< EH_search_rule_table_index_ �̕Ԃ�l�s��
  EH_EL_LOCAL_ID_UNKNOWN_ERR                //!< �s���ȃG���[
} EH_EL_LOCAL_ID;

/**
 * @enum   EH_ACK
 * @brief  EH �̓����֐��Ԃ�l
 * @note   uint8_t ��z��
 */
typedef enum
{
  EH_ACK_OK,
  EH_ACK_ERR
} EH_ACK;

/**
 * @enum   EH_RULE_SORTED_INDEX_ACK
 * @brief  EH_RuleSortedIndex �𑀍삷��Ƃ��̕Ԃ�l
 * @note   uint8_t ��z��
 */
typedef enum
{
  EH_RULE_SORTED_INDEX_ACK_OK = 0,           //!< ����I��
  EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID,  //!< �s���� EH_RULE_ID
  EH_RULE_SORTED_INDEX_ACK_NOT_FOUND,        //!< �����炸
  EH_RULE_SORTED_INDEX_ACK_FULL,             //!< ����ȏ�o�^�ł��Ȃ�
  EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE,   //!< ���[���̏㏑���ɂȂ��Ă��܂��i���łɓ��� ID �Ƀ��[�����o�^����Ă��邽�ߊ��p�j
  EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL,   //!< �d������܂ŏd�����Ă��܂��Ă���
  EH_RULE_SORTED_INDEX_ACK_UNKNOWN_ERR       //!< �s���ȃG���[
} EH_RULE_SORTED_INDEX_ACK;

/**
 * @enum   EH_CKECK_AND_RESPOND_ACK
 * @brief  EH_check_{hoge}_and_respond_ �̕Ԃ�l
 * @note   uint8_t ��z��
 */
typedef enum
{
  EH_CKECK_AND_RESPOND_ACK_RESPONDED = 0,         //!< EH �Ƃ��đΉ�����
  EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED,         //!< �Ή����� EH_Rule �Ȃ� or �Ή�������������
  EH_CKECK_AND_RESPOND_ACK_NO_UNPROCESSED_EVENT,  //!< �������� EL_Event �Ȃ�
  EH_CKECK_AND_RESPOND_ACK_UNKNOWN_ERR            //!< �s���ȃG���[
} EH_CKECK_AND_RESPOND_ACK;


/**
 * @brief  EH_RuleTable ��S����
 * @param  void
 * @return void
 */
static void EH_clear_rules_(void);

/**
 * @brief  EH �Ή����O��S����
 * @param  void
 * @return void
 */
static void EH_clear_log_(void);

/**
 * @brief  EL ���� EL_Event �̃J�E���^���`�F�b�N���C���������m�F����
 * @param  void
 * @retval EH_ACK_OK  : ����I��
 * @retval EH_ACK_ERR : EL �� EH �̃J�E���^�̕s�������������C EH �J�E���^���Z�b�g
 */
static EH_ACK EH_check_el_event_counter_(void);

/**
 * @brief  �ł��Â� EL_Event ���P�擾���C EL_Event �ɑΉ����� EH_Rule �����݂��邩�`�F�b�N���C�Ή�����
 * @param  responded_num[out] �Ή����� EH_Rule ��
 * @return EH_CKECK_AND_RESPOND_ACK
 */
static EH_CKECK_AND_RESPOND_ACK EH_check_event_and_respond_(uint8_t* responded_num);

/**
 * @brief  EH �̑Ή��������`�F�b�N���C EL_Event �ɑΉ�����
 * @note   �����̓A�T�[�V�����ς݂����肷��
 * @param  rule_id[in] �Ή��������`�F�b�N���C�Ή����� EH_RULE_ID
 * @param  event[in]   �������� EL_Event
 * @return EH_CKECK_AND_RESPOND_ACK
 */
static EH_CKECK_AND_RESPOND_ACK EH_check_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH �̑Ή��������`�F�b�N���C EL_Event �ɑΉ����� (EH_RESPONSE_CONDITION_SINGLE)
 * @note   �����̓A�T�[�V�����ς݂����肷��
 * @param  rule_id[in] �Ή��������`�F�b�N���C�Ή����� EH_RULE_ID
 * @param  event[in]   �������� EL_Event
 * @return EH_CKECK_AND_RESPOND_ACK
 */
static EH_CKECK_AND_RESPOND_ACK EH_check_single_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH �̑Ή��������`�F�b�N���C EL_Event �ɑΉ����� (EH_RESPONSE_CONDITION_CONTINUOUS)
 * @note   �����̓A�T�[�V�����ς݂����肷��
 * @param  rule_id[in] �Ή��������`�F�b�N���C�Ή����� EH_RULE_ID
 * @param  event[in]   �������� EL_Event
 * @return EH_CKECK_AND_RESPOND_ACK
 */
static EH_CKECK_AND_RESPOND_ACK EH_check_continuous_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH �̑Ή��������`�F�b�N���C EL_Event �ɑΉ����� (EH_RESPONSE_CONDITION_CUMULATIVE)
 * @note   �����̓A�T�[�V�����ς݂����肷��
 * @param  rule_id[in] �Ή��������`�F�b�N���C�Ή����� EH_RULE_ID
 * @param  event[in]   �������� EL_Event
 * @return EH_CKECK_AND_RESPOND_ACK
 */
static EH_CKECK_AND_RESPOND_ACK EH_check_cumulative_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event);

/**
 * @brief  EH �Ή������{
 * @note   �����̓A�T�[�V�����ς݂����肷��
 * @note   is_active �͂��̊֐��ł݂͂Ȃ��i�㗬�Ń`�F�b�N���Ă邱�Ƃ�z��j
 * @param  rule_id[in] �Ή����� EH_RULE_ID
 * @return void
 */
static void EH_respond_(EH_RULE_ID rule_id);

/**
 * @brief  EH �Ή��̃��O���c��
 * @note   �����̓A�T�[�V�����ς݂����肷��
 * @param  rule_id[in]        �Ή����� EH_RULE_ID
 * @param  deploy_cmd_ack[in] �Ή� BC �W�J�R�}���h�̎��s����
 * @return void
 */
static void EH_record_responded_log_(EH_RULE_ID rule_id, CCP_EXEC_STS deploy_cmd_ack);

/**
 * @brief  �܂��������Ă��Ȃ��ł��Â� EL_Event ��Ԃ�
 * @note   ������Ȃ������ꍇ�C NULL ��Ԃ�
 * @param  void
 * @return ������ EL_Event
 */
const EL_Event* EH_get_oldest_event_(void);

/**
 * @brief  EH_RuleSortedIndex ����C�ړI�� EL_Event �� idx ����������
 * @note   �����̓A�T�[�V�����ς݂����肷��
 * @param[in]  group: �������� EL_Event.group
 * @param[in]  local: �������� EL_Event.local
 * @param[out] found_ids: �������� EH_RULE_ID �ꗗ
 * @param[out] found_sorted_idxes: �������� EH_RuleSortedIndex �� index
 * @param[out] found_id_num: �������� idx �̐�
 * @retval EH_RULE_SORTED_INDEX_ACK_NOT_FOUND: �����炸
 * @retval EH_RULE_SORTED_INDEX_ACK_UNKNOWN_ERR: �s���ȃG���[
 * @retval EH_RULE_SORTED_INDEX_ACK_OK: ����ɒT������
 */
static EH_RULE_SORTED_INDEX_ACK EH_search_rule_table_index_(EL_GROUP group,
                                                            uint32_t local,
                                                            EH_RULE_ID found_ids[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint16_t found_sorted_idxes[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint8_t* found_id_num);

/**
 * @brief  EH_Rule �� EH_RuleTable �� EH_RuleSortedIndex �ɑ}������
 * @note   ������ rule �Ɋւ��Ă̓A�T�[�V�����ς݂����肷��
 * @note   ���łɓo�^���ꂽ id �ɑ΂��Ă̏㏑���o�^�̓G���[ (EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE) ��Ԃ�
 * @param[in]  id: �}������ EH_Rule �� EH_RULE_ID
 * @param[in]  rule: �}������ EH_Rule
 * @retval EH_RULE_SORTED_INDEX_ACK_FULL: ���[���o�^����ɓ��B�ς�
 * @retval EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID: �s���� EH_RULE_ID
 * @retval EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE: ���łɓ��� ID �Ƀ��[�����o�^����Ă��邽�ߊ��p
 * @retval EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL: �d������܂ŏd�����Ă��܂��Ă���
 * @retval EH_RULE_SORTED_INDEX_ACK_OK: ����ɑ}������
 */
static EH_RULE_SORTED_INDEX_ACK EH_insert_rule_table_(EH_RULE_ID id, const EH_Rule* rule);

/**
 * @brief  EH_Rule �� EH_RuleTable �� EH_RuleSortedIndex ����폜����
 * @param[in]  id: �폜���� EH_Rule �� EH_RULE_ID
 * @retval EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID: �s���� EH_RULE_ID
 * @retval EH_RULE_SORTED_INDEX_ACK_NOT_FOUND: �폜�Ώۂ������炸 or �o�^����ĂȂ�
 * @retval EH_RULE_SORTED_INDEX_ACK_OK: ����ɍ폜����
 */
static EH_RULE_SORTED_INDEX_ACK EH_delete_rule_table_(EH_RULE_ID id);

/**
 * @brief  EH_RULE_ID �̃��[���o�^�󋵂𒲂ׂ�
 * @param[in] id: ���ׂ� EH_RULE_ID
 * @return EH_CHECK_RULE_ACK
 */
static EH_CHECK_RULE_ACK EH_check_rule_id_(EH_RULE_ID id);


static EventHandler event_handler_;
const EventHandler* const event_handler = &event_handler_;


void EH_initialize(void)
{
  memset(&event_handler_, 0x00, sizeof(EventHandler));

  EH_clear_rules_();
  EH_clear_log_();

  EH_match_event_counter_to_el();
  event_handler_.max_response_num = EH_MAX_RESPONSE_NUM_DEFAULT;
  event_handler_.max_check_event_num = EH_MAX_CHECK_EVENT_NUM_DEFAULT;

  EH_load_default_rules();
}


static void EH_clear_rules_(void)
{
  int i;
  memset(&event_handler_.rule_table, 0x00, sizeof(EH_RuleTable));
  // �����_�� EL_CORE_GROUP_NULL == 0 �ł��邽�߁C�ȉ��͕s�v�D
  // for (i = 0; i < EH_RULE_MAX; ++i)
  // {
  //   event_handler_.rule_table.rules[i].settings.event.group = (EL_GROUP)EL_CORE_GROUP_NULL;
  // }

  // EH_RuleSortedIndex ���N���A
  memset(event_handler_.sorted_idxes, 0x00, sizeof(EH_RuleSortedIndex) * EH_RULE_MAX);
  for (i = 0; i < EH_RULE_MAX; ++i)
  {
    // �����_�� EL_CORE_GROUP_NULL == 0 �ł��邽�߁C�ȉ��͕s�v�D
    // event_handler_.sorted_idxes[i].group = (EL_GROUP)EL_CORE_GROUP_NULL;
    event_handler_.sorted_idxes[i].rule_id = EH_RULE_MAX;
  }
}


static void EH_clear_log_(void)
{
  int i;
  memset(&event_handler_.log_table, 0x00, sizeof(EH_LogTable));

  for (i = 0; i < EH_LOG_MAX; ++i)
  {
    event_handler_.log_table.logs[i].rule_id = EH_RULE_MAX;
  }
}


void EH_execute(void)
{
  uint8_t responded_count = 0;
  EH_ACK ack;
  uint16_t i;

  ack = EH_check_el_event_counter_();
  if (ack != EH_ACK_OK) return;

  for (i = 0; i < event_handler_.max_check_event_num; ++i)
  {
    uint8_t responded_num;
    EH_CKECK_AND_RESPOND_ACK ack = EH_check_event_and_respond_(&responded_num);
    if (ack == EH_CKECK_AND_RESPOND_ACK_NO_UNPROCESSED_EVENT) break;
    responded_count += responded_num;
    if (responded_count >= event_handler_.max_response_num) break;
  }
}


static EH_ACK EH_check_el_event_counter_(void)
{
  int32_t delta_counter = event_logger->statistics.record_counter_total - event_handler_.el_event_counter.counter_total;
  int32_t delta_counters[EL_ERROR_LEVEL_MAX];
  uint8_t err_level;
  uint32_t subtotal = 0;

  // �J�E���^�s�����𒲂ׂ�D
  // �G���[���͒���I�Ƀ��Z�b�g����Ă���͂��Ȃ̂ŁC�I�[�o�[�t���[�͍l�����Ă��Ȃ�
  if (delta_counter < 0)
  {
    // �s����
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_EL_TOTAL_COUNTER_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)(-delta_counter));
    // �d�����Ȃ��̂ŋ����I�ɍ��킹��
    EH_match_event_counter_to_el();
    return EH_ACK_ERR;
  }

  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    delta_counters[err_level] = event_logger->statistics.record_counters[err_level] - event_handler_.el_event_counter.counters[err_level];
    if (delta_counters[err_level] < 0)
    {
      // �s����
      EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                      EH_EL_LOCAL_ID_EL_COUNTER_ERR,
                      EL_ERROR_LEVEL_HIGH,
                      (uint32_t)(err_level));
      // �d�����Ȃ��̂ŋ����I�ɍ��킹��
      EH_match_event_counter_to_el();
      return EH_ACK_ERR;
    }

    subtotal += delta_counters[err_level];
  }

  if (delta_counter != subtotal)
  {
    // �s����
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_EL_COUNTER_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    EL_ERROR_LEVEL_MAX);    // ������ note �͏����l����H
    // �d�����Ȃ��̂ŋ����I�ɍ��킹��
    EH_match_event_counter_to_el();
    return EH_ACK_ERR;
  }

  return EH_ACK_OK;
}


static EH_CKECK_AND_RESPOND_ACK EH_check_event_and_respond_(uint8_t* responded_num)
{
  const EL_Event* event = EH_get_oldest_event_();
  EH_RULE_SORTED_INDEX_ACK ack;

  EH_RULE_ID found_ids[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES];
  uint16_t found_sorted_idxes[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES];
  uint8_t found_id_num;
  uint8_t i;

  // �G���[��Ԃ��Ă������悤�ɂ����ŏ�����
  *responded_num = 0;

  if (event == NULL) return EH_CKECK_AND_RESPOND_ACK_NO_UNPROCESSED_EVENT;

  // �������� EL_Event �����������̂ŁC�J�E���g�A�b�v
  event_handler_.el_event_counter.counter_total++;
  event_handler_.el_event_counter.counters[event->err_level]++;

  ack = EH_search_rule_table_index_(event->group,
                                    event->local,
                                    found_ids,
                                    found_sorted_idxes,
                                    &found_id_num);

  if (ack == EH_RULE_SORTED_INDEX_ACK_NOT_FOUND)
  {
    // �Ή����� EH_Rule �Ȃ�
    return EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED;
  }
  if (ack != EH_RULE_SORTED_INDEX_ACK_OK)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_SEARCH_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)ack);
    return EH_CKECK_AND_RESPOND_ACK_UNKNOWN_ERR;
  }

  // ���[���`�F�b�N & �Ή�
  for (i = 0; i < found_id_num; ++i)
  {
    EH_CKECK_AND_RESPOND_ACK ack =  EH_check_rule_and_respond_(found_ids[i], event);
    if (ack == EH_CKECK_AND_RESPOND_ACK_RESPONDED)
    {
      (*responded_num)++;
    }
  }

  return (*responded_num == 0) ? EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED : EH_CKECK_AND_RESPOND_ACK_RESPONDED;
}


static EH_CKECK_AND_RESPOND_ACK EH_check_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_RuleSettings* rule_settings = &event_handler_.rule_table.rules[rule_id].settings;

  if (rule_settings->event.group != event->group || rule_settings->event.local != event->local)
  {
    // ���������������i���肦�Ȃ����C���S�̂��ߓ���Ă���D���Ȃ������Ȃ�����Ă悵�j
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)rule_id);
    return EH_CKECK_AND_RESPOND_ACK_UNKNOWN_ERR;
  }

  if (rule_settings->should_match_err_level)
  {
    if (rule_settings->event.err_level != event->err_level)
    {
      return EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED;
    }
  }

  if (!(rule_settings->is_active))
  {
    return EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED;
  }

  switch (rule_settings->condition.type)
  {
  case EH_RESPONSE_CONDITION_SINGLE:
    return EH_check_single_rule_and_respond_(rule_id, event);
  case EH_RESPONSE_CONDITION_CONTINUOUS:
    return EH_check_continuous_rule_and_respond_(rule_id, event);
  case EH_RESPONSE_CONDITION_CUMULATIVE:
    return EH_check_cumulative_rule_and_respond_(rule_id, event);
  default:
    // ���������������i���肦�Ȃ����C���S�̂��ߓ���Ă���D���Ȃ������Ȃ�����Ă悵�j
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)rule_id);
    return EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED;
  }
}


static EH_CKECK_AND_RESPOND_ACK EH_check_single_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  rule->last_event_time = event->time;

  EH_respond_(rule_id);
  return EH_CKECK_AND_RESPOND_ACK_RESPONDED;
}


static EH_CKECK_AND_RESPOND_ACK EH_check_continuous_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  uint32_t delta_time_ms = OBCT_diff_in_msec(&rule->last_event_time, &event->time);
  rule->last_event_time = event->time;

  if (delta_time_ms > rule->settings.condition.time_threshold_ms)
  {
    // �A���ł͂Ȃ��Ȃ���
    rule->counter = 0;
  }

  rule->counter++;

  if (rule->counter >= rule->settings.condition.count_threshold)
  {
    EH_respond_(rule_id);
    return EH_CKECK_AND_RESPOND_ACK_RESPONDED;
  }

  return EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED;
}


static EH_CKECK_AND_RESPOND_ACK EH_check_cumulative_rule_and_respond_(EH_RULE_ID rule_id, const EL_Event* event)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  rule->last_event_time = event->time;
  rule->counter++;

  if (rule->counter >= rule->settings.condition.count_threshold)
  {
    EH_respond_(rule_id);
    return EH_CKECK_AND_RESPOND_ACK_RESPONDED;
  }

  return EH_CKECK_AND_RESPOND_ACK_NOT_RESPONDED;
}


static void EH_respond_(EH_RULE_ID rule_id)
{
  EH_Rule* rule = &event_handler_.rule_table.rules[rule_id];
  CTCP packet;
  CTCP_UTIL_ACK deploy_ack;
  CCP_EXEC_STS  deploy_cmd_ack;

  deploy_ack = CCP_form_block_deploy_cmd(&packet, TL_ID_DEPLOY_BC, rule->settings.deploy_bct_id);
  if (deploy_ack != CTCP_UTIL_ACK_OK)
  {
    // BC �W�J Cmd �̐����Ɏ��s
    // �������g��ł���ꍇ�C�����ɗ���͂��͂Ȃ�
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_FAIL_FORM_CTCP,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)rule_id);
    return;
  }

  deploy_cmd_ack = PH_dispatch_command(&packet);
  if (deploy_cmd_ack != CCP_EXEC_SUCCESS)
  {
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_FAIL_FORM_CTCP,
                    EL_ERROR_LEVEL_HIGH,
                    (uint32_t)(deploy_cmd_ack + 100));   // FIXME: CCP_EXEC_STS ���������܂ނ̂ŁD�D�D�Ȃ�Ƃ�������
  }

  rule->counter = 0;
  EH_inactivate_rule(rule_id);

  EH_record_responded_log_(rule_id, deploy_cmd_ack);
}


static void EH_record_responded_log_(EH_RULE_ID rule_id, CCP_EXEC_STS deploy_cmd_ack)
{
  EH_LogTable* log_table = &event_handler_.log_table;

  log_table->respond_counter++;

  log_table->logs[log_table->log_wp].rule_id = rule_id;
  log_table->logs[log_table->log_wp].respond_time_in_master_cycle = TMGR_get_master_total_cycle();
  log_table->logs[log_table->log_wp].deploy_cmd_ack = deploy_cmd_ack;

  log_table->log_wp++;
  if (log_table->log_wp >= EH_LOG_MAX)
  {
    log_table->log_wp = 0;
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_LOG_TABLE_FULL,
                    EL_ERROR_LEVEL_HIGH,
                    0);
  }
}


const EL_Event* EH_get_oldest_event_(void)
{
  uint8_t err_level;      // for �ŉ񂷂̂� u8 ��
  ObcTime oldest_time = OBCT_get_max();
  const EL_Event* oldest_event = NULL;

  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    // EH ���s���ɂ� EL �̃C�x���g�o�^�͔������邽�߁C�����ōŐV�̏������ɏ�������
    int32_t delta_counter = event_logger->statistics.record_counters[err_level] - event_handler_.el_event_counter.counters[err_level];
    const uint16_t tlog_capacity = event_logger->tlogs[err_level].log_capacity;
    const EL_Event* event;

    if (delta_counter <= 0)
    {
      continue;
    }

    // �L���p�����Ă�����C���߂ď����C������
    if (delta_counter > tlog_capacity)
    {
      const uint16_t keep_tlogs_num = (uint16_t)(tlog_capacity / 2);
      uint32_t drop_tlog_num;

      // �C�x���g�������������āCTLog�������Ă���
      EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                      EH_EL_LOCAL_ID_TOO_MANY_EVENT,
                      EL_ERROR_LEVEL_HIGH,
                      (uint32_t)(err_level));

      // ��ꂩ�����Ă���Ƃ������ƂȂ̂ŁC�܂��������邱�Ƃ��l����
      // tlog_capacity �̔������炢�܂ł̂����C��͒��߂�
      drop_tlog_num = (uint32_t)(delta_counter - keep_tlogs_num);

      event_handler_.el_event_counter.counter_total += drop_tlog_num;
      event_handler_.el_event_counter.counters[err_level] += drop_tlog_num;

      // �J�E���^��ς����̂ŁC�X�V
      delta_counter = event_logger->statistics.record_counters[err_level] - event_handler_.el_event_counter.counters[err_level];
    }

    event = EL_get_the_nth_tlog_from_the_latest((EL_ERROR_LEVEL)err_level, (uint16_t)(delta_counter - 1));
    if (OBCT_compare(&event->time, &oldest_time) == 1)
    {
      oldest_event = event;
      oldest_time = event->time;
    }
  }

  return oldest_event;
}


static EH_RULE_SORTED_INDEX_ACK EH_search_rule_table_index_(EL_GROUP group,
                                                            uint32_t local,
                                                            EH_RULE_ID found_ids[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint16_t found_sorted_idxes[EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES],
                                                            uint8_t* found_id_num)
{
  // idx: 0 ~ rule_table.registered_rule_num �̊Ԃœ񕪒T������
  // �d�������蓾�邱�Ƃ��l������

  int32_t lower, upper;     // �v�Z�̓s����Cupper �� -1 �ɂȂ邱�Ƃ�����̂ŁC�����t������
  uint16_t found_idx = EH_RULE_MAX;
  int i = 0;

  if (event_handler_.rule_table.registered_rule_num == 0)
  {
    return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
  }

  lower = 0;
  upper = event_handler_.rule_table.registered_rule_num - 1;   // ��r���i��܁j�͈�

  // �������[�v����̂��߂� while �ł͂Ȃ� for
  for (i = 0; i < EH_RULE_MAX; ++i)
  {
    int32_t mid = (lower + upper) / 2;
    EH_RuleSortedIndex* p_mid = &event_handler_.sorted_idxes[mid];

    if (lower > upper)
    {
      return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
    }

    if (p_mid->group == group)
    {
      if (p_mid->local == local)
      {
        if (p_mid->duplicate_id == 0)
        {
          // ���������I�I�i�d���̏ꍇ�͍ł� duplicate_id �����������́j
          found_idx = (uint16_t)mid;
          break;
        }
        else
        {
          // ���Ⴂ duplicate_id ������͂�
          upper = mid - 1;
        }
      }
      else if (p_mid->local < local)
      {
        lower = mid + 1;
      }
      else
      {
        upper = mid - 1;
      }
    }
    else if (p_mid->group < group)
    {
      lower = mid + 1;
    }
    else
    {
      upper = mid - 1;
    }
  }

  if (found_idx == EH_RULE_MAX) return EH_RULE_SORTED_INDEX_ACK_UNKNOWN_ERR;

  // ���������D��́C�������邩�H
  *found_id_num = 0;
  for (i = 0; i < EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES; ++i)
  {
    EH_RuleSortedIndex* p_idx = &event_handler_.sorted_idxes[found_idx + i];
    if ( !(p_idx->group == group && p_idx->local == local) )
    {
      break;
    }
    found_sorted_idxes[i] = (uint16_t)(found_idx + i);
    found_ids[i] = p_idx->rule_id;
    (*found_id_num)++;
  }

  return EH_RULE_SORTED_INDEX_ACK_OK;
}


static EH_RULE_SORTED_INDEX_ACK EH_insert_rule_table_(EH_RULE_ID id, const EH_Rule* rule)
{
  // insert �́C�߂�ǂ����� & �p�ɂɂ͋N���Ȃ��̂ŁC�񕪒T�������ɂ���Ă���
  EH_RuleTable* p_rule_table = &event_handler_.rule_table;
  EH_RuleSortedIndex* p_sorted_idxes = event_handler_.sorted_idxes;
  const EL_GROUP insert_group = rule->settings.event.group;
  const uint32_t insert_local = rule->settings.event.local;
  uint16_t i;
  uint8_t j;
  uint16_t insert_idx;
  uint8_t duplicate_id;

  EH_CHECK_RULE_ACK check_ack = EH_check_rule_id_(id);
  if (check_ack == EH_CHECK_RULE_ACK_INVALID_RULE_ID) return EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID;
  if (check_ack == EH_CHECK_RULE_ACK_OK) return EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE;

  if (p_rule_table->registered_rule_num >= EH_RULE_MAX)
  {
    return EH_RULE_SORTED_INDEX_ACK_FULL;
  }

  insert_idx = p_rule_table->registered_rule_num;
  duplicate_id = 0;
  for (i = 0; i < p_rule_table->registered_rule_num; ++i)
  {
    EH_RuleSortedIndex* p_sorted_idx = &p_sorted_idxes[i];
    uint8_t break_flag = 0;

    if (p_sorted_idx->group < insert_group) continue;

    if (p_sorted_idx->group > insert_group)
    {
      // �}������ꏊ�͂���
      insert_idx = i;
      break;
    }

    // �����܂ł����� p_sorted_idx->group == insert_group
    if (p_sorted_idx->local < insert_local) continue;

    if (p_sorted_idx->local > insert_local)
    {
      // �}������ꏊ�͂���
      insert_idx = i;
      break;
    }

    // �����܂ł�����
    // - p_sorted_idx->group == insert_group
    // - p_sorted_idx->local == insert_local

    // �d�����Ă� or ����
    for (j = 1; j < EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES; ++j)
    {
      uint16_t idx = (uint16_t)(i + j);
      EH_RuleSortedIndex* p_sorted_idx = &p_sorted_idxes[idx];

      if (idx >= p_rule_table->registered_rule_num)
      {
        // ���[�܂ł����̂ŁC�������}������ꏊ
        insert_idx = idx;
        duplicate_id = j;
        break_flag = 1;
        break;
      }

      if (p_sorted_idx->group == insert_group && p_sorted_idx->local == insert_local)
      {
        // �܂��d�����Ă���
        continue;
      }

      // �d�����Ȃ��Ȃ����̂ŁC�������}������ꏊ
      insert_idx = idx;
      duplicate_id = j;
      break_flag = 1;
      break;
    }
    if (break_flag) break;

    // �d������܂ŏd�����Ă��܂��Ă���
    return EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL;
  }

  // �����܂ŗ�����C�}���ł���
  // insert_idx, duplicate_id ���o�����Ă���͂�
  memmove(&p_sorted_idxes[insert_idx + 1],
          &p_sorted_idxes[insert_idx],
          sizeof(EH_RuleSortedIndex) * (EH_RULE_MAX - insert_idx - 1));
  p_sorted_idxes[insert_idx].group        = insert_group;
  p_sorted_idxes[insert_idx].local        = insert_local;
  p_sorted_idxes[insert_idx].duplicate_id = duplicate_id;
  p_sorted_idxes[insert_idx].rule_id      = id;

  p_rule_table->rules[id] = *rule;
  p_rule_table->registered_rule_num++;

  return EH_RULE_SORTED_INDEX_ACK_OK;
}


static EH_RULE_SORTED_INDEX_ACK EH_delete_rule_table_(EH_RULE_ID id)
{
  // delete �́C�߂�ǂ����� & �p�ɂɂ͋N���Ȃ��̂ŁC�񕪒T�������ɂ���Ă���
  EH_RuleTable* p_rule_table = &event_handler_.rule_table;
  EH_RuleSortedIndex* p_sorted_idxes = event_handler_.sorted_idxes;
  EL_GROUP delete_group;
  uint32_t delete_local;
  uint16_t i;
  uint8_t j;
  uint16_t delete_idx;

  EH_CHECK_RULE_ACK check_ack = EH_check_rule_id_(id);
  if (check_ack == EH_CHECK_RULE_ACK_INVALID_RULE_ID) return EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID;
  if (check_ack == EH_CHECK_RULE_ACK_UNREGISTERED) return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;

  // ���̂Q�Cconst �ɂ������D�D�D
  delete_group = p_rule_table->rules[id].settings.event.group;
  delete_local = p_rule_table->rules[id].settings.event.local;

  delete_idx = EH_RULE_MAX;
  for (i = 0; i < p_rule_table->registered_rule_num; ++i)
  {
    if (id == p_sorted_idxes[i].rule_id)
    {
      // ��������
      delete_idx = i;
      break;
    }
  }

  if (delete_idx == EH_RULE_MAX)
  {
    // �{���͂���͂��肦�Ȃ��͂�
    // �i���肦�Ȃ����C���S�̂��ߓ���Ă���D���Ȃ������Ȃ�����Ă悵�j
    EL_record_event((EL_GROUP)EL_CORE_GROUP_EVENT_HANDLER,
                    EH_EL_LOCAL_ID_UNKNOWN_ERR,
                    EL_ERROR_LEVEL_HIGH,
                    0xffffffff);
    return EH_RULE_SORTED_INDEX_ACK_NOT_FOUND;
  }

  // �����܂ŗ�����폜�ł���
  memmove(&p_sorted_idxes[delete_idx],
          &p_sorted_idxes[delete_idx + 1],
          sizeof(EH_RuleSortedIndex) * (EH_RULE_MAX - delete_idx - 1));
  memset(&p_sorted_idxes[EH_RULE_MAX - 1], 0x00, sizeof(EH_RuleSortedIndex));
  p_sorted_idxes[EH_RULE_MAX - 1].group = (EL_GROUP)EL_CORE_GROUP_NULL;
  p_sorted_idxes[EH_RULE_MAX - 1].rule_id = EH_RULE_MAX;

  // �d��ID�̏���
  for (j = 0; j < (EH_MAX_RULE_NUM_OF_EL_ID_DUPLICATES - 1); ++j)
  {
    uint16_t idx = (uint16_t)(delete_idx + j);
    EH_RuleSortedIndex* p_sorted_idx = &p_sorted_idxes[idx];

    if (p_sorted_idx->group == delete_group && p_sorted_idx->local == delete_local)
    {
      p_sorted_idx->duplicate_id--;
    }
    else
    {
      break;
    }
  }

  memset(&p_rule_table->rules[id], 0x00, sizeof(EH_Rule));
  p_rule_table->rules[id].settings.event.group = (EL_GROUP)EL_CORE_GROUP_NULL;
  p_rule_table->registered_rule_num--;

  return EH_RULE_SORTED_INDEX_ACK_OK;
}


EH_REGISTER_ACK EH_register_rule(EH_RULE_ID id, const EH_RuleSettings* settings)
{
  EH_RULE_SORTED_INDEX_ACK ack;
  EH_Rule rule;

  if (settings->event.group >= EL_GROUP_MAX)                 return EH_REGISTER_ACK_ILLEGAL_GROUP;
  if (settings->event.group <= (EL_GROUP)EL_CORE_GROUP_NULL) return EH_REGISTER_ACK_ILLEGAL_GROUP;
  if (settings->event.err_level < 0)                         return EH_REGISTER_ACK_ILLEGAL_ERROR_LEVEL;
  if (settings->event.err_level >= EL_ERROR_LEVEL_MAX)       return EH_REGISTER_ACK_ILLEGAL_ERROR_LEVEL;
  if (settings->should_match_err_level != 0 && settings->should_match_err_level != 1)
  {
    return EH_REGISTER_ACK_ILLEGAL_MATCH_FLAG;
  }
  if (settings->condition.type >= EH_RESPONSE_CONDITION_TYPE_MAX) return EH_REGISTER_ACK_ILLEGAL_CONDITION_TYPE;
  if (settings->condition.type != EH_RESPONSE_CONDITION_SINGLE)
  {
    if (settings->condition.count_threshold == 0) return EH_REGISTER_ACK_ILLEGAL_COUNT_THRESHOLD;
  }
  if (settings->deploy_bct_id >= BCT_MAX_BLOCKS) return EH_REGISTER_ACK_ILLEGAL_BCT_ID;
  if (settings->is_active != 0 && settings->is_active != 1)
  {
    return EH_REGISTER_ACK_ILLEGAL_ACTIVE_FLAG;
  }

  rule.settings = *settings;
  rule.counter = 0;
  OBCT_clear(&rule.last_event_time);

  ack = EH_insert_rule_table_(id, &rule);

  switch (ack)
  {
  case EH_RULE_SORTED_INDEX_ACK_OK:
    return EH_REGISTER_ACK_OK;
  case EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID:
    return EH_REGISTER_ACK_ILLEGAL_RULE_ID;
  case EH_RULE_SORTED_INDEX_ACK_FULL:
    return EH_REGISTER_ACK_ERR_FULL;
  case EH_RULE_SORTED_INDEX_ACK_RULE_OVERWRITE:
    return EH_REGISTER_ACK_ERR_RULE_OVERWRITE;
  case EH_RULE_SORTED_INDEX_ACK_DUPLICATE_FULL:
    return EH_REGISTER_ACK_ERR_DUPLICATE_FULL;
  default:
    return EH_REGISTER_ACK_UNKNOWN_ERR;
  }
}


static EH_CHECK_RULE_ACK EH_check_rule_id_(EH_RULE_ID id)
{
  if (id < 0) return EH_CHECK_RULE_ACK_INVALID_RULE_ID;
  if (id >= EH_RULE_MAX) return EH_CHECK_RULE_ACK_INVALID_RULE_ID;
  if (event_handler_.rule_table.rules[id].settings.event.group == (EL_GROUP)EL_CORE_GROUP_NULL)
  {
    return EH_CHECK_RULE_ACK_UNREGISTERED;
  }
  return EH_CHECK_RULE_ACK_OK;
}


EH_CHECK_RULE_ACK EH_activate_rule(EH_RULE_ID id)
{
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  event_handler_.rule_table.rules[id].settings.is_active = 1;
  return EH_CHECK_RULE_ACK_OK;
}


EH_CHECK_RULE_ACK EH_inactivate_rule(EH_RULE_ID id)
{
  EH_CHECK_RULE_ACK ack = EH_check_rule_id_(id);
  if (ack != EH_CHECK_RULE_ACK_OK) return ack;

  event_handler_.rule_table.rules[id].settings.is_active = 0;
  return EH_CHECK_RULE_ACK_OK;
}


void EH_match_event_counter_to_el(void)
{
  uint8_t err_level;
  event_handler_.el_event_counter.counter_total = event_logger->statistics.record_counter_total;
  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    event_handler_.el_event_counter.counters[err_level] = event_logger->statistics.record_counters[err_level];
  }
}


const EH_Log* EH_get_the_nth_log_from_the_latest(uint16_t n)
{
  uint16_t idx;

  if (n >= EH_LOG_MAX)
  {
    // �d�����Ȃ��̂ŁC�ŐV�̂��̂�
    idx = 0;
  }
  else
  {
    idx = (uint16_t)((event_handler->log_table.log_wp - 1 - n + EH_LOG_MAX) % EH_LOG_MAX);
  }

  return &event_handler->log_table.logs[idx];
}


CCP_EXEC_STS Cmd_EH_INIT(const CTCP* packet)
{
  (void)packet;
  EH_initialize();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_CLEAR_ALL_RULE(const CTCP* packet)
{
  (void)packet;
  EH_clear_rules_();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_LOAD_DEFAULT_RULE(const CTCP* packet)
{
  (void)packet;
  EH_load_default_rules();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_SET_REGISTER_RULE_EVENT_PARAM(const CTCP* packet)
{
  // �o�^����u�Ԃɂ����킩��Ȃ��̂ŁC�����ł͒l�̃A�T�[�V�����͂����C
  // Cmd_EH_REGISTER_RULE �ŃA�T�[�V��������
  const uint8_t* param = CCP_get_param_head(packet);

  uint16_t temp_u16;
  uint32_t temp_u32;
  bct_id_t bct_id;

  if (CCP_get_param_len(packet) != (12 + SIZE_OF_BCT_ID_T))
  {
    return CCP_EXEC_ILLEGAL_LENGTH;
  }

  endian_memcpy(&temp_u16, &param[0], 2);
  event_handler_.reg_from_cmd.rule_id = (EH_RULE_ID)temp_u16;
  endian_memcpy(&temp_u32, &param[2], 4);
  event_handler_.reg_from_cmd.settings.event.group = (EL_GROUP)temp_u32;
  endian_memcpy(&temp_u32, &param[6], 4);
  event_handler_.reg_from_cmd.settings.event.local = temp_u32;

  event_handler_.reg_from_cmd.settings.event.err_level = (EL_ERROR_LEVEL)param[10];
  event_handler_.reg_from_cmd.settings.should_match_err_level = param[11];

  endian_memcpy(&bct_id, &param[12], SIZE_OF_BCT_ID_T);
  event_handler_.reg_from_cmd.settings.deploy_bct_id = bct_id;

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_SET_REGISTER_RULE_CONDITION_PARAM(const CTCP* packet)
{
  // �o�^����u�Ԃɂ����킩��Ȃ��̂ŁC�����ł͒l�̃A�T�[�V�����͂����C
  // Cmd_EH_REGISTER_RULE �ŃA�T�[�V��������
  const uint8_t* param = CCP_get_param_head(packet);

  uint16_t temp_u16;
  uint32_t temp_u32;

  event_handler_.reg_from_cmd.settings.condition.type = (EH_RESPONSE_CONDITION_TYPE)param[0];

  endian_memcpy(&temp_u16, &param[1], 2);
  event_handler_.reg_from_cmd.settings.condition.count_threshold = temp_u16;
  endian_memcpy(&temp_u32, &param[3], 4);
  event_handler_.reg_from_cmd.settings.condition.time_threshold_ms = temp_u32;

  event_handler_.reg_from_cmd.settings.is_active = param[7];

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_REGISTER_RULE(const CTCP* packet)
{
  (void)packet;
  event_handler_.reg_from_cmd.register_ack =
      EH_register_rule(event_handler_.reg_from_cmd.rule_id, &event_handler_.reg_from_cmd.settings);

  switch (event_handler_.reg_from_cmd.register_ack)
  {
  case EH_REGISTER_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EH_REGISTER_ACK_ILLEGAL_RULE_ID:
  case EH_REGISTER_ACK_ILLEGAL_GROUP:
  case EH_REGISTER_ACK_ILLEGAL_ERROR_LEVEL:
  case EH_REGISTER_ACK_ILLEGAL_MATCH_FLAG:
  case EH_REGISTER_ACK_ILLEGAL_CONDITION_TYPE:
  case EH_REGISTER_ACK_ILLEGAL_COUNT_THRESHOLD:
  case EH_REGISTER_ACK_ILLEGAL_BCT_ID:
  case EH_REGISTER_ACK_ILLEGAL_ACTIVE_FLAG:
    return CCP_EXEC_ILLEGAL_PARAMETER;    // ���m�ɂ͂��̃R�}���h�̃p�����^�ł͂Ȃ����D�D�D
  case EH_REGISTER_ACK_ERR_FULL:
  case EH_REGISTER_ACK_ERR_RULE_OVERWRITE:
  case EH_REGISTER_ACK_ERR_DUPLICATE_FULL:
  case EH_REGISTER_ACK_UNKNOWN_ERR:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EH_DELETE_RULE(const CTCP* packet)
{
  uint16_t rule_id;
  EH_RULE_SORTED_INDEX_ACK ack;

  endian_memcpy(&rule_id, CCP_get_param_head(packet), 2);
  ack = EH_delete_rule_table_((EH_RULE_ID)rule_id);

  switch (ack)
  {
  case EH_RULE_SORTED_INDEX_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EH_RULE_SORTED_INDEX_ACK_ILLEGAL_RULE_ID:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case EH_RULE_SORTED_INDEX_ACK_NOT_FOUND:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EH_ACTIVATE_RULE(const CTCP* packet)
{
  uint16_t rule_id;
  EH_CHECK_RULE_ACK ack;

  endian_memcpy(&rule_id, CCP_get_param_head(packet), 2);
  ack = EH_activate_rule((EH_RULE_ID)rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EH_INACTIVATE_RULE(const CTCP* packet)
{
  uint16_t rule_id;
  EH_CHECK_RULE_ACK ack;

  endian_memcpy(&rule_id, CCP_get_param_head(packet), 2);
  ack = EH_inactivate_rule((EH_RULE_ID)rule_id);

  switch (ack)
  {
  case EH_CHECK_RULE_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EH_CHECK_RULE_ACK_INVALID_RULE_ID:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case EH_CHECK_RULE_ACK_UNREGISTERED:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EH_CLEAR_LOG(const CTCP* packet)
{
  (void)packet;
  EH_clear_log_();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_SET_MAX_RESPONSE_NUM(const CTCP* packet)
{
  event_handler_.max_response_num = CCP_get_param_head(packet)[0];
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_SET_MAX_CHECK_EVENT_NUM(const CTCP* packet)
{
  endian_memcpy(&event_handler_.max_check_event_num, CCP_get_param_head(packet), 2);
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_SET_PAGE_OF_RULE_TABLE_FOR_TLM(const CTCP* packet)
{
  uint8_t page = CCP_get_param_head(packet)[0];
  if (page >= EH_RULE_TLM_PAGE_MAX) return CCP_EXEC_ILLEGAL_PARAMETER;
  event_handler_.tlm_info.rule.page_no = page;
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_SET_PAGE_OF_RULE_SORTED_IDX_FOR_TLM(const CTCP* packet)
{
  uint8_t page = CCP_get_param_head(packet)[0];
  if (page >= EH_RULE_TLM_PAGE_MAX) return CCP_EXEC_ILLEGAL_PARAMETER;
  event_handler_.tlm_info.rule_sorted_index.page_no = page;
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EH_SET_PAGE_OF_LOG_TABLE_FOR_TLM(const CTCP* packet)
{
  uint8_t page = CCP_get_param_head(packet)[0];
  if (page >= EH_LOG_TLM_PAGE_MAX) return CCP_EXEC_ILLEGAL_PARAMETER;
  event_handler_.tlm_info.log.page_no = page;
  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_EH_SET_TARGET_ID_OF_RULE_TABLE_FOR_TLM(const CTCP* packet)
{
  uint16_t temp;
  EH_RULE_ID rule_id;

  endian_memcpy(&temp, CCP_get_param_head(packet), 2);
  rule_id = (EH_RULE_ID)temp;

  if (EH_check_rule_id_(rule_id) == EH_CHECK_RULE_ACK_INVALID_RULE_ID)
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  event_handler_.tlm_info.rule.target_rule_id = rule_id;
  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_EH_MATCH_EVENT_COUNTER_TO_EL(const CTCP* packet)
{
  (void)packet;
  EH_match_event_counter_to_el();
  return CCP_EXEC_SUCCESS;
}

#endif  // EL_IS_ENABLE_TLOG

CCP_EXEC_STS Cmd_EH_TEMP_FOR_ISTS(const CTCP* packet)
{
  EH_RULE_ID rule_id = (EH_RULE_ID)CCP_get_param_from_packet(packet, 0, uint16_t);
  EH_Rule oldest_rule = event_handler->rule_table.rules[EH_RULE_MAX];

  if (EH_check_rule_id_(rule_id) == EH_CHECK_RULE_ACK_INVALID_RULE_ID)
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  if (EH_insert_rule_table_(rule_id, &oldest_rule) != EH_RULE_SORTED_INDEX_ACK_OK)
  {
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
  return CCP_EXEC_SUCCESS;
}

#pragma section
