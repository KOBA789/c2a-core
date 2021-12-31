/**
 * @file
 * @brief Ground Station �Ƃ̒ʐM�p�� Driver
 */
#ifndef GS_H_
#define GS_H_

#include <src_core/CmdTlm/packet_handler.h>
#include <src_core/Drivers/Super/driver_super.h>
#include <src_core/IfWrapper/uart.h>
#include <src_core/IfWrapper/ccsds.h>
#include "../../IfWrapper/ccsds_user.h"
#include "../../CmdTlm/Ccsds/vcdu.h"
#include "gs_validate.h"

/**
 * @enum GS_TCF_TYPE_ENUM
 * @note rx_header_ <-> stream �� �e CMD �Ƃ̑Ή��̐U�蕪�����s���Ă���. ���� Index �ƑΉ������邽��.
 * @note uint8_t ��z��
 */
typedef enum
{
  GS_TCF_TYPE_ENUM_AD_CMD,
  GS_TCF_TYPE_ENUM_BC_CMD,
  GS_TCF_TYPE_ENUM_BD_CMD,
  GS_TCF_TYPE_ENUM_UNKNOWN
} GS_TCF_TYPE_ENUM;

/**
 * @enum  GS_PORT_TYPE
 * @brief GS �̒ʐM�|�[�g�� CCSDS �� UART �� 2 ����
 * @note  uint8_t ��z��
 */
typedef enum
{
  GS_PORT_TYPE_CCSDS, //!< CCSDS
  GS_PORT_TYPE_UART,  //!< UART
  GS_PORT_TYPE_NUM    //!< port ��
} GS_PORT_TYPE;

/**
 * @struct GS_Info
 * @brief  �h���C�o�[�̊e��Ԃ̋L�^
 */
typedef struct
{
  DS_ERR_CODE rec_status;                   //!< DriverSuper ����̎�M����
  GS_TCF_TYPE_ENUM last_rec_tcf_type;       //!< �Ō�Ɏ�M���� tcf �̃^�C�v
  DS_ERR_CODE ad_rec_status;                //!< AD CMD ����M�����Ƃ��� analyze ����
  DS_ERR_CODE bc_rec_status;                //!< BC CMD ����M�����Ƃ��� analyze ����
  DS_ERR_CODE bd_rec_status;                //!< BD CMD ����M�����Ƃ��� analyze ����
  GS_VALIDATE_ERR tc_frame_validate_status; //!< TC Frame �̌��،���
  int ret_from_if_rx;                       //!< UART or CCSDS ����̕Ԃ�l
  cycle_t last_rec_time;                    //!< �Ō�Ɏ�M��������
  PH_ACK cmd_ack;                           //!< ��M���� CMD �̎��s����

  cycle_t send_cycle;                       //!< �Ō�ɑ��M�����Ƃ��̎���
  VCDU_VCID vcid;                           //!< ���M���� TLM �̃^�C�v
  uint32_t vcdu_counter;                    //!< VCDU counter
} GS_Info;

/**
 * @struct GS_Driver
 * @brief  GroundStation Driver
 */
typedef struct
{
  // CCSDS ���� Driver
  struct
  {
    DriverSuper  super;
    CCSDS_Config ccsds_config;
  } driver_ccsds;

  // UART ���� Driver
  struct
  {
    DriverSuper super;
    UART_Config uart_config;
  } driver_uart;

  GS_Info info[GS_PORT_TYPE_NUM]; //!< CCSDS �p�� UART �p 2 ��
  const GS_Info* latest_info;
  CCSDS_Info ccsds_info;
  GS_PORT_TYPE tlm_tx_port_type;
  uint8_t is_ccsds_tx_valid;      //!< CCSDS TX ���L���ɂȂ��Ă��邩 (�n�㎎���œˑR�d�g���o���Ɗ댯�Ȃ̂�)
} GS_Driver;

/**
 * @brief Driver �̏�����
 * @param[in] uart_ch: �L���ʐM���� CH
 * @return int: 0
 */
int GS_init(GS_Driver* gs_driver, uint8_t uart_ch);

/**
 * @brief �n�ォ�� CMD ����M����. �`���� TCFrame
 * @param[in] gs_driver: �h���C�o�[
 * @return int: 0
 */
int GS_rec_tcf(GS_Driver* gs_driver);

/**
 * @brief �n��Ɍ����� TLM �𑗐M. �`���� VCDU
 * @note  DS_send_general_cmd ���g���Ă��邪, ����� DS �� MOBC �R���|�Ԃ�z�肵�Ă��邽��, MOBC ���猩��ƃR���|�� cmd �𑗐M���Ă���l�Ɍ�����, �� ����� MOBC ����n��� TLM �𑗐M���Ă���
 * @note TLM ���M, �`���� VCDU
 * @param[in] gs_driver: �h���C�o�[
 * @param[in] vcdu: ���M���� VCDU. �ꍇ�ɂ���Ă͂��̂܂� DS �ɓn���̂ŁC local�ϐ��ł͂Ȃ�static�ȕϐ���n������
 * @return DS_CMD_ERR_CODE: ���M����
 */
DS_CMD_ERR_CODE GS_send_vcdu(GS_Driver* gs_driver, const VCDU* vcdu);

#endif
