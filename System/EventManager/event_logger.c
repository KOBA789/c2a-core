#pragma section REPRO
/**
 * @file  event_logger.c
 * @brief �A�m�}����G���[�Ȃǂ̊e��C�x���g���L�^����
 * @note  ���̃C�x���g�����Ƃ� event_handler �𔭉΂����邱�Ƃ��ł���
 * @note  ���O�� TLog (TimeSeriesLog) �� CLog (EL_CumulativeLog) �̓��ނ��������C�K�v�Ȃ��݂̂̂��g��
 * @note  ����C NvLog (�s����Log) �������\��
 * @note  �ڍׂ� event_logger.h ���Q��
 */
#include "event_logger.h"
#include "event_handler.h"
#include <string.h>
#include "../TimeManager/time_manager.h"
#include "../WatchdogTimer/watchdog_timer.h"
#include <src_user/Settings/System/event_logger_settings.h>
#include "../../Library/endian_memcpy.h"


#ifdef EL_IS_ENABLE_CLOG
/**
 * @enum   EL_CLOG_LOG_ACK
 * @brief  ���O����̕Ԃ�l
 * @note   uint8_t ��z��
 */
typedef enum
{
  EL_CLOG_LOG_ACK_OK = 0,           //!< ����I��
  EL_CLOG_LOG_ACK_NOT_FOUND,        //!< �w�胍�O�������炸
} EL_CLOG_LOG_ACK;
#endif


/**
 * @brief  �C�x���g (EL_Event) ��������
 * @param  group: EL_Event.group
 * @param  local: EL_Event.local
 * @param  err_level: EL_Event.err_level
 * @param  note: EL_Event.note
 * @return EL_Event
 */
static EL_Event EL_init_event_(EL_GROUP group, uint32_t local, EL_ERROR_LEVEL err_level, uint32_t note);

/**
 * @brief  �C�x���g (EL_Event) ���L�^
 * @note   EL�����ł� EL_ERROR_LEVEL_EL �̃C�x���g�����s�ł��邽�߁C���J�֐��ƕ����Ă���
 * @param  group: EL_Event.group
 * @param  local: EL_Event.local
 * @param  err_level: EL_Event.err_level
 * @param  note: EL_Event.note
 * @return EL_ACK
 */
static EL_ACK EL_record_event_(EL_GROUP group, uint32_t local, EL_ERROR_LEVEL err_level, uint32_t note);

#ifdef EL_IS_ENABLE_TLOG
/**
 * @brief  �C�x���g (EL_Event) �� TLog �ɋL�^
 * @note   �A�T�[�V�����̂Ȃ��C�����p�֐�
 * @param  event: �L�^����C�x���g
 * @retval EL_ACK_OK        : ����I��
 * @retval EL_ACK_TLOG_FULL : TLog�̒ǋL���s�\�̂��߁CTLog�L�^���ł���
 */
static EL_ACK EL_record_event_to_tlog_(const EL_Event* event);

/**
 * @brief  TLog �� wp ���`�F�b�N���C event �𐶐�����
 *
 *         wp �� �����C�I�[�̂Ƃ��� event �𐶐�����
 *         ����ɂ��C�����Ő����� event �� TLog �e�[�u���̃f�[�^�̑ޔ��Ȃ��s���Ƃ悢�D
 * @note   err_level �̃A�T�[�V�����͂��Ȃ��i�����֐��j
 * @param  err_level: �`�F�b�N����e�[�u���̃G���[���x��
 * @return void
 */
static void EL_check_tlog_wp_(EL_ERROR_LEVEL err_level);

/**
 * @brief  TLog�e�[�u�������ׂăN���A
 * @return void
 */
static void EL_clear_all_tlog_(void);

/**
 * @brief  TLog�e�[�u�����N���A
 * @note   err_level �̃A�T�[�V�����͂��Ȃ��i�����֐��j
 * @param  err_level: �N���A����e�[�u���̃G���[���x��
 * @return void
 */
static void EL_clear_tlog_(EL_ERROR_LEVEL err_level);
#endif

#ifdef EL_IS_ENABLE_CLOG
/**
 * @brief  �C�x���g (EL_Event) �� CLog �ɋL�^
 * @note   �A�T�[�V�����̂Ȃ��C�����p�֐�
 * @param  event: �L�^����C�x���g
 * @return void
 */
static void EL_record_event_to_clog_(const EL_Event* event);

/**
 * @brief  CLog�e�[�u�������ׂăN���A
 * @return void
 */
static void EL_clear_all_clog_(void);

/**
 * @brief  CLog�e�[�u�����N���A
 * @note   err_level �̃A�T�[�V�����͂��Ȃ��i�����֐��j
 * @param  err_level: �N���A����e�[�u���̃G���[���x��
 * @return void
 */
static void EL_clear_clog_(EL_ERROR_LEVEL err_level);

/**
 * @brief  �Y���C�x���g�� CLog ���������擾����
 * @param[in]  event:     �T������ EL_Event
 * @param[out] log_idx:   EL_CumulativeLog.logs �ɂ����� Clog �̏ꏊ
 * @param[out] order_idx: EL_CumulativeLog.log_orders �ɂ����� Clog �̏ꏊ
 * @retval EL_CLOG_LOG_ACK_OK:        ��������
 * @retval EL_CLOG_LOG_ACK_NOT_FOUND: �����炸
 */
static EL_CLOG_LOG_ACK EL_search_clog_(const EL_Event* event, uint16_t* log_idx, uint16_t* order_idx);

/**
 * @brief  �Y���C�x���g�� CLog ���C�C�x���g�e�[�u���̐擪�ɏo��
 * @note   CLog ���Ȃ���� insert ����
 * @param[in]  event: �擪�ɏo�� EL_Event
 * @return void
 */
static void EL_move_to_front_in_clog_(const EL_Event* event);

/**
 * @brief  �C�x���g�e�[�u���擪�ɊY���C�x���g�� CLog ������
 * @note   �ł��Â� CLog ���j������邱�Ƃɒ���
 * @note   ���ł� CLog ���L�^�����Ă���ꍇ���`�F�b�N������ insert ����D���łɃ`�F�b�N (EL_search_clog_) ����Ă��邱�Ƃ�O��Ƃ���I�I�I
 * @param[in]  event: �擪�ɍ�� EL_Event
 * @return void
 */
static void EL_create_clog_on_front_(const EL_Event* event);
#endif

/**
 * @brief  �ŐV���O���N���A
 * @return void
 */
static void EL_clear_latest_event_(void);

/**
 * @brief  ���v�����N���A
 * @return void
 */
static void EL_clear_statistics_(void);


static EventLogger event_logger_;
const EventLogger* const event_logger = &event_logger_;

#ifdef EL_IS_ENABLE_TLOG
static EL_Event EL_tlog_event_table_high_[EL_TLOG_LOG_SIZE_MAX_HIGH];
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
static EL_Event EL_tlog_event_table_middle_[EL_TLOG_LOG_SIZE_MAX_MIDDLE];
#endif
static EL_Event EL_tlog_event_table_low_[EL_TLOG_LOG_SIZE_MAX_LOW];
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
static EL_Event EL_tlog_event_table_el_[EL_TLOG_LOG_SIZE_MAX_EL];
#endif
#endif

#ifdef EL_IS_ENABLE_CLOG
static EL_CLogElement EL_clog_log_table_high_[EL_CLOG_LOG_SIZE_MAX_HIGH];
static uint16_t EL_clog_log_order_table_high_[EL_CLOG_LOG_SIZE_MAX_HIGH];
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
static EL_CLogElement EL_clog_log_table_middle_[EL_CLOG_LOG_SIZE_MAX_MIDDLE];
static uint16_t EL_clog_log_order_table_middle_[EL_CLOG_LOG_SIZE_MAX_MIDDLE];
#endif
static EL_CLogElement EL_clog_log_table_low_[EL_CLOG_LOG_SIZE_MAX_LOW];
static uint16_t EL_clog_log_order_table_low_[EL_CLOG_LOG_SIZE_MAX_LOW];
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
static EL_CLogElement EL_clog_log_table_el_[EL_CLOG_LOG_SIZE_MAX_EL];
static uint16_t EL_clog_log_order_table_el_[EL_CLOG_LOG_SIZE_MAX_EL];
#endif
#endif


void EL_initialize(void)
{
  memset(&event_logger_, 0x00, sizeof(EventLogger));

  // �ŏ��͂��ׂẴ��M���O���L��
  EL_enable_all_logging();

#ifdef EL_IS_ENABLE_TLOG
  event_logger_.tlogs[EL_ERROR_LEVEL_HIGH].events         = EL_tlog_event_table_high_;
  event_logger_.tlogs[EL_ERROR_LEVEL_HIGH].log_capacity   = EL_TLOG_LOG_SIZE_MAX_HIGH;
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
  event_logger_.tlogs[EL_ERROR_LEVEL_MIDDLE].events       = EL_tlog_event_table_middle_;
  event_logger_.tlogs[EL_ERROR_LEVEL_MIDDLE].log_capacity = EL_TLOG_LOG_SIZE_MAX_MIDDLE;
#endif
  event_logger_.tlogs[EL_ERROR_LEVEL_LOW].events          = EL_tlog_event_table_low_;
  event_logger_.tlogs[EL_ERROR_LEVEL_LOW].log_capacity    = EL_TLOG_LOG_SIZE_MAX_LOW;
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  event_logger_.tlogs[EL_ERROR_LEVEL_EL].events           = EL_tlog_event_table_el_;
  event_logger_.tlogs[EL_ERROR_LEVEL_EL].log_capacity     = EL_TLOG_LOG_SIZE_MAX_EL;
#endif

  EL_clear_all_tlog_();

  // �f�t�H���g�ł� TLog �̏㏑���͗L��
  EL_enable_tlog_overwrite_all();
#endif  // EL_IS_ENABLE_TLOG

#ifdef EL_IS_ENABLE_CLOG
  event_logger_.clogs[EL_ERROR_LEVEL_HIGH].logs           = EL_clog_log_table_high_;
  event_logger_.clogs[EL_ERROR_LEVEL_HIGH].log_orders     = EL_clog_log_order_table_high_;
  event_logger_.clogs[EL_ERROR_LEVEL_HIGH].log_capacity   = EL_CLOG_LOG_SIZE_MAX_HIGH;
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
  event_logger_.clogs[EL_ERROR_LEVEL_MIDDLE].logs         = EL_clog_log_table_middle_;
  event_logger_.clogs[EL_ERROR_LEVEL_MIDDLE].log_orders   = EL_clog_log_order_table_middle_;
  event_logger_.clogs[EL_ERROR_LEVEL_MIDDLE].log_capacity = EL_CLOG_LOG_SIZE_MAX_MIDDLE;
#endif
  event_logger_.clogs[EL_ERROR_LEVEL_LOW].logs            = EL_clog_log_table_low_;
  event_logger_.clogs[EL_ERROR_LEVEL_LOW].log_orders      = EL_clog_log_order_table_low_;
  event_logger_.clogs[EL_ERROR_LEVEL_LOW].log_capacity    = EL_CLOG_LOG_SIZE_MAX_LOW;
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  event_logger_.clogs[EL_ERROR_LEVEL_EL].logs             = EL_clog_log_table_el_;
  event_logger_.clogs[EL_ERROR_LEVEL_EL].log_orders       = EL_clog_log_order_table_el_;
  event_logger_.clogs[EL_ERROR_LEVEL_EL].log_capacity     = EL_CLOG_LOG_SIZE_MAX_EL;
#endif

  EL_clear_all_clog_();
#endif  // EL_IS_ENABLE_CLOG

  // EL_clear_statistics_();    // �`���� memset ������̂ŕs�v
  // EL_clear_latest_event_();    // �`���� memset ������̂ŕs�v

  // ���[�U�[�f�t�H���g�ݒ�
  EL_load_default_settings();

  EH_match_event_counter_to_el();
}


static EL_Event EL_init_event_(EL_GROUP group, uint32_t local, EL_ERROR_LEVEL err_level, uint32_t note)
{
  EL_Event event;

  event.group = group;
  event.local = local;
  event.err_level = err_level;
  event.time = TMGR_get_master_clock();
#ifdef EL_IS_ENABLE_EVENT_NOTE
  event.note = note;
#else
  (void)note;
#endif

  return event;
}


EL_ACK EL_record_event(EL_GROUP group, uint32_t local, EL_ERROR_LEVEL err_level, uint32_t note)
{
  // ���[�U�[����̃C�x���g�L�^�� EL_IS_ENABLE_EL_ERROR_LEVEL �͋֎~
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  if (err_level == EL_ERROR_LEVEL_EL) return EL_ACK_ILLEGAL_ERROR_LEVEL;
#endif

  return EL_record_event_(group, local, err_level, note);
}


static EL_ACK EL_record_event_(EL_GROUP group, uint32_t local, EL_ERROR_LEVEL err_level, uint32_t note)
{
  const EL_Event event = EL_init_event_(group, local, err_level, note);
  EL_ACK ack = EL_ACK_OK;

  if (event.group >= EL_GROUP_MAX)           return EL_ACK_ILLEGAL_GROUP;
  if (event.group <= EL_CORE_GROUP_NULL)     return EL_ACK_ILLEGAL_GROUP;     // ����͖{���ɓ���Ă悢���v�c�_
  if (event.err_level < 0)                   return EL_ACK_ILLEGAL_ERROR_LEVEL;
  if (event.err_level >= EL_ERROR_LEVEL_MAX) return EL_ACK_ILLEGAL_ERROR_LEVEL;
  if (!EL_is_logging_enable(event.group))    return EL_ACK_DISABLE_LOGGING;

  event_logger_.latest_event = event;   // �ċA�Ăяo���̉\��������̂ŁC�ʓr�R�s�[���Ď���
  event_logger_.statistics.record_counter_total++;
  event_logger_.statistics.record_counters[event.err_level]++;

#ifdef EL_IS_ENABLE_TLOG
  ack = EL_record_event_to_tlog_(&event);
#endif

#ifdef EL_IS_ENABLE_CLOG
  EL_record_event_to_clog_(&event);
#endif

  // EL_record_event �̍ċA�Ăяo���ɂȂ�̂ōŌ�ɌĂяo���D���ӁI�I
#ifdef EL_IS_ENABLE_TLOG
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  // �ǋL�ł��Ȃ��ꍇ�̓X�L�b�v
  if (ack != EL_ACK_TLOG_FULL)
  {
    EL_check_tlog_wp_(event.err_level);
  }
#endif
#endif

  return ack;
}


#ifdef EL_IS_ENABLE_TLOG
static EL_ACK EL_record_event_to_tlog_(const EL_Event* event)
{
  const EL_ERROR_LEVEL err_level = event->err_level;
  EL_TimeSeriesLog* p_tlog = &event_logger_.tlogs[err_level];
  const uint16_t wp = p_tlog->log_wp;

  if ( EL_is_tlog_overwrite_enable(err_level) == 0 && p_tlog->is_table_overflow == 1 )
  {
    // �ǋL�ł��Ȃ�
    return EL_ACK_TLOG_FULL;
  }

  p_tlog->events[wp] = *event;
  p_tlog->log_wp++;
  p_tlog->log_wp %= p_tlog->log_capacity;

  if ( EL_is_tlog_overwrite_enable(err_level) == 0 && p_tlog->log_wp == 0 )
  {
    // ���񂩂�ǋL�ł��Ȃ��Ȃ�
    p_tlog->is_table_overflow = 1;
  }
  else
  {
    p_tlog->is_table_overflow = 0;
  }
  return EL_ACK_OK;
}


#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
static void EL_check_tlog_wp_(EL_ERROR_LEVEL err_level)
{
  const uint16_t wp = event_logger_.tlogs[err_level].log_wp;
  const uint16_t capacity = event_logger_.tlogs[err_level].log_capacity;
  if (wp == 0)    // capacity �𒴂��C��������Ƃ�
  {
    switch (err_level)
    {
    case EL_ERROR_LEVEL_HIGH:
      EL_record_event_((EL_GROUP)EL_CORE_GROUP_EVENT_LOGGER,
                       EL_EVENT_LOCAL_TLOG_HIGH_FULL,
                       EL_ERROR_LEVEL_EL,
                       0);
      break;

#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
    case EL_ERROR_LEVEL_MIDDLE:
      EL_record_event_((EL_GROUP)EL_CORE_GROUP_EVENT_LOGGER,
                       EL_EVENT_LOCAL_TLOG_MIDDLE_FULL,
                       EL_ERROR_LEVEL_EL,
                       0);
      break;
#endif

    case EL_ERROR_LEVEL_LOW:
      EL_record_event_((EL_GROUP)EL_CORE_GROUP_EVENT_LOGGER,
                       EL_EVENT_LOCAL_TLOG_LOW_FULL,
                       EL_ERROR_LEVEL_EL,
                       0);
      break;

    default:
      // EL_ERROR_LEVEL_EL �̂��̂́C�������[�v�̉\��������̂ŁC�C�x���g���s���Ȃ�
      break;
    }
  }

  if (wp == (capacity / 2) && (capacity % 2 ) == 0)   // �񔼕��ɒB�����Ƃ�
  {
    switch (err_level)
    {
    case EL_ERROR_LEVEL_HIGH:
      EL_record_event_((EL_GROUP)EL_CORE_GROUP_EVENT_LOGGER,
                       EL_EVENT_LOCAL_TLOG_HIGH_HALF_FULL,
                       EL_ERROR_LEVEL_EL,
                       0);
      break;

#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
    case EL_ERROR_LEVEL_MIDDLE:
      EL_record_event_((EL_GROUP)EL_CORE_GROUP_EVENT_LOGGER,
                       EL_EVENT_LOCAL_TLOG_MIDDLE_HALF_FULL,
                       EL_ERROR_LEVEL_EL,
                       0);
      break;
#endif

    case EL_ERROR_LEVEL_LOW:
      EL_record_event_((EL_GROUP)EL_CORE_GROUP_EVENT_LOGGER,
                       EL_EVENT_LOCAL_TLOG_LOW_HALF_FULL,
                       EL_ERROR_LEVEL_EL,
                       0);
      break;

    default:
      // EL_ERROR_LEVEL_EL �̂��̂́C�������[�v�̉\��������̂ŁC�C�x���g���s���Ȃ�
      break;
    }
  }
}
#endif


static void EL_clear_all_tlog_(void)
{
  uint8_t err_level;
  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    EL_clear_tlog_((EL_ERROR_LEVEL)err_level);
  }
}


static void EL_clear_tlog_(EL_ERROR_LEVEL err_level)
{
  EL_TimeSeriesLog* p_tlog = &event_logger_.tlogs[err_level];

  p_tlog->is_table_overflow = 0;
  p_tlog->log_wp = 0;

  memset(p_tlog->events, 0x00, p_tlog->log_capacity * sizeof(EL_Event));
}
#endif


#ifdef EL_IS_ENABLE_CLOG
static void EL_record_event_to_clog_(const EL_Event* event)
{
  uint16_t log_idx;
  const EL_ERROR_LEVEL err_level = event->err_level;
  EL_CLogElement* p_newest_clog_log;

  EL_move_to_front_in_clog_(event);

  // ���g�̃��Oidx�C�܂�ł��V�������Oidx���E��
  log_idx = event_logger_.clogs[err_level].log_orders[0];
  p_newest_clog_log = &event_logger_.clogs[err_level].logs[log_idx];

  p_newest_clog_log->count++;
  p_newest_clog_log->delta_record_time = OBCT_diff(&p_newest_clog_log->event.time, &event->time);

  p_newest_clog_log->event = *event;
}


static void EL_clear_all_clog_(void)
{
  uint8_t err_level;
  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    EL_clear_clog_((EL_ERROR_LEVEL)err_level);
  }
}


static void EL_clear_clog_(EL_ERROR_LEVEL err_level)
{
  EL_CumulativeLog* p_clog = &event_logger_.clogs[err_level];
  uint16_t i;
  const uint16_t capacity = p_clog->log_capacity;

  memset(p_clog->logs, 0x00, capacity * sizeof(EL_CLogElement));

  for (i = 0; i < capacity; ++i)
  {
    p_clog->log_orders[i] = (uint16_t)i;
  }
}


static EL_CLOG_LOG_ACK EL_search_clog_(const EL_Event* event, uint16_t* log_idx, uint16_t* order_idx)
{
  uint16_t i;
  EL_CumulativeLog* p_clog = &event_logger_.clogs[event->err_level];
  const uint16_t capacity = p_clog->log_capacity;
  *log_idx = 0;
  *order_idx = 0;

  for (i = 0; i < capacity; ++i)
  {
    uint16_t idx = p_clog->log_orders[i];
    if (p_clog->logs[idx].event.group == event->group &&
        p_clog->logs[idx].event.local == event->local)
    {
      *log_idx  = idx;
      *order_idx = i;
      return EL_CLOG_LOG_ACK_OK;
    }
  }

  return EL_CLOG_LOG_ACK_NOT_FOUND;
}


static void EL_move_to_front_in_clog_(const EL_Event* event)
{
  uint16_t i;
  uint16_t log_idx;
  uint16_t order_idx;
  EL_CumulativeLog* p_clog = &event_logger_.clogs[event->err_level];
  EL_CLOG_LOG_ACK log_ret = EL_search_clog_(event, &log_idx, &order_idx);

  if (log_ret == EL_CLOG_LOG_ACK_NOT_FOUND)
  {
    // ���O���Ȃ��̂ō��
    EL_create_clog_on_front_(event);
    return;
  }

  for (i = order_idx; i > 0; --i)
  {
    p_clog->log_orders[i] = p_clog->log_orders[i - 1];
  }
  p_clog->log_orders[0] = log_idx;
}


static void EL_create_clog_on_front_(const EL_Event* event)
{
  uint16_t i;
  uint16_t log_idx;
  const EL_ERROR_LEVEL err_level = event->err_level;
  EL_CumulativeLog* p_clog = &event_logger_.clogs[err_level];
  const uint16_t capacity = p_clog->log_capacity;

  // �ł��Â����O���擾
  log_idx = p_clog->log_orders[capacity - 1];

#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  {
    const EL_CLogElement* drop_clog_log = &p_clog->logs[log_idx];
    if (drop_clog_log->event.group != (EL_GROUP)EL_CORE_GROUP_NULL)
    {
      // �ꉞ�L�^�̂��߂ɃC�x���g�𔭍s����
      // �����ŁC EL ���o���Ă��܂��ƍċA & �ň��������[�v�ɂȂ�
      if (err_level != EL_ERROR_LEVEL_EL)
      {
        EL_record_event_((EL_GROUP)EL_CORE_GROUP_EL_DROP_CLOG1,
                         (uint32_t)drop_clog_log->event.group,
                         EL_ERROR_LEVEL_EL,
                         (uint32_t)drop_clog_log->event.err_level);
        EL_record_event_((EL_GROUP)EL_CORE_GROUP_EL_DROP_CLOG2,
                         drop_clog_log->event.local,
                         EL_ERROR_LEVEL_EL,
                         (uint32_t)drop_clog_log->count);
      }
    }
  }
#endif

  for (i = (uint16_t)(capacity - 1); i > 0; --i)
  {
    p_clog->log_orders[i] = p_clog->log_orders[i - 1];
  }
  p_clog->log_orders[0] = log_idx;

  // ������ event ������Ă��܂�
  // �������邱�ƂŁC����� delta_record_time �� 0 �ɂȂ�
  p_clog->logs[log_idx].event = *event;
  p_clog->logs[log_idx].count = 0;

  // delta_record_time �͂Ƃ肠�����͏����ĂȂ����C����ɏ㏑�������͂�
}
#endif


static void EL_clear_latest_event_(void)
{
  memset(&event_logger_.latest_event, 0x00, sizeof(EL_Event));
}


static void EL_clear_statistics_(void)
{
  uint8_t err_level;

  event_logger_.statistics.record_counter_total = 0;

  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    event_logger_.statistics.record_counters[err_level] = 0;
  }

  EH_match_event_counter_to_el();
}


EL_ACK EL_enable_logging(EL_GROUP group)
{
  uint32_t group_idx    = (uint32_t)group / 8;
  uint32_t group_subidx = 7 - (uint32_t)group % 8;    // ���]
  uint8_t  info;
  uint8_t  mask;

  if (group >= EL_GROUP_MAX)       return EL_ACK_ILLEGAL_GROUP;
  if (group <= EL_CORE_GROUP_NULL) return EL_ACK_ILLEGAL_GROUP;     // ����͖{���ɓ���Ă悢���v�c�_

  info = event_logger_.is_logging_enable[group_idx];
  mask = (uint8_t)(0x01 << group_subidx);
  info = (uint8_t)(info | mask);

  event_logger_.is_logging_enable[group_idx] = info;

  return EL_ACK_OK;
}


EL_ACK EL_disable_logging(EL_GROUP group)
{
  uint32_t group_idx    = (uint32_t)group / 8;
  uint32_t group_subidx = 7 - (uint32_t)group % 8;    // ���]
  uint8_t  info;
  uint8_t  mask;

  if (group >= EL_GROUP_MAX)       return EL_ACK_ILLEGAL_GROUP;
  if (group <= EL_CORE_GROUP_NULL) return EL_ACK_ILLEGAL_GROUP;     // ����͖{���ɓ���Ă悢���v�c�_

  info = event_logger_.is_logging_enable[group_idx];
  mask = (uint8_t)(0x01 << group_subidx);
  mask = (uint8_t)(~mask);                     // �r�b�g���]
  info = (uint8_t)(info & mask);

  event_logger_.is_logging_enable[group_idx] = info;

  return EL_ACK_OK;
}


int EL_is_logging_enable(EL_GROUP group)
{
  uint32_t group_idx    = (uint32_t)group / 8;
  uint32_t group_subidx = 7 - (uint32_t)group % 8;    // ���]
  uint8_t  info;
  uint8_t  mask;
  uint8_t  ret;

  // �s���� group �͖�������
  if (group >= EL_GROUP_MAX) return 0;

  info = event_logger_.is_logging_enable[group_idx];
  mask = (uint8_t)(0x01 << group_subidx);
  ret  = (uint8_t)(info & mask);

  if (ret == 0) return 0;
  return 1;
}


void EL_enable_all_logging(void)
{
  memset(event_logger_.is_logging_enable, 0xFF, sizeof(event_logger_.is_logging_enable));
}


void EL_disable_all_logging(void)
{
  memset(event_logger_.is_logging_enable, 0x00, sizeof(event_logger_.is_logging_enable));
}


#ifdef EL_IS_ENABLE_TLOG
EL_ACK EL_enable_tlog_overwrite(EL_ERROR_LEVEL err_level)
{
  if (err_level < 0)                   return EL_ACK_ILLEGAL_ERROR_LEVEL;
  if (err_level >= EL_ERROR_LEVEL_MAX) return EL_ACK_ILLEGAL_ERROR_LEVEL;

  event_logger_.tlogs[err_level].is_enable_overwrite = 1;
  return EL_ACK_OK;
}


EL_ACK EL_disable_tlog_overwrite(EL_ERROR_LEVEL err_level)
{
  if (err_level < 0)                   return EL_ACK_ILLEGAL_ERROR_LEVEL;
  if (err_level >= EL_ERROR_LEVEL_MAX) return EL_ACK_ILLEGAL_ERROR_LEVEL;

  event_logger_.tlogs[err_level].is_enable_overwrite = 0;
  return EL_ACK_OK;
}


void EL_enable_tlog_overwrite_all(void)
{
  uint8_t err_level;

  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    EL_enable_tlog_overwrite((EL_ERROR_LEVEL)err_level);
  }
}


void EL_disable_tlog_overwrite_all(void)
{
  uint8_t err_level;

  for (err_level = 0; err_level < EL_ERROR_LEVEL_MAX; ++err_level)
  {
    EL_disable_tlog_overwrite((EL_ERROR_LEVEL)err_level);
  }
}


int EL_is_tlog_overwrite_enable(EL_ERROR_LEVEL err_level)
{
  if (err_level < 0)                   return 0;
  if (err_level >= EL_ERROR_LEVEL_MAX) return 0;

  return (int)event_logger_.tlogs[err_level].is_enable_overwrite;
}


const EL_Event* EL_get_the_nth_tlog_from_the_latest(EL_ERROR_LEVEL err_level, uint16_t n)
{
  uint16_t capacity;
  uint16_t idx;
  EL_TimeSeriesLog* p_tlog;

  if (err_level < 0 || err_level >= EL_ERROR_LEVEL_MAX)
  {
    // �d�����Ȃ��̂� HIGH��
    err_level = EL_ERROR_LEVEL_HIGH;
  }

  p_tlog = &event_logger_.tlogs[err_level];
  capacity = p_tlog->log_capacity;

  if (n >= capacity)
  {
    // �d�����Ȃ��̂ŁC�ŐV�̂��̂�
    idx = 0;
  }
  else
  {
    idx = (uint16_t)((p_tlog->log_wp - 1 - n + capacity) % capacity);
  }

  return &p_tlog->events[idx];
}
#endif


CCP_EXEC_STS Cmd_EL_INIT(const CTCP* packet)
{
  (void)packet;
  EL_initialize();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EL_CLEAR_LOG_ALL(const CTCP* packet)
{
  (void)packet;

#ifdef EL_IS_ENABLE_TLOG
  EL_clear_all_tlog_();
#endif

#ifdef EL_IS_ENABLE_CLOG
  EL_clear_all_clog_();
#endif

  EL_clear_latest_event_();
  EL_clear_statistics_();

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EL_CLEAR_LOG_BY_ERR_LEVEL(const CTCP* packet)
{
  EL_ERROR_LEVEL err_level = (EL_ERROR_LEVEL)CCP_get_param_head(packet)[0];

  if (err_level < 0) return CCP_EXEC_ILLEGAL_PARAMETER;
  if (err_level >= EL_ERROR_LEVEL_MAX) return CCP_EXEC_ILLEGAL_PARAMETER;

#ifdef EL_IS_ENABLE_TLOG
  EL_clear_tlog_(err_level);
#endif

#ifdef EL_IS_ENABLE_CLOG
  EL_clear_clog_(err_level);
#endif

  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EL_CLEAR_STATISTICS(const CTCP* packet)
{
  (void)packet;
  EL_clear_statistics_();
  return CCP_EXEC_SUCCESS;
}


#ifdef EL_IS_ENABLE_TLOG
CCP_EXEC_STS Cmd_EL_CLEAR_TLOG(const CTCP* packet)
{
  EL_ERROR_LEVEL err_level = (EL_ERROR_LEVEL)CCP_get_param_head(packet)[0];

  if (err_level < 0) return CCP_EXEC_ILLEGAL_PARAMETER;
  if (err_level >= EL_ERROR_LEVEL_MAX) return CCP_EXEC_ILLEGAL_PARAMETER;

  EL_clear_tlog_(err_level);

  return CCP_EXEC_SUCCESS;
}
#endif


#ifdef EL_IS_ENABLE_CLOG
CCP_EXEC_STS Cmd_EL_CLEAR_CLOG(const CTCP* packet)
{
  EL_ERROR_LEVEL err_level = (EL_ERROR_LEVEL)CCP_get_param_head(packet)[0];

  if (err_level < 0) return CCP_EXEC_ILLEGAL_PARAMETER;
  if (err_level >= EL_ERROR_LEVEL_MAX) return CCP_EXEC_ILLEGAL_PARAMETER;

  EL_clear_clog_(err_level);

  return CCP_EXEC_SUCCESS;
}
#endif


CCP_EXEC_STS Cmd_EL_RECORD_EVENT(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  EL_ACK ack;
  EL_GROUP group;             // uint32_t ��z��
  uint32_t local;
  EL_ERROR_LEVEL err_level;   // uint8_t ��z��
  uint32_t note;

  uint32_t temp;

  endian_memcpy(&temp, &param[0], 4);
  group = (EL_GROUP)temp;
  endian_memcpy(&local, &param[4], 4);
  err_level = (EL_ERROR_LEVEL)param[8];
  endian_memcpy(&note, &param[9], 4);

  ack = EL_record_event(group, local, err_level, note);

  switch (ack)
  {
  case EL_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EL_ACK_TLOG_FULL:
    // �v���������C����͐���ł͂���̂ł���ł悵
    return CCP_EXEC_SUCCESS;
  case EL_ACK_ILLEGAL_GROUP:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case EL_ACK_ILLEGAL_ERROR_LEVEL:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case EL_ACK_DISABLE_LOGGING:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


#ifdef EL_IS_ENABLE_TLOG
CCP_EXEC_STS Cmd_EL_TLOG_SET_PAGE_FOR_TLM(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint8_t page_no;
  EL_ERROR_LEVEL err_level;

  page_no = param[0];
  err_level = (EL_ERROR_LEVEL)param[1];

  switch (err_level)
  {
  case EL_ERROR_LEVEL_HIGH:
    if (page_no >= EL_TLOG_TLM_PAGE_MAX_HIGH) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
  case EL_ERROR_LEVEL_MIDDLE:
    if (page_no >= EL_TLOG_TLM_PAGE_MAX_MIDDLE) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#endif
  case EL_ERROR_LEVEL_LOW:
    if (page_no >= EL_TLOG_TLM_PAGE_MAX_LOW) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  case EL_ERROR_LEVEL_EL:
    // TODO: ����͗v����
    if (page_no != 0) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#endif
  default:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  event_logger_.tlm_info.tlog.page_no = page_no;
  event_logger_.tlm_info.tlog.err_level = err_level;

  return CCP_EXEC_SUCCESS;
}
#endif


#ifdef EL_IS_ENABLE_CLOG
CCP_EXEC_STS Cmd_EL_CLOG_SET_PAGE_FOR_TLM(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint8_t page_no;
  EL_ERROR_LEVEL err_level;

  page_no = param[0];
  err_level = (EL_ERROR_LEVEL)param[1];

  switch (err_level)
  {
  case EL_ERROR_LEVEL_HIGH:
    if (page_no >= EL_CLOG_TLM_PAGE_MAX_HIGH) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
  case EL_ERROR_LEVEL_MIDDLE:
    if (page_no >= EL_CLOG_TLM_PAGE_MAX_MIDDLE) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#endif
  case EL_ERROR_LEVEL_LOW:
    if (page_no >= EL_CLOG_TLM_PAGE_MAX_LOW) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  case EL_ERROR_LEVEL_EL:
    // TODO: ����͗v����
    if (page_no != 0) return CCP_EXEC_ILLEGAL_PARAMETER;
    break;
#endif
  default:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  event_logger_.tlm_info.clog.page_no = page_no;
  event_logger_.tlm_info.clog.err_level = err_level;

  return CCP_EXEC_SUCCESS;
}
#endif


CCP_EXEC_STS Cmd_EL_INIT_LOGGING_SETTINGS(const CTCP* packet)
{
  (void)packet;
  EL_enable_all_logging();
  EL_load_default_settings();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EL_ENABLE_LOGGING(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  EL_ACK ack;
  EL_GROUP group;

  uint32_t temp;

  endian_memcpy(&temp, param, 4);
  group = (EL_GROUP)temp;

  ack = EL_enable_logging(group);

  switch (ack)
  {
  case EL_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EL_ACK_ILLEGAL_GROUP:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EL_DISABLE_LOGGING(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  EL_ACK ack;
  EL_GROUP group;

  uint32_t temp;

  endian_memcpy(&temp, param, 4);
  group = (EL_GROUP)temp;

  ack = EL_disable_logging(group);

  switch (ack)
  {
  case EL_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EL_ACK_ILLEGAL_GROUP:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EL_ENABLE_LOGGING_ALL(const CTCP* packet)
{
  (void)packet;
  EL_enable_all_logging();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EL_DISABLE_LOGGING_ALL(const CTCP* packet)
{
  (void)packet;
  EL_disable_all_logging();
  return CCP_EXEC_SUCCESS;
}


#ifdef EL_IS_ENABLE_TLOG
CCP_EXEC_STS Cmd_EL_ENABLE_TLOG_OVERWRITE(const CTCP* packet)
{
  EL_ERROR_LEVEL err_level = (EL_ERROR_LEVEL)CCP_get_param_head(packet)[0];
  EL_ACK ack;

  ack = EL_enable_tlog_overwrite(err_level);

  switch (ack)
  {
  case EL_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EL_ACK_ILLEGAL_ERROR_LEVEL:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EL_DISABLE_TLOG_OVERWRITE(const CTCP* packet)
{
  EL_ERROR_LEVEL err_level = (EL_ERROR_LEVEL)CCP_get_param_head(packet)[0];
  EL_ACK ack;

  ack = EL_disable_tlog_overwrite(err_level);

  switch (ack)
  {
  case EL_ACK_OK:
    return CCP_EXEC_SUCCESS;
  case EL_ACK_ILLEGAL_ERROR_LEVEL:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  default:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}


CCP_EXEC_STS Cmd_EL_ENABLE_TLOG_OVERWRITE_ALL(const CTCP* packet)
{
  (void)packet;
  EL_enable_tlog_overwrite_all();
  return CCP_EXEC_SUCCESS;
}


CCP_EXEC_STS Cmd_EL_DISABLE_TLOG_OVERWRITE_ALL(const CTCP* packet)
{
  (void)packet;
  EL_disable_tlog_overwrite_all();
  return CCP_EXEC_SUCCESS;
}
#endif

#pragma section
