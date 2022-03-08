/**
 * @file
 * @brief CCSDS で規定される TC Segment の実装
 * @note  packet 構造
 *        |---------+-------+-------+-----------------------------|
 *        | Pos     | Pos   | size  | name                        |
 *        | [octet] | [bit] | [bit] |                             |
 *        |---------+-------+-------+-----------------------------|
 *        | === Primary Header ===================================|
 *        |---------+-------+-------+-----------------------------|
 *        |       0 |     0 |     2 | Sequential Flag             |
 *        |       0 |     2 |     6 | Multiplexer Access Point ID |
 *        |---------+-------+-------+-----------------------------|
 *        | === User Data Field ==================================|
 *        |---------+-------+-------+-----------------------------|
 *        |       1 |     0 |     * | Command Space Packet        |
 *        |---------+-------+-------+-----------------------------|
 */
#ifndef TCSEGMENT_H_
#define TCSEGMENT_H_

#include "../../Library/stdint.h"
#include <src_core/TlmCmd/Ccsds/cmd_space_packet.h>

#define TCS_HEADER_SIZE (1u)
#define TCS_PACKET_MAX_LENGTH (TCS_HEADER_SIZE + CSP_MAX_LEN)

typedef struct
{
  uint8_t packet[TCS_PACKET_MAX_LENGTH];
} TCSegment;

typedef enum
{
  TCS_SEQ_FIRST  = 1, // 01b: First Segment
  TCS_SEQ_CONT   = 0, // 00b: Continuing Segment
  TCS_SEQ_LAST   = 2, // 10b: Last Segment
  TCS_SEQ_SINGLE = 3  // 11b: No Segmentation
} TCS_SEQ_FLAG;

typedef enum
{
  TCS_MAP_ID_DHU_HDC = 1, // 000001b: DHU Hard Decode Command Packet
  TCS_MAP_ID_NORMAL  = 2, // 000010b: Normal Command Packet
  TCS_MAP_ID_LONG    = 4, // 000100b: Long Command Packet
  TCS_MAP_ID_UNKNOWN
} TCS_MAP_ID;

TCS_SEQ_FLAG TCS_get_seq_flag(const TCSegment* tcs);

TCS_MAP_ID TCS_get_map_id(const TCSegment* tcs);

const CmdSpacePacket* TCS_get_command_space_packet(const TCSegment* tcs);

#endif
