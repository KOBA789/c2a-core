#pragma section REPRO
#include "utility_counter.h"
#include "../CmdTlm/packet_handler.h"
#include "../System/AnomalyLogger/anomaly_logger.h"
#include <string.h>   // for memcpy

static UtilityCounter utility_counter_;
const UtilityCounter* const utility_counter = &utility_counter_;

static void UTIL_COUNTER_all_init_(void);
static void UTIL_COUNTER_clear_(UTIL_COUNTER_NAME num);
static void UTIL_COUNTER_incl_(UTIL_COUNTER_NAME num);

// �ėp�J�E���^�֘A��������
AppInfo UTIL_COUNTER_create_app(void)
{
  return AI_create_app_info("utility_counter", UTIL_COUNTER_all_init_, NULL);
}

static void UTIL_COUNTER_all_init_(void)
{
  int i;
  for (i = 0; i < UTIL_COUNTER_MAX; i++)
  {
    utility_counter_.cnt[i].counter = 0;
    utility_counter_.cnt[i].threshold = 0;
    utility_counter_.cnt[i].anomaly_active = 0;
  }
}

static void UTIL_COUNTER_clear_(UTIL_COUNTER_NAME num)
{
  utility_counter_.cnt[num].counter = 0;
}

static void UTIL_COUNTER_incl_(UTIL_COUNTER_NAME num)
{
  if (utility_counter_.cnt[num].counter == 0xffffffff)
  {
    utility_counter_.cnt[num].counter = 0;
  }
  else
  {
    utility_counter_.cnt[num].counter += 1;
  }
  if (utility_counter_.cnt[num].counter >= utility_counter_.cnt[num].threshold)
  {
    if (utility_counter_.cnt[num].anomaly_active)
    {
      // FIXME: utility counter �̂�t�@�N�^���ɒ���
// #ifndef AL_DISALBE_AT_C2A_CORE
      AL_add_anomaly(AL_GROUP_UTIL_CNT, (uint32_t)num);  // �J�E���^��臒l���������Ƃ������ăA�m�}���𔭐�������
// #endif
      utility_counter_.cnt[num].anomaly_active = 0;  // �A�m�}���𔭐��������疳��������
    }
  }

}

CCP_EXEC_STS Cmd_UTIL_COUNTER_INCREMENT(const CTCP* packet)
{
  UTIL_COUNTER_NAME index;

  // ���̑�����@�͈��S??
  // index = (int32_t)(CCP_get_param_head(packet)[0] << 24 | CCP_get_param_head(packet)[1] << 16 | CCP_get_param_head(packet)[2] << 8 | CCP_get_param_head(packet)[3]);
  memcpy(&index, &CCP_get_param_head(packet)[0], 4);

  if (index < UTIL_COUNTER_MAX)
  {
    // �͈͓��Ȃ�C���N�������g
    UTIL_COUNTER_incl_(index);
  }
  else
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }
  return CCP_EXEC_SUCCESS;

}

CCP_EXEC_STS Cmd_UTIL_COUNTER_RESET(const CTCP* packet)
{
  UTIL_COUNTER_NAME index;

  // ���̑�����@�͈��S??
  // index = (int32_t)(CCP_get_param_head(packet)[0] << 24 | CCP_get_param_head(packet)[1] << 16 | CCP_get_param_head(packet)[2] << 8 | CCP_get_param_head(packet)[3]);
  memcpy(&index, &CCP_get_param_head(packet)[0], 4);

  if (index < UTIL_COUNTER_MAX)
  {
    // �͈͓��Ȃ�Y���̂��̂��N���A
    UTIL_COUNTER_clear_(index);
  }
  else if (index == 0xff)
  {
    // 0xff�w��Ȃ�S�N���A
    UTIL_COUNTER_all_init_();
  }
  else
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }
  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_UTIL_COUNTER_SET_PARAM(const CTCP* packet)
{
  UTIL_COUNTER_NAME index;

  // ���̑�����@�͈��S??
  // index = (int32_t)(CCP_get_param_head(packet)[0] << 24 | CCP_get_param_head(packet)[1] << 16 | CCP_get_param_head(packet)[2] << 8 | CCP_get_param_head(packet)[3]);
  memcpy(&index, &CCP_get_param_head(packet)[0], 4);

  if (index < UTIL_COUNTER_MAX)
  {
    memcpy(&utility_counter_.cnt[index].threshold, &CCP_get_param_head(packet)[4], 4);
    memcpy(&utility_counter_.cnt[index].anomaly_active, &CCP_get_param_head(packet)[8], 1);
    if (utility_counter_.cnt[index].anomaly_active > 1)
    {
      utility_counter_.cnt[index].anomaly_active = 0;
      return CCP_EXEC_ILLEGAL_PARAMETER;
    }
  }
  else
  {
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }
  return CCP_EXEC_SUCCESS;
}

#pragma section
