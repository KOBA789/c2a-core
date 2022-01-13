#pragma section REPRO
#include "anomaly_handler.h"

#include "../Library/print.h"
#include <src_user/Library/VT100.h>
#include "../System/TimeManager/time_manager.h"
#include "../Library/endian_memcpy.h"
#include "../TlmCmd/common_tlm_cmd_packet_util.h"
#include "timeline_command_dispatcher.h"
// #include "../anomaly_group.h"
// #include "../../TlmCmd/block_command_definitions.h"

static void   AH_init_(void);
static void   AH_load_default_rules_(void);
static void   AH_exec_(void);
static size_t AH_check_rules_(size_t record_id);
static int    AH_is_equal_code_(const AL_AnomalyCode* a,
                                const AL_AnomalyCode* b);
static void   AH_respond_to_anomaly_(size_t id);
static void   AH_print_ah_status_(void);
static void   AH_add_rule_(size_t id, const AH_Rule* ahr);
static void   AH_clear_log_(void);
static void   AH_respond_log_clear(void);

static AnomalyHandler anomaly_handler_;
const AnomalyHandler* const anomaly_handler = &anomaly_handler_;

static AH_AnomalyRespondLog AH_respond_log_;
const AH_AnomalyRespondLog* const AH_respond_log = &AH_respond_log_;

static size_t AH_prev_pos_;
static size_t AH_prev_act_;


AppInfo AH_create_app(void)
{
  return AI_create_app_info("ah", AH_init_, AH_exec_);
}


static void AH_init_(void)
{
  int i;

  // �����ݒ�͂��ׂẴ��[���𖳌�������
  for (i = 0; i < AH_MAX_RULES; ++i)
  {
    anomaly_handler_.elements[i].is_active = 0;
  }

  // �f�t�H���g�̃��[���\����ǂݍ���
  AH_load_default_rules_();
  // �Ή��������[���N���A
  OBCT_clear(&(anomaly_handler_.respond_at));
  // �����͔͈͊O�ɐݒ肵�Y���Ȃ���\��
  anomaly_handler_.latest_id = AH_MAX_RULES;
  // �Ή����s�����N���A
  anomaly_handler_.action_counter = 0;
  // �A�m�}���Ή����m�p�̕ϐ����N���A
  AH_prev_act_ = 0;
  // �A�m�}�������󋵂�������
  AH_clear_log_();
  // �e���g���p�y�[�W�ԍ���0�ɏ�����
  anomaly_handler_.page_no = 0;

  AH_respond_log_.pointer = 0;

  for (i = 0; i < AH_LOG_MAX; i++)
  {
    AH_respond_log_.log[i].respond_at_master = 0;
    AH_respond_log_.log[i].rule_num = 0;
  }
  AH_respond_log_.page_no = 0;
}


static void AH_load_default_rules_(void)
{
  AH_Rule_default_load();
}


static void AH_exec_(void)
{
  const AL_AnomalyRecord* ar = AL_get_record(anomaly_handler_.al_pointer.pos);   // �����l�͑����J�n�ʒu

  // �A�m�}�������ω����Ă��Ȃ��ꍇ
  if (anomaly_handler_.al_pointer.count == anomaly_logger->counter) return;
  // �V���ȃA�m�}�������������ꍇ
  else if (anomaly_handler_.al_pointer.count < anomaly_logger->counter)
  {
    size_t record_id;

    for (record_id = anomaly_handler_.al_pointer.pos; record_id < anomaly_logger->header; ++record_id)
    {
      size_t prev_id = AH_check_rules_(record_id);   // �ʒu���킹�̂��߁DReprog���̑ΏǗÖ@�Ȃ̂ŁC�C��������
      (void)prev_id;
      ar = AL_get_record(record_id);

      // �ŏ��̈�ځF�O��Ō�ɋL�^���ꂽ�����D���k���͂����ɋL�^�����
      if (record_id == anomaly_handler_.al_pointer.pos)
      {
        // ����run_length�������Ă���΁��V���ɂ��̃A�m�}�����������Ă����
        if (ar->run_length > anomaly_handler_.latest_run_length)
        {
          // �Ή�������
          size_t id = AH_check_rules_(record_id);

          if (id != AH_MAX_RULES)
          {
            AH_respond_to_anomaly_(id);
          }
        }
      }
      else
      {
        // anomaly_logger->header��1�ȏ㑝���Ă��遨�V���Ȏ�ނ̃A�m�}�����������Ă���
        size_t id = AH_check_rules_(record_id);

        if (id != AH_MAX_RULES)
        {
          AH_respond_to_anomaly_(id);
        }
      }
    }

    // �����I����A�ŐV�ʒu�E�񐔂�AnomalyLogger���Ɠ���
    anomaly_handler_.al_pointer.pos = anomaly_logger->header - 1;
    anomaly_handler_.al_pointer.count = anomaly_logger->counter;
    anomaly_handler_.latest_run_length = ar->run_length;
  }
  // �ُ�: AnomalyHandler��AnomalyLogger�̊֌W������Ă���B
  else
  {
    // �d�����Ȃ��̂ŁA�L�^�����Z�b�g��������������B
    AH_clear_log_();
  }
}


static size_t AH_check_rules_(size_t record_id)
{
  const AL_AnomalyRecord* ar = AL_get_record(record_id);
  int is_latest = (record_id == anomaly_logger->header);
  size_t id;

  // �S���[������������
  for (id = 0; id < AH_MAX_RULES; ++id)
  {
    AH_Element ahe = anomaly_handler_.elements[id];

    // ���[���������̏ꍇ�̓X�L�b�v
    if (!(ahe.is_active)) continue;
    // �R�[�h�s��v�̏ꍇ�̓X�L�b�v
    else if (!AH_is_equal_code_(&(ahe.rule.code), &(ar->code))) continue;

    // �R�[�h�ɑΉ����郋�[�������݁B���������]���B
    switch (ahe.rule.cond)
    {
    case AH_SINGLE:
      // �R�[�h�����݂������_�Ŗ��������v�B
      return id;

    case AH_CONTINUOUS:

      // �A���񐔂𒴉߂����ꍇ�ɏ������v�B
      if (ahe.rule.threshold < ar->run_length) return id;

      break;

    case AH_CUMULATE:

      // �ώZ�񐔂𒴉߂����ꍇ�ɏ������v�B
      if (ahe.counter <= ar->run_length)
      {
        return id;
      }
      // �ŐV���R�[�h�ɑ΂����r�łȂ��ꍇ�͐ώZ�����X�V����B
      else if (!is_latest)
      {
        ahe.counter -= ar->run_length;
      }

      break;
    }
  }

  return AH_MAX_RULES; // �Y���Ȃ��̏ꍇ�͓o�^�����Ԃ��B
}


static int AH_is_equal_code_(const AL_AnomalyCode* a,
                             const AL_AnomalyCode* b)
{
  return ((a->group == b->group) && (a->local == b->local));
}


static void AH_respond_to_anomaly_(size_t id)
{
  // ���ꂪ�Ă΂�Ă�ɂ́Canomaly_handler_.elements[].is_active == 1�͕ۏ؂���Ă���D

  // �Ή��u���b�N�R�}���h�����A���^�C���R�}���h�œW�J
  CTCP packet;
  // �ʏ�BC�Ȃ̂�TLC1�ɓW�J
  CCP_form_block_deploy_cmd(&packet, TL_ID_DEPLOY_BC, anomaly_handler_.elements[id].rule.bc_id);
  PH_dispatch_command(&packet);

  // ���s�������[�����L�^���񐔂��X�V
  anomaly_handler_.respond_at = TMGR_get_master_clock();
  anomaly_handler_.latest_id = id;
  ++anomaly_handler_.action_counter;

  if (AH_respond_log_.pointer < AH_LOG_MAX)
  {
    AH_respond_log_.log[AH_respond_log_.pointer].respond_at_master = TMGR_get_master_total_cycle();
    AH_respond_log_.log[AH_respond_log_.pointer].rule_num = id;
    AH_respond_log_.pointer += 1;
  }

  // ���v�������[���͖���������B
  anomaly_handler_.elements[id].is_active = 0;
}


AppInfo print_ah_status(void)
{
  return AI_create_app_info("ahs", NULL, AH_print_ah_status_);
}


static void AH_print_ah_status_(void)
{
  VT100_erase_line();
  Printf("AH: ID %d, ACT %d, AT <%d, %d, %d>, POS %d, CTR %d\n",
         anomaly_handler_.latest_id, anomaly_handler_.action_counter,
         anomaly_handler_.respond_at.total_cycle, anomaly_handler_.respond_at.mode_cycle, anomaly_handler_.respond_at.step,
         anomaly_handler_.al_pointer.pos, anomaly_handler_.al_pointer.count);

  // �V��̃A�m�}�������������ꍇ�x����1��
  if (AH_prev_pos_ != anomaly_handler_.al_pointer.pos)
  {
    Printf("\a");
    AH_prev_pos_ = anomaly_handler_.al_pointer.pos;
  }

  // �A�m�}���Ή������������ꍇ�x����2��
  if (AH_prev_act_ != anomaly_handler_.action_counter)
  {
    Printf("\a\a");
    AH_prev_act_ = anomaly_handler_.action_counter;
  }
}


CCP_EXEC_STS Cmd_AH_REGISTER_RULE(const CTCP* packet)
{
  enum
  {
    ID, GROUP, LOCAL, COND, THRESHOLD, BC
  };
  const uint8_t* param = CCP_get_param_head(packet);
  AH_Rule ahr;

  if (CCP_get_param_len(packet) != (5 + SIZE_OF_BCT_ID_T))
  {
    // �p�����[�^���m�F(6Bytes)
    return CCP_EXEC_ILLEGAL_LENGTH;
  }
  else if (param[ID] >= AH_MAX_RULES)
  {
    // �o�^�w��ʒu�����e�͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }
  else if (param[COND] > AH_CUMULATE)
  {
    // �����������`���ꂽ���̂ƈ�v���Ȃ�
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  ahr.code.group = (uint32_t)param[GROUP];
  ahr.code.local = (uint32_t)param[LOCAL];
  ahr.cond       = (AH_CONDITION)param[COND];
  ahr.threshold  = (size_t)param[THRESHOLD];
  endian_memcpy(&(ahr.bc_id), &(param[BC]), SIZE_OF_BCT_ID_T);

  AH_add_rule_((size_t)param[ID], &ahr);

  return CCP_EXEC_SUCCESS;
}


static void AH_add_rule_(size_t id, const AH_Rule* ahr)
{
  anomaly_handler_.elements[id].is_active = 0; // �o�^���_�ł͖����Ƃ���
  anomaly_handler_.elements[id].rule = *ahr;
  anomaly_handler_.elements[id].counter = ahr->threshold;
}


CCP_EXEC_STS Cmd_AH_ACTIVATE_RULE(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  size_t id;

  id = param[0];            // �񖾎��I��cast
  if (id >= AH_MAX_RULES)
  {
    // �w��ʒu���͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  AH_activate_rule(id);
  return CCP_EXEC_SUCCESS;
}


void AH_activate_rule(size_t id)
{
  anomaly_handler_.elements[id].is_active = 1;
  anomaly_handler_.elements[id].counter = anomaly_handler_.elements[id].rule.threshold;
}


CCP_EXEC_STS Cmd_AH_INACTIVATE_RULE(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  size_t id;

  id = param[0];            // �񖾎��I��cast
  if (id >= AH_MAX_RULES)
  {
    // �w��ʒu���͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  AH_inactivate_rule(id);
  return CCP_EXEC_SUCCESS;
}


void AH_inactivate_rule(size_t id)
{
  anomaly_handler_.elements[id].is_active = 0;
}


CCP_EXEC_STS Cmd_AH_CLEAR_LOG(const CTCP* packet)
{
  (void)packet;
  AH_clear_log_();
  return CCP_EXEC_SUCCESS;
}


static void AH_clear_log_(void)
{
  // AnomalyHnadler���̕ێ�����������
  anomaly_handler_.al_pointer.count = 0;
  anomaly_handler_.al_pointer.pos = 0;
  // �O���run_length��������
  anomaly_handler_.latest_run_length = 0;
  // �A�m�}���������m�p�ϐ���������
  AH_prev_pos_ = 0;
  // AnomalyLogger�̃��X�g���N���A
  AL_clear();
}


CCP_EXEC_STS Cmd_AH_SET_PAGE_FOR_TLM(const CTCP* packet)
{
  uint8_t page;

  page = CCP_get_param_head(packet)[0];

  if (page >= AH_TLM_PAGE_MAX)
  {
    // �y�[�W�ԍ����R�}���h�e�[�u���͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  anomaly_handler_.page_no = page;
  return CCP_EXEC_SUCCESS;
}


static void AH_respond_log_clear(void)
{
  int i = 0;
  for (i = 0; i < AH_LOG_MAX; i++)
  {
    AH_respond_log_.log[i].respond_at_master = 0;
    AH_respond_log_.log[i].rule_num = 0;
  }
    AH_respond_log_.pointer = 0;
}


CCP_EXEC_STS Cmd_AHRES_LOG_CLEAR(const CTCP* packet)
{
  (void)packet;
  AH_respond_log_clear();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_AHRES_LOG_SET_PAGE_FOR_TLM(const CTCP* packet)
{
  uint8_t page;

  page = CCP_get_param_head(packet)[0];

  if (page >= AH_LOG_TLM_PAGE_MAX)
  {
    // �y�[�W�ԍ����R�}���h�e�[�u���͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  AH_respond_log_.page_no = page;
  return CCP_EXEC_SUCCESS;
}


// 2019-02-04
// ���[�U�[��`�������ɂ��C�ǉ�
void AH_add_rule(size_t id, const AH_Rule* ahr, uint8_t is_active)
{
  AH_add_rule_(id, ahr);
  anomaly_handler_.elements[id].is_active = is_active;
}

#pragma section
