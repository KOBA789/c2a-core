#ifndef ANOMALY_LOGGER_H_
#define ANOMALY_LOGGER_H_

#include <stddef.h> // for size_t

#include "../TimeManager/obc_time.h"
#include "../../CmdTlm/common_tlm_cmd_packet.h"
#include <src_user/Settings/AnomalyLogger/anomaly_group.h>

#define AL_TLM_PAGE_SIZE (32)                                 //!< �A�m�}�����K�[�̃��O�e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[����郍�O���i�y�[�W�l�[�V�����p�j
#define AL_TLM_PAGE_MAX  (4)                                  //!< �A�m�}�����K�[�̃��O�e�[�u���̃y�[�W���i�y�[�W�l�[�V�����p�j
#define AL_RECORD_MAX (AL_TLM_PAGE_SIZE * AL_TLM_PAGE_MAX)    //!< �ő剽�̃A�m�}�����L�^�ł��邩
// [TODO] 2018/12/10
// AL_FULL �̂܂܂ł����̂��H
// FLASH�ɏ����Ă����āC�����Ă��H
// ����I�ɑS�����낵�āCclear����̂����肩������Ȃ�
// �A�m�}�����R�[�_�[�̕��́Crunlength�ň��k�����ɁCti�ƂƂ��ɕۑ�������

// anomaly_logger_params.h �ɂāC�ȉ��̒萔�̍Ē�`������
//   - AL_TLM_PAGE_SIZE
//   - AL_TLM_PAGE_MAX
//   - AL_RECORD_MAX
// �܂�
//   AL_DISALBE_AT_C2A_CORE
// ���`���邱�ƂŁC C2A CORE �ł� AL_add_anomaly ��}���ł���D
// AL ���� Event Logger �ւ̈ڍs���ς�ł�����̂ɑ΂��ẮC��`�𐄏�����D
// �Ȃ��C������`���Ă��C AL, AH ���̂̋@�\�͈����������p�\�ł���D
#include <src_user/Settings/System/anomaly_logger_params.h>

typedef enum
{
  AL_CORE_GROUP_ANOMALY_LOGGER,
  AL_CORE_GROUP_MODE_MANAGER,
  AL_CORE_GROUP_TASK_MANAGER,
  AL_CORE_GROUP_TASK_DISPATCHER,
  AL_CORE_GROUP_APP_MANAGER,
  AL_CORE_GROUP_TLCD,
  AL_CORE_GROUP_MAX
} AL_CORE_GROUP;

typedef enum
{
  AL_SUCCESS,
  AL_NEARLY_FULL,
  AL_FULL
} AL_ACK;

// AL_add_anomaly�̕Ԃ�l
enum
{
  AL_ADD_SUCCESS,         // ����I��
  AL_ADD_ERR_INVALID,     // �G���[
  AL_ADD_DISABLE_LOGGING  // ���M���O�������i���p�j
};

typedef struct
{
  uint32_t group;
  uint32_t local;
} AL_AnomalyCode;

typedef struct
{
  ObcTime time;
  AL_AnomalyCode code;
  size_t run_length;
} AL_AnomalyRecord;

typedef struct
{
  size_t counter;           // AL_add_anomaly ���ꂽ��
  size_t header;            // ����add�����Ə������܂��ʒu�DAR�̓����O�o�b�t�@�ł͂Ȃ��CFull�łƂ܂�D
  AL_AnomalyRecord records[AL_RECORD_MAX];
  uint8_t page_no;
  uint8_t is_logging_enable[AL_GROUP_MAX / 8];    // bit�P�ʂŏ���ێ�����
  uint16_t threshold_of_nearly_full;
} AnomalyLogger;

extern const AnomalyLogger* const anomaly_logger;

const AL_AnomalyRecord* AL_get_record(size_t pos);

const AL_AnomalyRecord* AL_get_latest_record(void);

void AL_initialize(void);

int AL_add_anomaly(uint32_t group, uint32_t local);

int AL_enable_logging(uint32_t group);

int AL_disable_logging(uint32_t group);

int  AL_is_logging_enable(uint32_t group);

void AL_clear(void);

CCP_EXEC_STS Cmd_AL_ADD_ANOMALY(const CTCP* packet);

CCP_EXEC_STS Cmd_AL_CLEAR_LIST(const CTCP* packet);

CCP_EXEC_STS Cmd_AL_SET_PAGE_FOR_TLM(const CTCP* packet);

CCP_EXEC_STS Cmd_AL_INIT_LOGGING_ENA_FLAG(const CTCP* packet);

CCP_EXEC_STS Cmd_AL_ENABLE_LOGGING(const CTCP* packet);

CCP_EXEC_STS Cmd_AL_DISABLE_LOGGING(const CTCP* packet);

CCP_EXEC_STS Cmd_AL_SET_THRES_OF_NEARLY_FULL(const CTCP* packet);

#endif
