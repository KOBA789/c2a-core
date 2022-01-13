/**
 * @file
 * @brief  �e�����g���̓o�^�C�e�����g���֘A����
 */
#ifndef TELEMETRY_FRAME_H_
#define TELEMETRY_FRAME_H_

#include "./common_tlm_cmd_packet.h"

#define TF_TLM_PAGE_SIZE (64)                                    //!< �e�����e�[�u����1�e�����g���p�P�b�g(=1�y�[�W)�Ɋi�[�����e������
#define TF_TLM_PAGE_MAX  (4)                                     //!< �e�����e�[�u���y�[�W���i�y�[�W�l�[�V�����p�j
#define TF_MAX_TLMS      (TF_TLM_PAGE_SIZE * TF_TLM_PAGE_MAX)    //!< �e�����e�[�u���T�C�Y�D���Ȃ킿�o�^�ł���ő�e������

#include <src_user/Settings/TlmCmd/telemetry_frame_params.h>

/**
 * @struct TF_TlmInfo
 * @brief  tlm �̏��
 * @note   tlm_func �́C�������̓e�������C���s���� TF_ACK ��Ԃ�
 */
typedef struct
{
  int (*tlm_func)(unsigned char*, int);
} TF_TlmInfo;

/**
 * @enum   TF_ACK
 * @brief  tlm_func �Ȃǂ̕Ԃ�l�ɂ���
 * @note   int8_t ��z��i�����̓e�������Ŏg���̂ŁC�����j
 */
typedef enum
{
  TF_SUCCESS = 0,
  TF_TOO_SHORT_LEN = -1,
  TF_NOT_DEFINED = -2,
  TF_UNKNOWN = -3
} TF_ACK;

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


/**
 * @brief  TF �̏�����
 * @param  void
 * @return void
 */
void TF_initialize(void);

/**
 * @brief  �e���������ۂɐ�������֐�
 * @param  packet_id: Tlm ID
 * @param  contents:  �e��������� body �ւ̃|�C���^
 * @param  max_len:   �e������ body �Ƃ��Ďg����ő咷
 * @return �������̓e�������C���s���� TF_ACK
 */
int TF_generate_contents(int packet_id,
                         unsigned char* contents,
                         int max_len);

/**
 * @brief  Tlm Table�̃��[�h
 * @note   ��`�� /src_user/TlmCmd/TelemetryDefinitions.c �ɂ���
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
