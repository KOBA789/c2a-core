/**
 * @file  gs_validate.h
 * @brief ��M���� TC Frame �̌��؂��s��
 */
#ifndef GS_VALIDATE_H_
#define GS_VALIDATE_H_

#include "../../CmdTlm/Ccsds/TCFrame.h"
#include "../../Library/stdint.h"

/**
 * @enum GS_VALIDATE_ERR
 * @brief gs_validate �֐��̕Ԃ�l
 */
typedef enum
{
  GS_VALIDATE_ERR_OK = 0,

  GS_VALIDATE_ERR_TCF_VER,
  GS_VALIDATE_ERR_TCF_SCID,
  GS_VALIDATE_ERR_TCF_VCID,
  GS_VALIDATE_ERR_TCF_TYPE,
  GS_VALIDATE_ERR_IN_LOCKOUT,
  GS_VALIDATE_ERR_TCS_SEQ_FLAG,
  GS_VALIDATE_ERR_TCP_VER,
  GS_VALIDATE_ERR_TCP_2ND_HDR_FLAG,
  GS_VALIDATE_ERR_TCP_APID,
  GS_VALIDATE_ERR_TCP_TYPE_IS_NOT_CMD,
  GS_VALIDATE_ERR_TCP_SEQ_IS_NOT_SINGLE,
  GS_VALIDATE_ERR_FECW_MISSMATCH,
  GS_VALIDATE_ERR_FARM1_POSITIVE_WINDOW_AREA,
  GS_VALIDATE_ERR_FARM1_NEGATIVE_WINDOW_AREA,
  GS_VALIDATE_ERR_FARM1_LOCKOUT_AREA,
  GS_VALIDATE_ERR_FARM1_SAME_NUMBER,
  GS_VALIDATE_ERR_INVALID_BC_CMD
} GS_VALIDATE_ERR;

/**
 * @struct GS_ValidateInfo
 * @brief  ���߂Ɏ�M�����p�P�b�g�̌��،��ʂ��i�[
 */
typedef struct
{
  // FARM-A,B�p�̃J�E���^�l
  uint8_t type_a_counter;
  uint8_t type_b_counter;
  // FARM-1�p�̏��
  uint8_t lockout_flag;
  uint8_t retransmit_flag;
  uint8_t positive_window_width;
} GS_ValiateInfo;

extern const GS_ValiateInfo* const gs_validate_info;

/**
 * @brief GS_ValiateInfo �̏�����
 */
void GS_validate_init(void);

/**
 * @brief tc_frame �̌���
 * @param[in] tc_frame: ���؂��� tc frame
 * @return GS_VALIDATE_ERR
 */
GS_VALIDATE_ERR GS_validate_tc_frame(const TCF* tc_frame);

/**
 * @brief �e�����p�� clcw �̍쐬
 * @return clcw
 */
uint32_t GS_form_clcw(void);

/**
 * @brief positive_window_width �̐ݒ�
 * @param[in] positive_window_width
 * @return GS_VALIDATE_ERR
 */
GS_VALIDATE_ERR GS_set_farm_pw(uint8_t positive_window_width);

#endif
