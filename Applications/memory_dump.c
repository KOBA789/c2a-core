#pragma section REPRO
#include "memory_dump.h"

#include <string.h> // for memcpy

#include "../System/TimeManager/time_manager.h"
#include "../CmdTlm/packet_handler.h"
#include "../Library/endian_memcpy.h"

static MemoryDump memory_dump_;
const MemoryDump* const memory_dump = &memory_dump_;

static CTCP MEM_ctcp_; // �f�[�^�T�C�Y���傫���̂�static�m��

static void MEM_init_(void);

static uint8_t MEM_get_next_adu_counter_(void);

static CCP_EXEC_STS MEM_dump_region_(uint8_t category,
                                     uint8_t num_dumps);

static MEM_ACK MEM_form_packet_(TCP* packet,
                                uint8_t category);

static TCP_SEQ_FLAG MEM_judge_seq_flag_(uint32_t rp,
                                        size_t len);

static void MEM_setup_header_(TCP* packet,
                              size_t data_len,
                              uint8_t category,
                              uint8_t adu_cnt,
                              TCP_SEQ_FLAG adu_seq_flag,
                              uint16_t adu_seq_cnt,
                              uint32_t adu_len);

static void MEM_send_packet_(const CTCP* packet,
                             uint8_t num_dumps);

AppInfo MEM_create_app(void)
{
  return create_app_info("mem", MEM_init_, NULL);
}

static void MEM_init_(void)
{
  memory_dump_.begin = 0;
  memory_dump_.end = 0;
  memory_dump_.adu_size = 0;
  memory_dump_.adu_seq = 0;
  memory_dump_.adu_counter = 0;
}

CCP_EXEC_STS Cmd_MEM_SET_REGION(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint32_t begin, end, span;

  // �p�����[�^��ǂݏo��
  endian_memcpy(&begin, param, 4);
  endian_memcpy(&end,   param + 4, 4);

  if (begin > end)
  {
    // �̈�̊J�n�ƏI���̑召�֌W���t�̏ꍇ�ُ͈�I��
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }

  span = end - begin;

  if (span > MEM_MAX_SPAN)
  {
    // �w��_���v�����ő�ʂ𒴂��Ă���ꍇ�ُ͈�I���B
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  memory_dump_.begin = begin;
  memory_dump_.end = end;
  memory_dump_.adu_size = span;
  memory_dump_.adu_seq = 0;

  // �̈�ݒ�1�񖈂ɓƗ�����ADU�J�E���g�l�����蓖�Ă�B
  memory_dump_.adu_counter = MEM_get_next_adu_counter_();

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_MEM_DUMP_REGION_SEQ(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint8_t category, num_dumps;

  // �p�����[�^��ǂݏo��
  category = param[0];
  num_dumps = param[1];

  if (num_dumps >= 8)
  {
    // �p�P�b�g�����񐔂̏����8��Ƃ���B
    // 32kbps�ł�DL����8VCDU/sec��1�b���̒ʐM�ʁB
    // ����𒴂���ꍇ�͕�����R�}���h�𑗐M���đΉ�����B
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  return MEM_dump_region_(category, num_dumps);
}

CCP_EXEC_STS Cmd_MEM_DUMP_REGION_RND(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint8_t category, num_dumps;
  uint16_t adu_seq;
  uint32_t rp;

  // �p�����[�^��ǂݏo��
  category = param[0];
  num_dumps = param[1];

  if (num_dumps >= 8)
  {
    // �p�P�b�g�����񐔂̏����8��Ƃ���B
    // 32kbps�ł�DL����8VCDU/sec��1�b���̒ʐM�ʁB
    // ����𒴂���ꍇ�͕�����R�}���h�𑗐M���đΉ�����B
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  endian_memcpy(&adu_seq, param + 2, 2);

  rp = memory_dump_.begin + (adu_seq * MEM_DUMP_WIDTH);

  if (rp < memory_dump_.end)
  {
    // �w�肳�ꂽADU Sequence Counter���̈���Ȃ�l����荞�݁A
    // �w�肳�ꂽ�ʒu�̓��e���_���v�B
    // �u�w�肷�� = �_���v�������v�Ɣ��f���Ă���
    memory_dump_.adu_seq = adu_seq;
    return MEM_dump_region_(category, num_dumps);
  }
  else
  {
    // �w�肳�ꂽADU Sequence Counter���̈�O�ł���Έُ�I��
    return CCP_EXEC_ILLEGAL_CONTEXT;
  }
}

CCP_EXEC_STS Cmd_MEM_DUMP_SINGLE(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint8_t category, num_dumps;
  uint32_t start_addr;
  uint8_t* data;

  // �p�����[�^��ǂݏo��
  category = param[0];
  num_dumps = param[1];

  if (num_dumps >= 8)
  {
    // �p�P�b�g�����񐔂̏����8��Ƃ���B
    // 32kbps�ł�DL����8VCDU/sec��1�b���̒ʐM�ʁB
    // ����𒴂���ꍇ�͕�����R�}���h�𑗐M���đΉ�����B
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  endian_memcpy(&start_addr, param + 2, 4);

  // �v����: �w��A�h���X�l���ُ�ȏꍇ�̏��������ׂ����H
  // Segmentation Fault�Ƃ��N����H

  // �������_���v�p�P�b�g�̃w�b�_��ݒ�
  // Packet Sequence Counter for each ADU�͒P�p�P�b�g�Ȃ̂�0�Œ�B
  // ADU�����`���ɂ��Ȃ����5Bytes���_���v�̈�𑝂₹�邪�A
  // Packet ID���ʂɂ���K�v��������̂łЂƂ܂��`�������킹�Ď�������B
  MEM_setup_header_(&MEM_ctcp_,
                    MEM_DUMP_WIDTH,              // 1�p�P�b�g�̍ő咷���_���v����
                    category,                    // ���o�J�e�S���̓p�����[�^�Ŏw��
                    MEM_get_next_adu_counter_(), // �PADU�Ȃ̂�ADU�J�E���g�l�𒼐ڊ��蓖��
                    TCP_SEQ_SINGLE,              // �A�h���X�w��̏ꍇ�͒PADU�Ƃ��ď���
                    0x0000,                      // �PADU�Ȃ̂�0x0000�ɌŒ�
                    MEM_DUMP_WIDTH);             // ADU����DUMP���Ɠ���

  // �_���v�f�[�^��ݒ�
  data = TCP_TLM_get_user_data_head(&MEM_ctcp_) + 5; // ADU�����ɂ��w�b�_����5Bytes������
  memcpy(data, (const void*)start_addr, MEM_DUMP_WIDTH);

  // ���������p�P�b�g�𑗏o
  MEM_send_packet_(&MEM_ctcp_, num_dumps);

  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_MEM_LOAD(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  size_t param_len = CCP_get_param_len(packet);
  size_t data_len;
  uint32_t start_addr;

  // �f�[�^���ǂݏo��
  // �f�[�^�� -> �p�����[�^�S��-�J�n�A�h���X�p�����[�^��
  data_len = param_len - 4;

  // �������݃A�h���X�ǂݏo��
  endian_memcpy(&start_addr, param, 4);

  // �w�肵���J�n�A�h���X����n�܂�̈�Ƀf�[�^����������
  memcpy((void*)start_addr, &(param[4]), data_len);
  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_MEM_SET_DESTINATION(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint32_t dest;

  endian_memcpy(&dest, param, 4);

  if ((dest >= memory_dump_.begin) && (dest < memory_dump_.end))
  {
    // ����A�h���X���̈�����Ɋ܂܂��ꍇ�B
    // �����F�߂�Ə��������G�ɂȂ�̂ŋ֎~����B
    return CCP_EXEC_ILLEGAL_PARAMETER;
  }

  // ����A�h���X��ݒ肵�ARP��̈�擪�ɍ��킹��B
  memory_dump_.dest = dest;
  memory_dump_.rp = memory_dump_.begin;
  return CCP_EXEC_SUCCESS;
}

CCP_EXEC_STS Cmd_MEM_COPY_REGION_SEQ(const CTCP* packet)
{
  const uint8_t* param = CCP_get_param_head(packet);
  uint32_t copy_width, wp;

  if (memory_dump_.rp == memory_dump_.end)
  {
    // ���ɗ̈�S�̂̓ǂݏo�����������Ă���ꍇ�B
    // �����͍s�킸����I������B
    return CCP_EXEC_SUCCESS;
  }

  // �p�����[�^�ǂݏo���B
  endian_memcpy(&copy_width, param, 4);

  if ((memory_dump_.rp + copy_width) > memory_dump_.end)
  {
    // �w��R�s�[���ŃR�s�[����Ɨ̈�O�ɏo�Ă��܂��ꍇ�B
    // �R�s�[���ʂ��̈�I�[�Ɉ�v����悤�R�s�[�����N���b�v�B
    copy_width = memory_dump_.end - memory_dump_.rp;
  }

  // WP�v�Z�BWP = ����擪+�ǂݏo���ς݃f�[�^�ʁB
  wp = memory_dump_.dest + (memory_dump_.rp - memory_dump_.begin);

  // �w�肳�ꂽ�R�s�[�������̈���R�s�[���ARP���X�V�B
  memcpy((uint8_t*)wp, (const uint8_t*)memory_dump_.rp, copy_width);
  memory_dump_.rp += copy_width;
  return CCP_EXEC_SUCCESS;
}

static uint8_t MEM_get_next_adu_counter_(void)
{
  // �C���N�������g��̒l��Ԃ��̂ŏ����l��0xff�Ƃ���B
  static uint8_t adu_counter_ = 0xff;
  return ++adu_counter_;
}

static CCP_EXEC_STS MEM_dump_region_(uint8_t category,
                                     uint8_t num_dumps)
{
  MEM_ACK ack;

  // �ݒ�l�ɂ��ƂÂ����o���ׂ��p�P�b�g���\�z
  ack = MEM_form_packet_(&MEM_ctcp_, category);

  switch (ack)
  {
  case MEM_SUCCESS:
    // ���������p�P�b�g�𑗏o���AADU Sequence Counter�̒l���X�V
    MEM_send_packet_(&MEM_ctcp_, num_dumps);
    ++memory_dump_.adu_seq;
    return CCP_EXEC_SUCCESS;

  case MEM_NO_DATA:
    // ���łɑS�̈�_���v�ς݂Ȃ牽�������I��
    return CCP_EXEC_SUCCESS;

  default:
    // ����ȊO�̃G���[�͂Ȃ��͂�
    return CCP_EXEC_UNKNOWN;
  }
}

static MEM_ACK MEM_form_packet_(TCP* packet,
                                uint8_t category)
{
  uint32_t rp;
  size_t len;
  TCP_SEQ_FLAG seq_flag;
  uint8_t* data;

  rp = memory_dump_.begin + (memory_dump_.adu_seq * MEM_DUMP_WIDTH);

  if (rp >= memory_dump_.end)
  {
    // Read Pointer���I�[�ɒB���Ă���Ή��������I��
    return MEM_NO_DATA;
  }

  // �c��_���v�����v�Z
  len = memory_dump_.end - rp;

  if (len > MEM_DUMP_WIDTH)
  {
    // 1�p�P�b�g�̃_���v���𒴂���ꍇ�̓_���v���ɉ�������B
    len = MEM_DUMP_WIDTH;
  }

  // Packet Sequence Flag�̎�ʔ���
  seq_flag = MEM_judge_seq_flag_(rp, len);

  // �������_���v�p�P�b�g�̃w�b�_��ݒ�
  MEM_setup_header_(&MEM_ctcp_,
                    len,
                    category,
                    memory_dump_.adu_counter,
                    seq_flag,
                    memory_dump_.adu_seq,
                    memory_dump_.adu_size);

  // �_���v�f�[�^��ݒ�
  data = TCP_TLM_get_user_data_head(packet) + 5; // ADU�����ɂ��w�b�_����5Bytes������
  memcpy(data, (const void*)rp, len);

  return MEM_SUCCESS;
}

static TCP_SEQ_FLAG MEM_judge_seq_flag_(uint32_t rp,
                                        size_t len)
{
  if (memory_dump_.adu_size < MEM_DUMP_WIDTH)
  {
    // ADU���P�p�P�b�g�Ɏ��܂�ꍇ
    return TCP_SEQ_SINGLE;
  }
  else
  {
    // ADU�������p�P�b�g�ɂ܂�����ꍇ
    if (rp == memory_dump_.begin)
    {
      // Read Pointer���ǂݏo���̈�擪�Ɉ�v����Ȃ�J�n�p�P�b�g
      return TCP_SEQ_FIRST;
    }
    else if (len < MEM_DUMP_WIDTH)
    {
      // �_���v�c�ʂ�1�p�P�b�g�ȓ��Ɏ��܂�Ȃ�I���p�P�b�g
      return TCP_SEQ_LAST;
    }
    else
    {
      // ����ȊO�̏ꍇ�͒��ԃp�P�b�g
      return TCP_SEQ_CONT;
    }
  }
}

static void MEM_setup_header_(TCP* packet,
                              size_t data_len,
                              uint8_t category,
                              uint8_t adu_cnt,
                              TCP_SEQ_FLAG adu_seq_flag,
                              uint16_t adu_seq_cnt,
                              uint32_t adu_len)
{
  uint8_t* data;

  // TCPacket�w�b�_��ݒ�
  TCP_TLM_setup_primary_hdr(packet, TCP_APID_DUMP_TLM, (uint16_t)(data_len + 12));
  TCP_TLM_set_ti(packet, (uint32_t)( TMGR_get_master_total_cycle() ));
  TCP_TLM_set_category(packet, category); // �p�����[�^�ɂ��w��
  TCP_TLM_set_packet_id(packet, (uint8_t)MEM_TLM_ID);
  TCP_TLM_set_adu_cnt(packet, adu_cnt);
  TCP_TLM_set_adu_seq_flag(packet, adu_seq_flag);

  // ADU��������Secondary Header�ǉ�����ݒ�
  data = TCP_TLM_get_user_data_head(packet);
  data[0] = (uint8_t)(adu_seq_cnt >> 8);
  data[1] = (uint8_t)(adu_seq_cnt);
  data += 2;
  // ADU Length��ݒ� 3Bytes���Ȃ̂Œ���
  data[0] = (uint8_t)(adu_len >> 16);
  data[1] = (uint8_t)(adu_len >> 8);
  data[2] = (uint8_t)(adu_len);
}

static void MEM_send_packet_(const CTCP* packet,
                             uint8_t num_dumps)
{
  int i;

  // TLM���o����
  for (i = 0; i < num_dumps; ++i)
  {
    // num_dumps�̉񐔂������������p�P�b�g��z�������֓n��
    PH_analyze_packet(packet);
  }
}

#pragma section
