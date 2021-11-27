/**
 * @file  event_logger.h
 * @brief �A�m�}����G���[�Ȃǂ̊e��C�x���g���L�^����
 * @note  ���̃C�x���g�����Ƃ� event_handler �𔭉΂����邱�Ƃ��ł���
 * @note  ���O�� TLog (TimeSeriesLog) �� CLog (EL_CumulativeLog) �̓��ނ��������C�K�v�Ȃ��݂̂̂��g��
 * @note  ����C NvLog (�s����Log) �������\��
 * @note  TLog (TimeSeriesLog)
 *        ���n�񃍃O
 *        �Ђ�����C�V���� EL_Event ���e�[�u���ɒǋL���Ă����D
 *        EL_get_the_nth_tlog_from_the_latest �ōŐV����n�Ԗڂ��擾�ł���D
 *        �����L�������Ȃ��ƁC event_handler �͎g���Ȃ��D
 *        �e�[�u�������t�ɂȂ����ꍇ�C�㏑�����邩���Ȃ����� is_enable_overwrite �ɂđI���\�D
 * @note  CLog (EL_CumulativeLog)
 *        �ݐσ��O
 *        EL_Event (EL_Event.group, EL_Event.local �ɂē���) ���Ƃ̗ݐσJ�E���^��ێ����C���n��\�[�g�����D
 *        �e�[�u�������t�ɂȂ�ƍł��Â����̂��̂Ă���D
 *        �ȉ��̂悤�ȓ���������
 *          �t�H�[�}�b�g:
 *            | log index | event | counter | time | dt |
 *          1. ���� t1: event1 ������
 *             | 0 | event1 | 1 | t1 | 0 |
 *          2. ���� t2: event2 ������
 *             | 0 | event2 | 1 | t2 | 0 |
 *             | 1 | event1 | 1 | t1 | 0 |
 *          3. ���� t3: event2 ������
 *             | 0 | event2 | 2 | t3 | t3 - t2 |
 *             | 1 | event1 | 1 | t1 | 0       |
 *          4. ���� t4: event1 ������
 *             | 0 | event1 | 2 | t4 | t4 - t1 |
 *             | 1 | event2 | 2 | t3 | t3 - t2 |
 *          5. ���� t5: event3 ������
 *             | 0 | event3 | 1 | t5 | 0       |
 *             | 1 | event1 | 2 | t4 | t4 - t1 |
 *             | 2 | event2 | 2 | t3 | t3 - t2 |
 */
#ifndef EVENT_LOGGER_H_
#define EVENT_LOGGER_H_

// TODO: FALSH���������̑�e�ʕs�����{�����[���ւ̏�������
// TODO: AL �� EL ���̂��̑S�̂��r���h����C���Ȃ�������ׂ� ifdef ��ǉ��i�������̂��߁j
// TODO: ObcTime �̏������� OBCT_clear �ł͂Ȃ� memset 0x00 �ł���i������Ȃ����C������ɂȂ邩���j

#include <stddef.h>
#include <src_user/Library/stdint.h>
#include "../../CmdTlm/common_tlm_cmd_packet.h"
#include "../TimeManager/obc_time.h"

// EL_GROUP (uint32_t ��z��) �������Œ�`����
#include <src_user/Settings/System/event_logger_group.h>

#define EL_TLOG_TLM_PAGE_SIZE         (32)    //!< TLog�ɂāC�e�����P�y�[�W�ɂ��낹��log��
#define EL_TLOG_TLM_PAGE_MAX_HIGH     (4)     //!< TLog�ŕۑ����郍�O�̃y�[�W�� (EL_ERROR_LEVEL_HIGH)
#define EL_TLOG_TLM_PAGE_MAX_MIDDLE   (4)     //!< TLog�ŕۑ����郍�O�̃y�[�W�� (EL_ERROR_LEVEL_MIDDLE)
#define EL_TLOG_TLM_PAGE_MAX_LOW      (4)     //!< TLog�ŕۑ����郍�O�̃y�[�W�� (EL_ERROR_LEVEL_LOW)
#define EL_CLOG_TLM_PAGE_SIZE         (32)    //!< CLog�ɂāC�e�����P�y�[�W�ɂ��낹��log��
#define EL_CLOG_TLM_PAGE_MAX_HIGH     (4)     //!< CLog�ŕۑ����郍�O�̃y�[�W�� (EL_ERROR_LEVEL_HIGH)
#define EL_CLOG_TLM_PAGE_MAX_MIDDLE   (4)     //!< CLog�ŕۑ����郍�O�̃y�[�W�� (EL_ERROR_LEVEL_MIDDLE)
#define EL_CLOG_TLM_PAGE_MAX_LOW      (4)     //!< CLog�ŕۑ����郍�O�̃y�[�W�� (EL_ERROR_LEVEL_LOW)

#define EL_TLOG_LOG_SIZE_MAX_EL       (16)    //!< TLog�ɂĕۑ����郍�O�� (EL_ERROR_LEVEL_EL)
#define EL_CLOG_LOG_SIZE_MAX_EL       (8)     //!< CLog�ɂĕۑ����郍�O�� (EL_ERROR_LEVEL_EL)

#define EL_IS_ENABLE_TLOG                     //!< TLog ��L���ɂ��邩�H �� ���ꂪ�Ȃ��� event_handler �͎g���Ȃ�
#define EL_IS_ENABLE_CLOG                     //!< CLog ��L���ɂ��邩�H
// #define EL_IS_ENABLE_NVLOG                 //!< TODO: NvLog (�s����Log) ��L���ɂ��邩�H
#define EL_IS_ENABLE_EVENT_NOTE               //!< EL_Event.note ��L���ɂ��邩�H event �Ƃ��Ă� (event_handler �Ȃǂ�) ���������C�⑫�����ʗp�� id ���g�����H
#define EL_IS_ENABLE_MIDDLE_ERROR_LEVEL       //!< EL_ERROR_LEVEL_MIDDLE ��L���ɂ��邩�H �iC2A core�ł͎g��Ȃ��̂ŁC��{������OK�D�g�������Ȃ�L���Ɂj
#define EL_IS_ENABLE_EL_ERROR_LEVEL           /*!< EL_ERROR_LEVEL_EL ��L���ɂ��邩�H
                                                   �ċA�Ăяo�� & �������[�v��h�����߂ɁCEL�����̃C�x���g�͕ʂ̐�p�̃G���[���x���ŋL�^�����D
                                                   �����L��������ƁCEL�����̃A�m�}�����L�^�ł���D */

#define SIZE_OF_EL_CLOG_COUNTER_T      (2)    //!< el_clog_counter_t �̌^�T�C�Y�D�������C���p�N�g������Ȃ�ɂ���̂ŁC�ς�

// �ȉ� event_logger_params.h �ɂāC�ȉ��̂悤�Ȑݒ���`����D��Ƀ������C���p�N�g�̂��߂ɒ����\�ɂ��Ă���D
// �Ȃ��C������Ɠ���m�F����Ă���̂́CEL_IS_ENABLE_MIDDLE_ERROR_LEVEL �̂ݖ����ȃ��[�h�̂Ƃ�
// #define EL_IS_ENABLE_TLOG: TLog ��L���ɂ���H
// #define EL_IS_ENABLE_CLOG: CLog ��L���ɂ���H
// #define EL_IS_ENABLE_EVENT_NOTE: EL_Event.note ��L���ɂ���H
// #define EL_IS_ENABLE_MIDDLE_ERROR_LEVEL: EL_ERROR_LEVEL_MIDDLE ���g����悤�ɂ��邩�H
// #define EL_IS_ENABLE_EL_ERROR_LEVEL: EL_ERROR_LEVEL_EL ���g����悤�ɂ��邩�H
// SIZE_OF_EL_CLOG_COUNTER_T: el_clog_counter_t �̌^�T�C�Y
// �㕔�Œ�`����Ă���C�e��table�T�C�Y
#include <src_user/Settings/System/event_logger_params.h>

#define EL_LOG_MINIMUM_REQUIRED           (2)   //!< ���O�ɋ��߂���ŏ��v�f��
#define EL_LOG_MINIMUM_REQUIRED_EL_LOG    (4)   //!< EL���O�ɋ��߂���ŏ��v�f��

#ifdef EL_IS_ENABLE_TLOG

#if EL_TLOG_TLM_PAGE_SIZE < EL_LOG_MINIMUM_REQUIRED
#error EL_TLOG_TLM_PAGE_SIZE must be >= EL_LOG_MINIMUM_REQUIRED
#endif

#define EL_TLOG_LOG_SIZE_MAX_HIGH     (EL_TLOG_TLM_PAGE_SIZE * EL_TLOG_TLM_PAGE_MAX_HIGH)
#if EL_TLOG_TLM_PAGE_MAX_HIGH <= 0
#error EL_TLOG_TLM_PAGE_MAX_HIGH must be a positive number
#endif
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#define EL_TLOG_LOG_SIZE_MAX_MIDDLE   (EL_TLOG_TLM_PAGE_SIZE * EL_TLOG_TLM_PAGE_MAX_MIDDLE)
#if EL_TLOG_TLM_PAGE_MAX_MIDDLE <= 0
#error EL_TLOG_TLM_PAGE_MAX_MIDDLE must be a positive number
#endif
#else   // EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#undef EL_TLOG_TLM_PAGE_MAX_MIDDLE
#endif  // EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#define EL_TLOG_LOG_SIZE_MAX_LOW      (EL_TLOG_TLM_PAGE_SIZE * EL_TLOG_TLM_PAGE_MAX_LOW)
#if EL_TLOG_TLM_PAGE_MAX_LOW <= 0
#error EL_TLOG_TLM_PAGE_MAX_LOW must be a positive number
#endif

#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
#if EL_TLOG_LOG_SIZE_MAX_EL < EL_LOG_MINIMUM_REQUIRED_EL_LOG
#error EL_TLOG_LOG_SIZE_MAX_EL is too small
#endif
#else   // EL_IS_ENABLE_EL_ERROR_LEVEL
#undef EL_TLOG_LOG_SIZE_MAX_EL
#endif  // EL_IS_ENABLE_EL_ERROR_LEVEL

#else   // EL_IS_ENABLE_TLOG

#undef EL_TLOG_TLM_PAGE_SIZE
#undef EL_TLOG_TLM_PAGE_MAX_HIGH
#undef EL_TLOG_TLM_PAGE_MAX_MIDDLE
#undef EL_TLOG_TLM_PAGE_MAX_LOW
#undef EL_TLOG_LOG_SIZE_MAX_EL

#endif  // EL_IS_ENABLE_TLOG


#ifdef EL_IS_ENABLE_CLOG

#if EL_CLOG_TLM_PAGE_SIZE < EL_LOG_MINIMUM_REQUIRED
#error EL_CLOG_TLM_PAGE_SIZE must be >= EL_LOG_MINIMUM_REQUIRED
#endif

#define EL_CLOG_LOG_SIZE_MAX_HIGH     (EL_CLOG_TLM_PAGE_SIZE * EL_CLOG_TLM_PAGE_MAX_HIGH)
#if EL_CLOG_TLM_PAGE_MAX_HIGH <= 0
#error EL_CLOG_TLM_PAGE_MAX_HIGH must be a positive number
#endif
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#define EL_CLOG_LOG_SIZE_MAX_MIDDLE   (EL_CLOG_TLM_PAGE_SIZE * EL_CLOG_TLM_PAGE_MAX_MIDDLE)
#if EL_CLOG_TLM_PAGE_MAX_MIDDLE <= 0
#error EL_CLOG_TLM_PAGE_MAX_MIDDLE must be a positive number
#endif
#else   // EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#undef EL_CLOG_TLM_PAGE_MAX_MIDDLE
#endif  // EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
#define EL_CLOG_LOG_SIZE_MAX_LOW      (EL_CLOG_TLM_PAGE_SIZE * EL_CLOG_TLM_PAGE_MAX_LOW)
#if EL_CLOG_TLM_PAGE_MAX_LOW <= 0
#error EL_CLOG_TLM_PAGE_MAX_LOW must be a positive number
#endif

#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
#if EL_CLOG_LOG_SIZE_MAX_EL < EL_LOG_MINIMUM_REQUIRED_EL_LOG
#error EL_CLOG_LOG_SIZE_MAX_EL is too small
#endif
#else   // EL_IS_ENABLE_EL_ERROR_LEVEL
#undef EL_CLOG_LOG_SIZE_MAX_EL
#endif  // EL_IS_ENABLE_EL_ERROR_LEVEL

#if SIZE_OF_EL_CLOG_COUNTER_T == 1
typedef uint8_t el_clog_counter_t;
#elif SIZE_OF_EL_CLOG_COUNTER_T == 2
typedef uint16_t el_clog_counter_t;
#elif SIZE_OF_EL_CLOG_COUNTER_T == 4
typedef uint32_t el_clog_counter_t;
#else
#error Illegal value for SIZE_OF_EL_CLOG_COUNTER_T
#endif

#else    // EL_IS_ENABLE_CLOG

#undef EL_CLOG_TLM_PAGE_SIZE
#undef EL_CLOG_TLM_PAGE_MAX_HIGH
#undef EL_CLOG_TLM_PAGE_MAX_MIDDLE
#undef EL_CLOG_TLM_PAGE_MAX_LOW
#undef EL_CLOG_LOG_SIZE_MAX_EL

#endif  // EL_IS_ENABLE_CLOG


/**
 * @enum  EL_CORE_GROUP
 * @brief event_logger �� Event Group �� C2A core �g�p��
 * @note  uint32_t ��z��
 */
typedef enum
{
  EL_CORE_GROUP_NULL = 0,             //!< �����l or NULL�D����� 0 �ł��邱�Ƃ�v�� (clear �� memset 0x00 ������Ȃ�)
  EL_CORE_GROUP_EVENT_LOGGER,         //!< EL
  EL_CORE_GROUP_MODE_MANAGER,
  EL_CORE_GROUP_TASK_DISPATCHER,
  EL_CORE_GROUP_APP_MANAGER,
  EL_CORE_GROUP_TLCD_PAST_TIME,
  EL_CORE_GROUP_TLCD_DEPLOY_BLOCK,
  EL_CORE_GROUP_DCU,
  EL_CORE_GROUP_EVENT_HANDLER,
  // TODO: Driver Super
  // TODO: Cmd exec
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  EL_CORE_GROUP_EL_DROP_CLOG1,        //!< EL CLogs �ŌÂ��G���[���㏑������Ƃ� (group, err_level ��ۑ�)
  EL_CORE_GROUP_EL_DROP_CLOG2,        //!< EL CLogs �ŌÂ��G���[���㏑������Ƃ� (local, count ��ۑ�)
#endif
  EL_CORE_GROUP_MAX
} EL_CORE_GROUP;


#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
/**
 * @enum  EL_EVENT_LOCAL
 * @brief EL_CORE_GROUP_EVENT_LOGGER �� local id
 * @note  uint32_t ��z��
 */
typedef enum
{
  EL_EVENT_LOCAL_UNKNOWN_ERR,             //!< �s���̃G���[
#ifdef EL_IS_ENABLE_TLOG
  EL_EVENT_LOCAL_TLOG_HIGH_HALF_FULL,     //!< TLog �̃e�[�u���̔��������܂��� (HIGH)
  EL_EVENT_LOCAL_TLOG_HIGH_FULL,          //!< TLog �̃e�[�u�������܂����Dwp�͍ŏ��ɖ߂�C�Â����̂���㏑�����Ă��� (HIGH)
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
  EL_EVENT_LOCAL_TLOG_MIDDLE_HALF_FULL,   //!< TLog �̃e�[�u���̔��������܂��� (MIDDLE)
  EL_EVENT_LOCAL_TLOG_MIDDLE_FULL,        //!< TLog �̃e�[�u�������܂����Dwp�͍ŏ��ɖ߂�C�Â����̂���㏑�����Ă��� (MIDDLE)
#endif
  EL_EVENT_LOCAL_TLOG_LOW_HALF_FULL,      //!< TLog �̃e�[�u���̔��������܂��� (LOW)
  EL_EVENT_LOCAL_TLOG_LOW_FULL,           //!< TLog �̃e�[�u�������܂����Dwp�͍ŏ��ɖ߂�C�Â����̂���㏑�����Ă��� (LOW)
#endif
  EL_EVENT_LOCAL_MAX
} EL_EVENT_LOCAL;
#endif


/**
 * @enum  EL_ERROR_LEVEL
 * @brief event_logger �� �G���[���x��
 * @note  uint8_t ��z��
 * @note  C2A Core �ł� HIGH, LOW �����g��Ȃ����߁C MIDDLE �̓��[�U�[��`�Ƃ���
 */
typedef enum
{
  EL_ERROR_LEVEL_HIGH = 0,
#ifdef EL_IS_ENABLE_MIDDLE_ERROR_LEVEL
  EL_ERROR_LEVEL_MIDDLE,
#endif
  EL_ERROR_LEVEL_LOW,
#ifdef EL_IS_ENABLE_EL_ERROR_LEVEL
  EL_ERROR_LEVEL_EL,
#endif
  EL_ERROR_LEVEL_MAX
} EL_ERROR_LEVEL;


/**
 * @enum  EL_ACK
 * @brief event_logger �̔ėp�Ԃ�l
 * @note  uint8_t ��z��
 */
typedef enum
{
  EL_ACK_OK,                    //!< ����I��
  EL_ACK_ILLEGAL_GROUP,         //!< �s���� EL_Event.group
  EL_ACK_ILLEGAL_ERROR_LEVEL,   //!< �s���� EL_Event.err_level
  EL_ACK_DISABLE_LOGGING,       //!< ���M���O�������ɂ��C���p
  EL_ACK_TLOG_FULL,             //!< TLog�̒ǋL���s�\�̂��߁CTLog�L�^���ł���
  EL_ACK_OTHER_ERR,             //!< ���̑��̃G���[
} EL_ACK;


/**
 * @struct EL_Event
 * @brief  �C�x���g
 */
typedef struct
{
  EL_GROUP group;             //!< �C�x���g����肷�� group id
  uint32_t local;             /*!< ���ꂼ��� group ���Ƃɏڍ׏���t�^���邽�߂� id
                                   enum�ł��悢���C�Ȃɂ��̃G���[�R�[�h�ł��ǂ����C�Ⴆ�Ώ������݂����s�����������A�h���X�C�Ȃǂł��ǂ�
                                   group���Ƃɒ�`���� */
  EL_ERROR_LEVEL err_level;   //!< �G���[���x��
  ObcTime  time;              //!< ��������
#ifdef EL_IS_ENABLE_EVENT_NOTE
  uint32_t note;              /*!< Event �� group, local �œ��肳��邪�C����ɕ⑫����ǉ�����ꍇ�Ɏg���D
                                   event_handler �Ȃǂł͖��������
                                   �⑫����ǉ�����ꍇ��C���� event �ł������ꏊ���ŋ�ʂ������ꍇ�Ɏg����D */
#endif
} EL_Event;


/**
 * @struct EL_EventStatistics
 * @brief  �C�x���g���v���
 */
typedef struct
{
  uint32_t record_counter_total;                //!< �C�x���g�L�^��
  uint32_t record_counters[EL_ERROR_LEVEL_MAX]; //!< �G���[���x�����Ƃ̋L�^��
} EL_EventStatistics;


#ifdef EL_IS_ENABLE_TLOG
/**
 * @struct EL_TimeSeriesLog
 * @brief  ���n�񃍃O (TLog) �̃��O�\����`
 */
typedef struct
{
  EL_Event* events;               //!< �L�^����C�x���g�e�[�u���i�T�C�Y�ς̂��߃|�C���^�j
  uint16_t  log_capacity;         //!< �C�x���g�e�[�u���̃L���p�V�e�B�i�T�C�Y�j
  uint16_t  log_wp;               //!< �C�x���g�e�[�u���� write pointer�D���񏑂����ݏꏊ
  uint8_t   is_enable_overwrite;  //!< TLog�̏㏑���������邩�H�i�f�t�H���g�ł͋��j
  uint8_t   is_table_overflow;    //!< �㏑���������̏ꍇ�C���ĒǋL�ł��Ȃ��������H
} EL_TimeSeriesLog;
#endif


#ifdef EL_IS_ENABLE_CLOG
/**
 * @struct EL_CLogElement
 * @brief  �ݐσ��O (CLog) �̂P�v�f
 * note    group, local �������C�x���g�𓯈�Ƃ݂Ȃ�
 */
typedef struct
{
  EL_Event          event;                  //!< �L�^����C�x���g�ƁC�Ō�ɋL�^���ꂽ�C�x���g���
  el_clog_counter_t count;                  //!< �ݐσJ�E���^
  ObcTime           delta_record_time;      //!< ���߂Q�̃C�x���g���������̍�
} EL_CLogElement;


/**
 * @struct EL_CumulativeLog
 * @brief  �ݐσ��O (CLog) �̃��O�\����`
 */
typedef struct
{
  EL_CLogElement* logs;         //!< �ݐσJ�E���^��ێ�����C�x���g�e�[�u���D[log_order[0]] ���ŐV�D�i�T�C�Y�ς̂��߃|�C���^�j
  uint16_t*       log_orders;   //!< logs �����n�񏇂ɕ��ёւ��� sort_key�i�T�C�Y�ς̂��߃|�C���^�j
  uint16_t        log_capacity; //!< �C�x���g�e�[�u���̃L���p�V�e�B�i�T�C�Y�j
} EL_CumulativeLog;
#endif


typedef struct
{
#ifdef EL_IS_ENABLE_TLOG
  struct
  {
    uint8_t page_no;            //!< tlm �ŉ��낷���O�y�[�W
    EL_ERROR_LEVEL err_level;   //!< tlm �ŉ��낷���̃G���[���x��
  } tlog;
#endif
#ifdef EL_IS_ENABLE_CLOG
  struct
  {
    uint8_t page_no;            //!< tlm �ŉ��낷���O�y�[�W
    EL_ERROR_LEVEL err_level;   //!< tlm �ŉ��낷���̃G���[���x��
  } clog;
#endif
} EL_TlmInfo;


typedef struct
{
#ifdef EL_IS_ENABLE_TLOG
  EL_TimeSeriesLog tlogs[EL_ERROR_LEVEL_MAX];
#endif
#ifdef EL_IS_ENABLE_CLOG
  EL_CumulativeLog clogs[EL_ERROR_LEVEL_MAX];
#endif

  EL_EventStatistics statistics;    //!< ���v���
  EL_TlmInfo tlm_info;              //!< tlm�̂��߂̏��

  uint8_t is_logging_enable[EL_GROUP_MAX / 8 + 1];  //!< �L�����t���O�D�������팸�̂��߁C bit �P�ʂŕۑ�

  EL_Event latest_event;
} EventLogger;


const extern EventLogger* const event_logger;


/**
 * @brief  event_logger �̏�����
 * @param  void
 * @return void
 */
void EL_initialize(void);

/**
 * @brief  �C�x���g (EL_Event) ���L�^
 * @note   C2A ��̔C�ӂ̏ꏊ���甭�s�\
 * @param  group: EL_Event.group
 * @param  local: EL_Event.local
 * @param  err_level: EL_Event.err_level EL_ERROR_LEVEL_EL �͋֎~
 * @param  note: EL_Event.note
 * @return EL_ACK
 */
EL_ACK EL_record_event(EL_GROUP group, uint32_t local, EL_ERROR_LEVEL err_level, uint32_t note);

/**
 * @brief  event_logger �� EL_Event.group �̗L����
 * @param  group: �L�������� EL_Event.group
 * @retval EL_ACK_OK: ����I��
 * @retval EL_ACK_ILLEGAL_GROUP: �s���� EL_Event.group
 */
EL_ACK EL_enable_logging(EL_GROUP group);

/**
 * @brief  event_logger �� EL_Event.group �̖�����
 * @param  group: ���������� EL_Event.group
 * @retval EL_ACK_OK: ����I��
 * @retval EL_ACK_ILLEGAL_GROUP: �s���� EL_Event.group
 */
EL_ACK EL_disable_logging(EL_GROUP group);

/**
 * @brief  event_logger �� EL_Event.group ���L�����H
 * @param  group: ���������� EL_Event.group
 * @retval 0: ���� or �����Ȉ���
 * @retval 1: �L��
 */
int EL_is_logging_enable(EL_GROUP group);

/**
 * @brief  event_logger �̑S EL_Event.group �̗L����
 * @param  void
 * @return void
 */
void EL_enable_all_logging(void);

/**
 * @brief  event_logger �̑S EL_Event.group �̖�����
 * @param  void
 * @return void
 */
void EL_disable_all_logging(void);

#ifdef EL_IS_ENABLE_TLOG
/**
 * @brief  TLog �̃C�x���g�e�[�u���̏㏑����L����
 * @param  err_level: �L�������� EL_Event.err_level
 * @retval EL_ACK_OK: ����I��
 * @retval EL_ACK_ILLEGAL_ERROR_LEVEL: �s���� EL_Event.err_level
 */
EL_ACK EL_enable_tlog_overwrite(EL_ERROR_LEVEL err_level);

/**
 * @brief  TLog �̃C�x���g�e�[�u���̏㏑���𖳌���
 * @param  err_level: ���������� EL_Event.err_level
 * @retval EL_ACK_OK: ����I��
 * @retval EL_ACK_ILLEGAL_ERROR_LEVEL: �s���� EL_Event.err_level
 */
EL_ACK EL_disable_tlog_overwrite(EL_ERROR_LEVEL err_level);

/**
 * @brief  TLog �̃C�x���g�e�[�u���̏㏑�������ׂėL����
 * @param  void
 * @return void
 */
void EL_enable_tlog_overwrite_all(void);

/**
 * @brief  TLog �̃C�x���g�e�[�u���̏㏑�������ׂĖ�����
 * @param  void
 * @return void
 */
void EL_disable_tlog_overwrite_all(void);

/**
 * @brief  TLog �̃C�x���g�e�[�u���̏㏑�����L�����H
 * @param  err_level: ���������� EL_Event.err_level
 * @retval 0: ���� or �����Ȉ���
 * @retval 1: �L��
 */
int EL_is_tlog_overwrite_enable(EL_ERROR_LEVEL err_level);

/**
 * @brief  TLog �̃C�x���g�e�[�u���̍ŐV����n�Ԗڂ̃C�x���g���擾
 * @note   n ���傫������ꍇ�́C�ŐV�̂��̂�Ԃ�
 * @note   err_level ���s���ȏꍇ�́C EL_ERROR_LEVEL_HIGH �̂��̂�Ԃ�
 * @note   �܂�C�A�T�[�V�����G���[���o���Ȃ�
 * @param  err_level: �擾���� EL_Event.err_level
 * @param  n: n�Ԗځi0�̂Ƃ��ŐV�j
 * @return EL_Event
 */
const EL_Event* EL_get_the_nth_tlog_from_the_latest(EL_ERROR_LEVEL err_level, uint16_t n);
#endif

CCP_EXEC_STS Cmd_EL_INIT(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_CLEAR_LOG_ALL(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_CLEAR_LOG_BY_ERR_LEVEL(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_CLEAR_STATISTICS(const CTCP* packet);

#ifdef EL_IS_ENABLE_TLOG
CCP_EXEC_STS Cmd_EL_CLEAR_TLOG(const CTCP* packet);
#endif

#ifdef EL_IS_ENABLE_CLOG
CCP_EXEC_STS Cmd_EL_CLEAR_CLOG(const CTCP* packet);
#endif

CCP_EXEC_STS Cmd_EL_RECORD_EVENT(const CTCP* packet);

#ifdef EL_IS_ENABLE_TLOG
CCP_EXEC_STS Cmd_EL_TLOG_SET_PAGE_FOR_TLM(const CTCP* packet);
#endif

#ifdef EL_IS_ENABLE_CLOG
CCP_EXEC_STS Cmd_EL_CLOG_SET_PAGE_FOR_TLM(const CTCP* packet);
#endif

CCP_EXEC_STS Cmd_EL_INIT_LOGGING_SETTINGS(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_ENABLE_LOGGING(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_DISABLE_LOGGING(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_ENABLE_LOGGING_ALL(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_DISABLE_LOGGING_ALL(const CTCP* packet);

#ifdef EL_IS_ENABLE_TLOG
CCP_EXEC_STS Cmd_EL_ENABLE_TLOG_OVERWRITE(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_DISABLE_TLOG_OVERWRITE(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_ENABLE_TLOG_OVERWRITE_ALL(const CTCP* packet);

CCP_EXEC_STS Cmd_EL_DISABLE_TLOG_OVERWRITE_ALL(const CTCP* packet);
#endif

#endif
