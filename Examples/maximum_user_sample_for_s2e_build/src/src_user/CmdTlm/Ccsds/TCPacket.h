#ifndef TC_PACKET_H_
#define TC_PACKET_H_

#include <stddef.h> // for size_t

#include "../../Library/stdint.h"

#define TCP_MAX_LEN          (432u)
#define TCP_PRM_HDR_LEN        (6u)
#define TCP_TLM_2ND_HDR_LEN    (7u)
#define TCP_CMD_2ND_HDR_LEN    (1u)
#define TCP_CMD_USER_HDR_LEN   (8u)


/**
 * @struct TCP
 * @brief  TCPacket or Space Packet
 */
typedef struct
{
  uint8_t packet[TCP_MAX_LEN];
} TCP;

/**
 * @enum   TCP_VER
 * @brief  TCP version
 * @note   3bit
 */
typedef enum
{
  TCP_VER_1 = 0, // 000b: Version-1
  TCP_VER_UNKNOWN
} TCP_VER;

/**
 * @enum   TCP_TYPE
 * @brief  tlm or cmd ���K��
 * @note   0/1��1bit
 */
typedef enum
{
  TCP_TYPE_TLM = 0, // 0b: TELEMETRY
  TCP_TYPE_CMD = 1  // 1b: COMMAND
} TCP_TYPE;

/**
 * @enum   TCP_2ND_HDR_FLAG
 * @brief  TCP �� Secondary Header Flag �̗L��
 * @note   0/1��1bit
 */
typedef enum
{
  TCP_2ND_HDR_ABSENT  = 0, // 0b: Secondary Header Absent
  TCP_2ND_HDR_PRESENT = 1  // 1b: Secondary Header Present
} TCP_2ND_HDR_FLAG;

/**
 * @enum   TCP_APID
 * @brief  Application Process ID
 * @note   GSTOS �̒~�σf�[�^�̊g���q�ɂ�����
 * @note   11bit
 * @note   CTCP_DEST_ID �Ƃ��� typedef ����
 */
typedef enum
{
  TCP_APID_MOBC_CMD = 0x210,         // 01000010000b:
  TCP_APID_AOBC_CMD = 0x211,         // 01000010001b:
  TCP_APID_TOBC_CMD = 0x212,         // 01000010010b:
  TCP_APID_TCAL_TLM = 0x410,         // 10000010000b: APID for TIME CARIBLATION TLM
  TCP_APID_MIS_TLM  = 0x510,         // 10100010000b: APID for MIS TLM
  TCP_APID_DUMP_TLM = 0x710,         // 11100010000b: APID for DUMP TLM
  TCP_APID_FILL_PKT = 0x7ff,         // 11111111111b: APID for FILL PACKET
  TCP_APID_UNKNOWN
} TCP_APID;

/**
 * @enum   TCP_SEQ_FLAG
 * @brief  Sequence Flag
 * @note   2bit
 * @note   Packet Sequence Flag for each ADU �������p����D
 *         ���̏ꍇ�C component �� segment �Ɠǂݑւ���D
 */
typedef enum
{
  TCP_SEQ_CONT   = 0, // 00b: Continuation component of higher data structure.
  TCP_SEQ_FIRST  = 1, // 01b: First component of higher data structure.
  TCP_SEQ_LAST   = 2, // 10b: Last component of higher data structure.
  TCP_SEQ_SINGLE = 3  // 11b: Standalone packet.
} TCP_SEQ_FLAG;

/**
 * @enum   TCP_CMD_FMT_ID
 * @brief  Format ID
 * @note   8bit
 */
typedef enum
{
  TCP_CMD_FMT_ID_CONTROL = 0x01, // 01h: Control Command Packet
  TCP_CMD_FMT_ID_USER    = 0x02, // 02h: User Data Command Packet
  TCP_CMD_FMT_ID_MEMORY  = 0x03, // 03h: Memory Write Command Packet
  TCP_CMD_FMT_ID_UNKNOWN
} TCP_CMD_FMT_ID;

/**
 * @enum   TCP_CM
 * @brief  Command Type
 * @note   8bit
 */
typedef enum
{
  TCP_CMD_TYPE_DC = 0x01, // 01h: Discrete Command
  TCP_CMD_TYPE_SM = 0x02, // 02h: Serial Magnitude Command
  TCP_CMD_TYPE_UNKNOWN
} TCP_CMD_TYPE;

/**
 * @enum   TCP_CMD_EXEC_TYPE
 * @brief  �R�}���h���s���
 * @note   0x0*��z��i���4bit�͑���C2A�𓋍ڂ����{�[�h�p�ɓ]�����邽�߂Ɏg�����߁j
 *         ���̒�`�� TCP_DEST_TYPE �ɂ���
 *         pure TCP�͂�����8bit�ł��邱�Ƃɒ���
 * @note   TCP_CMD_EXEC_TYPE �� CCP_EXEC_TYPE �̒�`�͈�v (���m�ɂ́A�O�҂͌�҂Ɋ܂܂�Ă���)
 *         CCP_EXEC_TYPE CCP_get_exec_type(const CTCP* packet) ���Q��
 * @note   GS�����OBC.TL��CC2A����CCP_form_tlc�͂��ׂ�TL0�����ɂȂ�
 */
typedef enum
{
  TCP_CMD_EXEC_TYPE_GS = 0x00, // 00h: Ground Station Command
  TCP_CMD_EXEC_TYPE_TL = 0x01, // 01h: Timeline Command
  TCP_CMD_EXEC_TYPE_MC = 0x02, // 02h: Macro Command
  TCP_CMD_EXEC_TYPE_RT = 0x03, // 03h: Realtime Command
  TCP_CMD_EXEC_TYPE_UNKNOWN
} TCP_CMD_EXEC_TYPE;

/**
 * @enum   TCP_CMD_DEST_TYPE
 * @brief  �R�}���h�̉��߂̈�����K��
 * @note   TO_ME: �������g �� �������g��TLC��BC�Ƃ��ĉ���
 * @note   TO_*:  �]�����TL��BC�Ƃ��ĉ��߁iGS���痈���R�}���h�����g�̃L���[�ɂ���Ȃ��j
 * @note   0x*0��z��
 * @note   ����4bit�� TCP_CMD_EXEC_TYPE �� CCP_EXEC_TYPE ��
 */
typedef enum
{
  TCP_CMD_DEST_TYPE_TO_ME     = 0x00,
  TCP_CMD_DEST_TYPE_TO_MOBC   = 0x10,
  TCP_CMD_DEST_TYPE_TO_AOBC   = 0x20,
  TCP_CMD_DEST_TYPE_TO_TOBC   = 0x30,
  TCP_CMD_DEST_TYPE_TO_UNKOWN = 0x40
} TCP_CMD_DEST_TYPE;


// *******************
//  TCP getter/setter
// *******************

/**
 * @brief  TCP ver ���擾
 * @param  tcp: TCP
 * @return TCP_VER
 */
TCP_VER TCP_get_ver(const TCP* tcp);

/**
 * @brief  TCP ver ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     ver: TCP_VER
 * @return void
 */
void TCP_set_ver(TCP* tcp, TCP_VER ver);

/**
 * @brief  TCP type ���擾
 * @param  tcp: TCP
 * @return TCP_TYPE
 */
TCP_TYPE TCP_get_type(const TCP* tcp);

/**
 * @brief  TCP type ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     type: TCP_TYPE
 * @return void
 */
void TCP_set_type(TCP* tcp, TCP_TYPE type);

/**
 * @brief  TCP Secondary Header Flag ���擾
 * @param  tcp: TCP
 * @return TCP_2ND_HDR_FLAG
 */
TCP_2ND_HDR_FLAG TCP_get_2nd_hdr_flag(const TCP* tcp);

/**
 * @brief  TCP Secondary Header Flag ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     flag: TCP_2ND_HDR_FLAG
 * @return void
 */
void TCP_set_2nd_hdr_flag(TCP* tcp, TCP_2ND_HDR_FLAG flag);

/**
 * @brief  APID ���擾
 * @param  tcp: TCP
 * @return TCP_APID
 */
TCP_APID TCP_get_apid(const TCP* tcp);

/**
 * @brief  APID ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     apid: APID
 * @return void
 */
void TCP_set_apid(TCP* tcp, TCP_APID apid);

/**
 * @brief  Sequence Flag ���擾
 * @param  tcp: TCP
 * @return TCP_SEQ_FLAG
 */
TCP_SEQ_FLAG TCP_get_seq_flag(const TCP* tcp);

/**
 * @brief  Sequence Flag ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     flag: TCP_SEQ_FLAG
 * @return void
 */
void TCP_set_seq_flag(TCP* tcp, TCP_SEQ_FLAG flag);

/**
 * @brief  Sequence Count ���擾
 * @param  tcp: TCP
 * @return Sequence Count
 */
uint16_t TCP_get_seq_cnt(const TCP* tcp);

/**
 * @brief  Sequence Count ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     cnt: Sequence Count
 * @return void
 */
void TCP_set_seq_cnt(TCP* tcp, uint16_t cnt);

/**
 * @brief  Packet Length ���擾
 * @param  tcp: TCP
 * @return Packet Length
 */
uint16_t TCP_get_packet_len(const TCP* tcp);

/**
 * @brief  Packet Length ��ݒ�
 * @note   Packet Length �Ƃ� space packet ��PACKET DATA FIELD ���ł���C PRIMARY HEADER ���܂܂Ȃ�
 *         ( https://public.ccsds.org/Pubs/133x0b2e1.pdf )
 * @param[in,out] tcp: TCP
 * @param[in]     len: Packet Length
 * @return void
 */
void TCP_set_packet_len(TCP* tcp, uint16_t len);

/**
 * @brief  TCP ���R�s�[
 * @note   �������̂��߂����ׂĂł͂Ȃ��K�v�Œ�������R�s�[���Ȃ��ꍇ
 * @param[in,out] dest: �R�s�[�� TCP
 * @param[in]     src:  �R�s�[�� TCP
 * @return void
 */
void TCP_copy_packet(TCP* dest, const TCP* src);

// ***********************
//  TCP CMD getter/setter
// ***********************

/**
 * @brief  Format ID ���擾
 * @param  tcp: TCP
 * @return TCP_CMD_FMT_ID
 */
TCP_CMD_FMT_ID TCP_CMD_get_fmt_id(const TCP* tcp);

/**
 * @brief  Format ID ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     id:  TCP_CMD_FMT_ID
 * @return void
 */
void TCP_CMD_set_fmt_id(TCP* tcp, TCP_CMD_FMT_ID id);

/**
 * @brief  Command Type ���擾
 * @param  tcp: TCP
 * @return TCP_CMD_TYPE
 */
TCP_CMD_TYPE TCP_CMD_get_type(const TCP* tcp);

/**
 * @brief  Command Type ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     type: TCP_CMD_TYPE
 * @return void
 */
void TCP_CMD_set_type(TCP* tcp, TCP_CMD_TYPE type);

/**
 * @brief  channel ID ���擾
 * @param  tcp: TCP
 * @return channel ID
 */
uint16_t TCP_CMD_get_channel_id(const TCP* tcp);

/**
 * @brief  channel ID ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     id:  channel ID
 * @return void
 */
void TCP_CMD_set_channel_id(TCP* tcp, uint16_t id);

/**
 * @brief  EXEC_TYPE ���擾
 * @param  tcp: TCP
 * @return TCP_CMD_EXEC_TYPE
 */
TCP_CMD_EXEC_TYPE TCP_CMD_get_exec_type(const TCP* tcp);

/**
 * @brief  EXEC_TYPE ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     type: TCP_CMD_EXEC_TYPE
 * @return void
 */
void TCP_CMD_set_exec_type(TCP* tcp, TCP_CMD_EXEC_TYPE type);

/**
 * @brief  CMD_DEST_TYPE ���擾
 * @param  tcp: TCP
 * @return TCP_CMD_DEST_TYPE
 * @note   2021/01/22�� TCP_CMD_EXEC_TYPE �̏��4bit��q�؂���`�Œǉ�����
 */
TCP_CMD_DEST_TYPE TCP_CMD_get_dest_type(const TCP* tcp);

/**
 * @brief  CMD_DEST_TYPE ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     type: TCP_CMD_DEST_TYPE
 * @return void
 * @note   2021/01/22�� TCP_CMD_EXEC_TYPE �̏��4bit��q�؂���`�Œǉ�����
 */
void TCP_CMD_set_dest_type(TCP* tcp, TCP_CMD_DEST_TYPE type);

/**
 * @brief  Time Indicator (TI) ���擾
 * @param  tcp: TCP
 * @return TI
 */
uint32_t TCP_CMD_get_ti(const TCP* tcp);

/**
 * @brief  Time Indicator (TI) ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     ti:  TI
 * @return void
 */
void TCP_CMD_set_ti(TCP* tcp, uint32_t ti);

/**
 * @brief  �擪�̃p�����^�̃|�C���^���擾
 * @param  tcp: TCP
 * @return *param_head
 */
const uint8_t* TCP_CMD_get_param_head(const TCP* tcp);

/**
 * @brief  �p�����^��ݒ�
 * @param[in,out] tcp:   TCP
 * @param[in]     param: �p�����^�z��̃|�C���^
 * @param[in]     len:   �p�����^��
 * @return void
 */
void TCP_CMD_set_param(TCP* tcp, const uint8_t* param, uint16_t len);

/**
 * @brief  ���ʕ����� Header ���\�z
 * @param[in,out] tcp:  TCP
 * @return void
 */
void TCP_CMD_set_common_hdr(TCP* tcp);


// ***********************
//  TCP TLM getter/setter
// ***********************

/**
 * @brief  Time Indicator (TI) ���擾
 * @param  tcp: TCP
 * @return TI
 */
uint32_t TCP_TLM_get_ti(const TCP* tcp);

/**
 * @brief  Time Indicator (TI) ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     ti:  TI
 * @return void
 */
void TCP_TLM_set_ti(TCP* tcp, uint32_t ti);

/**
 * @brief  category ���擾
 * @param  tcp: TCP
 * @return category
 */
uint8_t TCP_TLM_get_category(const TCP* tcp);

/**
 * @brief  category ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     category: category
 * @return void
 */
void TCP_TLM_set_category(TCP* tcp, uint8_t category);

/**
 * @brief  packet id ���擾
 * @param  tcp: TCP
 * @return packet id
 */
uint8_t TCP_TLM_get_packet_id(const TCP* tcp);

/**
 * @brief  packet id ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     id:  packet id
 * @return void
 */
void TCP_TLM_set_packet_id(TCP* tcp, uint8_t id);

/**
 * @brief  Packet Sequence Flag for each ADU ���擾
 * @param  tcp: TCP
 * @return TCP_SEQ_FLAG
 */
TCP_SEQ_FLAG TCP_TLM_get_adu_seq_flag(const TCP* tcp);

/**
 * @brief  Packet Sequence Flag for each ADU ��ݒ�
 * @param[in,out] tcp:  TCP
 * @param[in]     flag: TCP_SEQ_FLAG
 * @return void
 */
void TCP_TLM_set_adu_seq_flag(TCP* tcp, TCP_SEQ_FLAG flag);

/**
 * @brief  ADU Count ���擾
 * @param  tcp: TCP
 * @return ADU Count
 */
uint8_t TCP_TLM_get_adu_cnt(const TCP* tcp);

/**
 * @brief  ADU Count ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     cnt: ADU Count
 * @return void
 */
void TCP_TLM_set_adu_cnt(TCP* tcp, uint8_t cnt);

/**
 * @brief  Packet Sequence Count for each ADU ���擾
 * @param  tcp: TCP
 * @return Packet Sequence Count for each ADU
 */
uint16_t TCP_TLM_get_adu_seq_cnt(const TCP* tcp);

/**
 * @brief  Packet Sequence Count for each ADU ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     cnt: Packet Sequence Count for each ADU
 * @return void
 */
void TCP_TLM_set_adu_seq_cnt(TCP* tcp, uint16_t cnt);

/**
 * @brief  ADU Length ���擾
 * @param  tcp: TCP
 * @return ADU Length
 */
uint32_t TCP_TLM_get_adu_len(const TCP* tcp);

/**
 * @brief  ADU Length ��ݒ�
 * @param[in,out] tcp: TCP
 * @param[in]     len: ADU Length
 * @return void
 * @note   24 bit�Ȃ��߁C�ŏ��8bit�͐؂�̂Ă���
 */
void TCP_TLM_set_adu_len(TCP* tcp, uint32_t len);

/**
 * @brief  TCP�S�̂́i Primary Header ���܂񂾁j Packet�����擾
 * @param[in,out] tcp: TCP
 * @param[in]     len: Packet Length
 * @return void
 */
uint16_t TCP_TLM_get_packet_len(const TCP* packet);

/**
 * @brief  ADU�����Ȃ��̏ꍇ��User Data�擪�̃|�C���^���擾
 * @param  tcp: TCP
 * @return *param_head
 * @note   ���̊֐��Ŏ擾�����|�C���^�o�R��tlm�̃f�[�^���Z�b�g���鎖�����邽�߁Cconst�����Ă��Ȃ�
 */
uint8_t* TCP_TLM_get_user_data_head(TCP* packet);

/**
 * @brief  Primary Header ���\�z
 * @param[in,out] tcp:  TCP
 * @param[in]     apid: TCP_APID
 * @param[in]     len:  Packet Length
 * @return void
 */
void TCP_TLM_setup_primary_hdr(TCP* tcp, TCP_APID apid, uint16_t len);

/**
 * @brief  ���ʕ����� Header ���\�z
 * @param[in,out] tcp:  TCP
 * @return void
 */
void TCP_TLM_set_common_hdr(TCP* tcp);

/**
 * @brief  TCP�S�̂�0x00�N���A���Cfill_size�ɓ������T�C�Y�̃p�P�b�g�𐶐�����
 * @param[in,out] tcp: TCP
 * @param[in]     fill_size: Packet size
 * @return void
 */
void TCP_TLM_setup_fill_packet(TCP* tcp, uint16_t fill_size);


#endif // TC_PACKET_H_
