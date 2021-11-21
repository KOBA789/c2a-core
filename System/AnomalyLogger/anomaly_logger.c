#pragma section REPRO
#include "anomaly_logger.h"
#include <src_user/Settings/AnomalyLogger/al_default_settings.h>

#include <string.h> // memset

#include "../TimeManager/time_manager.h"
#include "../../Library/endian_memcpy.h"

static void AL_clear_records_(void);
static int  AC_is_equal_(const AL_AnomalyCode* lhs,
                         const AL_AnomalyCode* rhs);
static void AL_init_logging_ena_flag_(void);
static int  AL_is_logging_enable_(uint32_t group);
static int  AL_enable_logging_(uint32_t group);
static int  AL_disable_logging_(uint32_t group);

static AnomalyLogger anomaly_logger_;
const AnomalyLogger* const anomaly_logger = &anomaly_logger_;

const AL_AnomalyRecord* AL_get_record(size_t pos)
{
  // �w��͈̓G���[�̏ꍇ��NULL��Ԃ�
  if (pos >= anomaly_logger_.header) return NULL;

  return &(anomaly_logger_.records[pos]);
}

const AL_AnomalyRecord* AL_get_latest_record(void)
{
  // �o�^�A�m�}���[���Ȃ��ꍇ�͐擪�v�f��Ԃ��B
  // ���̏ꍇ�̐擪�v�f��AL_initialize()�ɂ���ă[���N���A�ς݁B
  if (anomaly_logger_.header == 0) return &(anomaly_logger_.records[0]);

  return &(anomaly_logger_.records[anomaly_logger_.header - 1]);
}

void AL_initialize(void)
{
  AL_clear();
  AL_init_logging_ena_flag_();
  anomaly_logger_.threshold_of_nearly_full = AL_RECORD_MAX - 10;      // ���̒l 2019/02/06

  AL_load_default_settings();
}

CCP_EXEC_STS Cmd_AL_ADD_ANOMALY(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint32_t group, local;
  int ret;

  // �p�����[�^�𒊏o
  endian_memcpy(&group, param, 4);
  endian_memcpy(&local, param + 4, 4);

  // �p�����[�^��o�^
  ret = AL_add_anomaly(group, local);

  if (ret == AL_ADD_SUCCESS)
  {
    return CCP_EXEC_SUCCESS;
  }
  else
  {
    return CCP_EXEC_UNKNOWN;
  }
}

int AL_add_anomaly(uint32_t group, uint32_t local)
{
  const AL_AnomalyRecord* prev_anomaly_record = AL_get_latest_record();
  AL_AnomalyRecord new_anomaly_record;
  int ret;

  ret = AL_is_logging_enable_(group);
  if (ret == -1) return AL_ADD_ERR_INVALID;
  if (ret ==  0) return AL_ADD_DISABLE_LOGGING;

  new_anomaly_record.time       = TMGR_get_master_clock();
  new_anomaly_record.code.group = group;
  new_anomaly_record.code.local = local;
  new_anomaly_record.run_length = 1;

  // �o�^������̏ꍇ�͂��̎������L�^����
  if (anomaly_logger_.header == AL_RECORD_MAX - 1)
  {
    new_anomaly_record.code.group = AL_CORE_GROUP_ANOMALY_LOGGER;
    new_anomaly_record.code.local = AL_FULL;
  }

  // ����AL_AnomalyCode�̓o�^�͈��k����
  if (AC_is_equal_(&(new_anomaly_record.code), &(prev_anomaly_record->code)))
  {
    // �����ƘA�����X�V���ēo�^
    new_anomaly_record.run_length += prev_anomaly_record->run_length;
    anomaly_logger_.records[anomaly_logger_.header - 1] = new_anomaly_record;
  }
  else
  {
    // �V�K�o�^
    anomaly_logger_.records[anomaly_logger_.header] = new_anomaly_record;

    // �o�^��������łȂ��Ȃ�擪�ʒu���C���N�������g
    if (anomaly_logger_.header != AL_RECORD_MAX - 1)
    {
      ++anomaly_logger_.header;
    }
  }

  ++anomaly_logger_.counter;

  if (anomaly_logger_.header == anomaly_logger_.threshold_of_nearly_full)
  {
    AL_add_anomaly(AL_CORE_GROUP_ANOMALY_LOGGER, AL_NEARLY_FULL);
  }

  return AL_ADD_SUCCESS;
}

CCP_EXEC_STS Cmd_AL_CLEAR_LIST(const CTCP* packet)
{
  (void)packet;
  AL_clear();
  return CCP_EXEC_SUCCESS;
}

// �����́CAH������Ă΂��̂Œ��ӁI
void AL_clear(void)
{
  anomaly_logger_.counter = 0;
  anomaly_logger_.header = 0;
  AL_clear_records_();
  anomaly_logger_.page_no = 0;
}

static void AL_clear_records_(void)
{
  int i;
  AL_AnomalyRecord ar = {{0, 0, 0}, {0, 0}, 0};

  for (i = 0; i < AL_RECORD_MAX; ++i)
  {
    anomaly_logger_.records[i] = ar;
  }
}

static int AC_is_equal_(const AL_AnomalyCode* lhs,
                        const AL_AnomalyCode* rhs)
{
  return ((lhs->group == rhs->group) && (lhs->local == rhs->local));
}

CCP_EXEC_STS Cmd_AL_SET_PAGE_FOR_TLM(const CTCP* packet)
{
  uint8_t page;

  page = CCP_get_param_head(packet)[0];

  if (page >= AL_TLM_PAGE_MAX)
  {
    // �y�[�W�ԍ����R�}���h�e�[�u���͈͊O
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  anomaly_logger_.page_no = page;
  return CCP_EXEC_SUCCESS;
}


// 2019-01-18
// �ǉ�

CCP_EXEC_STS Cmd_AL_INIT_LOGGING_ENA_FLAG(const CTCP* packet)
{
  (void)packet;
  AL_init_logging_ena_flag_();
  return CCP_EXEC_SUCCESS;
}

static void AL_init_logging_ena_flag_(void)
{
  int i;
  // �f�t�H���g�ł͑SGROUP ID�Ń��M���O���L��
  for (i = 0; i < (AL_GROUP_MAX / 8); ++i)
  {
    anomaly_logger_.is_logging_enable[i] = 0xff;
  }
}

CCP_EXEC_STS Cmd_AL_ENABLE_LOGGING(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint32_t group;
  int ret;

  // �p�����[�^�𒊏o
  endian_memcpy(&group, param, 4);

  if ( !(0 <= group && group < AL_GROUP_MAX) )
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  ret = AL_enable_logging_(group);

  if (ret == 0)
  {
    return CCP_EXEC_SUCCESS;
  }
  else
  {
    return CCP_EXEC_UNKNOWN;
  }
}

CCP_EXEC_STS Cmd_AL_DISABLE_LOGGING(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint32_t group;
  int ret;

  // �p�����[�^�𒊏o
  endian_memcpy(&group, param, 4);

  if ( !(0 <= group && group < AL_GROUP_MAX) )
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  ret = AL_disable_logging_(group);

  if (ret == 0)
  {
    return CCP_EXEC_SUCCESS;
  }
  else
  {
    return CCP_EXEC_UNKNOWN;
  }
}

// �L���Ȃ�1
// �����Ȃ�0
// �G���[��-1
static int  AL_is_logging_enable_(uint32_t group)
{
  uint32_t group_idx      = group / 8;
  uint32_t group_subidx   = 7 - group % 8;    // ���]
  uint8_t  info;
  uint8_t  mask;
  uint8_t  ret;

  if ( !(0 <= group && group < AL_GROUP_MAX) )
  {
    return -1;
  }


  info = anomaly_logger_.is_logging_enable[group_idx];
  mask = (uint8_t)(0x01 << group_subidx);
  ret  = (uint8_t)(info & mask);

  if (ret == 0) return 0;
  return 1;
}

static int  AL_enable_logging_(uint32_t group)
{
  uint32_t group_idx      = group / 8;
  uint32_t group_subidx   = 7 - group % 8;    // ���]
  uint8_t  info;
  uint8_t  mask;

  if ( !(0 <= group && group < AL_GROUP_MAX) )
  {
    return -1;
  }

  info = anomaly_logger_.is_logging_enable[group_idx];
  mask = (uint8_t)(0x01 << group_subidx);
  info = (uint8_t)(info | mask);

  anomaly_logger_.is_logging_enable[group_idx] = info;

  return 0;
}

static int  AL_disable_logging_(uint32_t group)
{
  uint32_t group_idx      = group / 8;
  uint32_t group_subidx   = 7 - group % 8;    // ���]
  uint8_t  info;
  uint8_t  mask;

  if ( !(0 <= group && group < AL_GROUP_MAX) )
  {
    return -1;
  }

  info = anomaly_logger_.is_logging_enable[group_idx];
  mask = (uint8_t)(0x01 << group_subidx);
  mask = (uint8_t)(~mask);                     // �r�b�g���]
  info = (uint8_t)(info & mask);

  anomaly_logger_.is_logging_enable[group_idx] = info;

  return 0;
}


CCP_EXEC_STS Cmd_AL_SET_THRES_OF_NEARLY_FULL(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint16_t thres;

  // �p�����[�^�𒊏o
  endian_memcpy(&thres, param, 2);

  anomaly_logger_.threshold_of_nearly_full = thres;
  return CCP_EXEC_SUCCESS;
}



// 2019/04/26 ���J����
int AL_enable_logging(uint32_t group)
{
  return AL_enable_logging_(group);
}

int AL_disable_logging(uint32_t group)
{
  return AL_disable_logging_(group);
}

int  AL_is_logging_enable(uint32_t group)
{
  return AL_is_logging_enable_(group);
}


#pragma section
