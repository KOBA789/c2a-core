/**
 * @file telemetry_frame.h
 * @brief  �e�����g���̓o�^�C�e�����g���֘A����
 */
#ifndef TELEMETRY_FRAME_H_
#define TELEMETRY_FRAME_H_

#include "./common_tlm_cmd_packet.h"

#define TF_TLM_PAGE_SIZE (64)                                    //!< �e�����e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[�����e������
#define TF_TLM_PAGE_MAX  (4)                                     //!< �e�����e�[�u���y�[�W���i�y�[�W�l�[�V�����p�j
#define TF_MAX_TLMS      (TF_TLM_PAGE_SIZE * TF_TLM_PAGE_MAX)    //!< �e�����e�[�u���T�C�Y�D���Ȃ킿�o�^�ł���ő�e������

#include <src_user/Settings/CmdTlm/telemetry_frame_params.h>

typedef struct
{
  int (*tlm_func)(unsigned char*, int);
} TF_TlmInfo;

typedef enum
{
  TLM_SUCCESS = 0,
  TLM_TOO_SHORT_LEN = -1,
  TLM_NOT_DEFINED = -2,
  TLM_UNKNOWN = -3
} TLM_ACK;

/**
 * @struct TelemetryFrame
 * @brief  TelemetryFrame �� Info �\����
 */
typedef struct
{
  TF_TlmInfo tlm_table[TF_MAX_TLMS];  //!< �e�����g���e�[�u��
  uint8_t tlm_page_no;                //!< �e�����Ŏg���y�[�W��
} TelemetryFrame;

extern const TelemetryFrame* const telemetry_frame;


void TF_initialize(void);

int TF_generate_contents(int packet_id,
                         unsigned char* contents,
                         int max_len);

/**
 * @brief  Tlm Table�̃��[�h
 * @note   ��`�� /src_user/CmdTlm/TelemetryDefinitions.c �ɂ���
 * @param  tlm_table: Tlm Table�̎���
 * @return void
 */
void TF_load_tlm_table(TF_TlmInfo tlm_table[TF_MAX_TLMS]);

CCP_EXEC_STS Cmd_TF_REGISTER_TLM(const CTCP* packet);

CCP_EXEC_STS Cmd_TF_SET_PAGE_FOR_TLM(const CTCP* packet);

void TF_copy_u8(uint8_t* ptr,
                uint8_t data);

void TF_copy_u16(uint8_t* ptr,
                 uint16_t data);

void TF_copy_u32(uint8_t* ptr,
                 uint32_t data);

void TF_copy_i8(uint8_t* ptr,
                int8_t data);

void TF_copy_i16(uint8_t* ptr,
                 int16_t data);

void TF_copy_i32(uint8_t* ptr,
                 int32_t data);

void TF_copy_float(uint8_t* ptr,
                   float data);

void TF_copy_double(uint8_t* ptr,
                    double data);

#endif
