#pragma section REPRO
/**
 * @file   driver_super.c
 * @brief  �e����Z���T�E�A�N�`���G�[�^���Ƃ̃C���^�[�t�F�[�X�h���C�o�Q�̃X�[�p�[�N���X
 *
 *         DriverSuper Class ��
 *         �e����Z���T�E�A�N�`���G�[�^���Ƃ̃C���^�[�t�F�[�X���������C
 *         �������C�R�}���h���s�C�e�����g�����N�G�X�g�C�e�����g����M�C�e�����g����͂Ȃǂ��s���C�h���C�o�Q�̃X�[�p�[�N���X�ł��D
 *         �X�̋@��̃C���^�[�t�F�[�X�h���C�o�Ɍp�������Ďg�p���܂��D
 */

#include "driver_super.h"
#include "../../Library/print.h"
#include <string.h>     // for memset�Ȃǂ�mem�n
#include <stddef.h>     // for NULL

// #define DS_DEBUG                       // �K�؂ȂƂ��ɃR�����g�A�E�g����
// #define DS_DEBUG_SHOW_REC_DATA         // �K�؂ȂƂ��ɃR�����g�A�E�g����

static DS_ERR_CODE DS_send_cmd_(DriverSuper* p_super, uint8_t stream);
static int      DS_tx_(DriverSuper* p_super, uint8_t stream);
static int      DS_rx_(DriverSuper* p_super);
static void     DS_analyze_rx_buffer_(DriverSuper* p_super,
                                      uint8_t stream,
                                      uint16_t rec_data_len);
static uint16_t DS_analyze_rx_buffer_prepare_buffer_(DriverSuper* p_super,
                                                     uint8_t stream,
                                                     uint8_t* rx_buffer,
                                                     uint16_t rec_data_len);
static uint16_t DS_analyze_rx_buffer_pickup_(DS_StreamConfig* p_stream_config,
                                             uint8_t* rx_buffer,
                                             uint16_t rec_data_len);
static void     DS_analyze_rx_buffer_carry_over_buffer_(DS_StreamConfig* p_stream_config,
                                                        uint8_t* rx_buffer,
                                                        uint16_t total_processed_data_len,
                                                        uint16_t rec_data_len);
static uint16_t DS_analyze_rx_buffer_fixed_pickup_(DS_StreamConfig* p_stream_config,
                                                   uint8_t* rx_buffer,
                                                   uint16_t total_processed_data_len,
                                                   uint16_t rec_data_len);
static uint16_t DS_analyze_rx_buffer_variable_pickup_with_rx_frame_size_(DS_StreamConfig* p_stream_config,
                                                                         uint8_t* rx_buffer,
                                                                         uint16_t total_processed_data_len,
                                                                         uint16_t rec_data_len);
static uint16_t DS_analyze_rx_buffer_variable_pickup_with_footer_(DS_StreamConfig* p_stream_config,
                                                                  uint8_t* rx_buffer,
                                                                  uint16_t total_processed_data_len,
                                                                  uint16_t rec_data_len);
static uint16_t DS_analyze_rx_buffer_finding_header_(DS_StreamConfig* p_stream_config,
                                                     uint8_t* rx_buffer,
                                                     uint16_t total_processed_data_len,
                                                     uint16_t rec_data_len);
static uint16_t DS_analyze_rx_buffer_receiving_header_(DS_StreamConfig* p_stream_config,
                                                       uint8_t* rx_buffer,
                                                       uint16_t total_processed_data_len);
static uint16_t DS_analyze_rx_buffer_receiving_footer_(DS_StreamConfig* p_stream_config,
                                                       uint8_t* rx_buffer,
                                                       uint16_t total_processed_data_len,
                                                       uint16_t rx_frame_size);
static uint32_t DS_analyze_rx_buffer_get_framelength_(DS_StreamConfig* p_stream_config);

static DS_ERR_CODE DS_reset_stream_config_(DS_StreamConfig* p_stream_config);
static DS_ERR_CODE DS_validate_stream_config_(DS_StreamConfig* p_stream_config);

// �_�~�[�֐�
// EQU���Ɗ֐��|�C���^�̏����l��NULL�ɂ��Ă������߂ɂʂ�ۂŎ��̂����̂�
static DS_ERR_CODE DS_load_init_setting_dummy_(DriverSuper* p_super);
static DS_ERR_CODE DS_data_analyzer_dummy_(DS_StreamConfig* p_stream_config, void* p_driver);


// ###### DriverSuper��{�֐� ######

DS_ERR_CODE DS_init(DriverSuper* p_super, void* if_config, DS_ERR_CODE (*load_init_setting)(DriverSuper* p_super))
{
  if (DS_reset(p_super) != DS_ERR_CODE_OK) return DS_ERR_CODE_ERR;

  p_super->if_config = if_config;

  p_super->config.load_init_setting = load_init_setting;
  if (p_super->config.load_init_setting(p_super) != DS_ERR_CODE_OK) return DS_ERR_CODE_ERR;

  if (DS_validate_config(p_super) != DS_ERR_CODE_OK) return DS_ERR_CODE_ERR;

  // IF�̏�����
  // ��U�V���v����IF_init�̃G���[�R�[�h�͖�������i���@�ł����ŃG���[�o��ꍇ�̓R�[�h�����������̂ŁD�K�v������Ώ��������D�j
  if ( (*IF_init[p_super->interface])(p_super->if_config) != 0 ) return DS_ERR_CODE_ERR;

  return DS_ERR_CODE_OK;
}


DS_ERR_CODE DS_reset(DriverSuper* p_super)
{
  uint8_t stream;

  p_super->interface = IF_LIST_MAX; // FIXME: (*IF_init[p_super->interface])(p_super->if_config) �̗l�Ȏg����������̂ŃZ�O�t�H���N����\��������
  p_super->if_config = NULL;        // FIXME: NULL�|�C���^�͂��̊֐���Reset�P�̂Ŏg����ƃ}�Y��

  memset(p_super->config.rx_buffer_, 0x00, sizeof(p_super->config.rx_buffer_));

  p_super->config.load_init_setting = DS_load_init_setting_dummy_;

  p_super->config.rec_status_.ret_from_if_rx       = 0;
  p_super->config.rec_status_.rx_disruption_status = DS_RX_DISRUPTION_STATUS_OK;

  p_super->config.rx_count_      = 0;
  p_super->config.rx_call_count_ = 0;
  p_super->config.rx_time_       = TMGR_get_master_clock();

  p_super->config.should_monitor_for_rx_disruption_ = 0;
  p_super->config.time_threshold_for_rx_disruption_ = 60 * 1000;      // ���̒l�͂悭�l���邱��

  for (stream = 0; stream < DS_STREAM_MAX; ++stream)
  {
    DS_ERR_CODE ret = DS_reset_stream_config_(&p_super->stream_config[stream]);
    if (ret != DS_ERR_CODE_OK) return ret;
  }
  return DS_ERR_CODE_OK;
}


DS_ERR_CODE DS_validate_config(DriverSuper* p_super)
{
  uint8_t stream;

  if (p_super->interface < 0 || p_super->interface >= IF_LIST_MAX) return DS_ERR_CODE_ERR;
  if (p_super->if_config == NULL) return DS_ERR_CODE_ERR;

  for (stream = 0; stream < DS_STREAM_MAX; ++stream)
  {
    DS_ERR_CODE ret = DS_validate_stream_config_(&p_super->stream_config[stream]);
    if (ret != DS_ERR_CODE_OK) return ret;
  }

  return DS_ERR_CODE_OK;
}

DS_ERR_CODE DS_clear_rx_buffer(DriverSuper* p_super)
{
  uint8_t stream;

  // �ȉ��C�e�� buffer �� memset�ŔO�̈�0�N���A���Ă������C
  // ���� carry_over_buffer_size_ �ɂ���̂ŁC�����Ӗ��͂Ȃ��D
  memset(p_super->config.rx_buffer_, 0x00, sizeof(p_super->config.rx_buffer_));

  for (stream = 0; stream < DS_STREAM_MAX; ++stream)
  {
    p_super->stream_config[stream].rx_frame_rec_len_ = 0;
    p_super->stream_config[stream].is_rx_buffer_carry_over_ = 0;
    p_super->stream_config[stream].carry_over_buffer_size_  = 0;

    memset(p_super->stream_config[stream].rx_frame_,
           0x00,
           sizeof(p_super->stream_config[stream].rx_frame_));
    memset(p_super->stream_config[stream].rx_buffer_for_carry_over_,
           0x00,
           sizeof(p_super->stream_config[stream].rx_buffer_for_carry_over_));
  }

  return DS_ERR_CODE_OK;
}


DS_ERR_CODE DS_receive(DriverSuper* p_super)
{
  uint8_t  stream;
  uint16_t rec_data_len;
  int      ret_rx;

  p_super->config.rx_call_count_++;

  // �eDriver�ŕ����I�ɐڑ�����Ă��� wire �͂P�{�Ȃ̂ŁC����������Ŏ�M����D
  // ��i�� stream �ł́C���̎�M�����r�b�g��ɑ΂��āC�����̃t���[����ނɑ΂��āC�t���[���T���C�m�菈���𑖂炷�D
  ret_rx = DS_rx_(p_super);
  p_super->config.rec_status_.ret_from_if_rx = ret_rx;

  if (ret_rx > 0)
  {
    // �Ȃɂ�����̎�M�f�[�^����
    p_super->config.rx_count_++;
    p_super->config.rx_time_ = TMGR_get_master_clock();
  }

  // ��M�r�┻��
  // �e�����ȂǂŌ���Ƃ��Ƀm�C�Y�ɂȂ�̂ŁC���肵�Ȃ��Ƃ��� OK �ɂ��Ă���
  p_super->config.rec_status_.rx_disruption_status = DS_RX_DISRUPTION_STATUS_OK;
  if (p_super->config.should_monitor_for_rx_disruption_)
  {
    ObcTime now = TMGR_get_master_clock();
    uint32_t last_rx_ago = OBCT_diff_in_msec(&p_super->config.rx_time_, &now);

    if (last_rx_ago > p_super->config.time_threshold_for_rx_disruption_)
    {
      p_super->config.rec_status_.rx_disruption_status = DS_RX_DISRUPTION_STATUS_LOST;
    }
  }

  for (stream = 0; stream < DS_STREAM_MAX; ++stream)
  {
    DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);
    if (!p_stream_config->is_enabled_)
    {
      p_stream_config->rec_status_.status_code = DS_STREAM_REC_STATUS_DISABLE;
      continue;
    }

    // setter �� validation ������ƁC�������Ȃǂŉ��x�������邱�Ƃ�C
    // ������������ validation �͑ł��グ���Ƃ������ނ���n�㎎�����ɗL�p�Ȃ̂ŁC�����ɒu��
    if (p_stream_config->is_validation_needed_for_rec_)
    {
      DS_ERR_CODE ret = DS_validate_stream_config_(p_stream_config);
      if (ret != DS_ERR_CODE_OK)
      {
        p_stream_config->rec_status_.status_code = DS_STREAM_REC_STATUS_VALIDATE_ERR;
        continue;
      }
    }

    if (ret_rx < 0)     // ���̋��ʏ����� for �̒��ɂ���́C�኱�L�����D�D�D
    {
      // RX���s
      p_stream_config->rec_status_.status_code = DS_STREAM_REC_STATUS_RX_ERR;
      // [TODO] ������ DriverSuper �Ƃ��Ă̋��ʂ̃A�m�}�����s�����邩�͗v�c�_
      //        �ʑΉ��Ȃ̂ŁC Driver �Ɏ�������C�Ȃ����́C IF ���Ɏ�������̂����R���H
      continue;
    }
    else if (ret_rx == 0)
    {
      // ��M�f�[�^�Ȃ�
      // �J�z�f�[�^������Ώ�������
      if (p_stream_config->is_rx_buffer_carry_over_)
      {
        // �J�z������̂ŁC������ continue �����֎���
      }
      else
      {
        // rec_status_.status_code ��{�͍X�V����
        // FIXED �̏ꍇ�̓��Z�b�g
        if (p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_FIXED_FRAME)
        {
          p_stream_config->rec_status_.status_code = DS_STREAM_REC_STATUS_FINDING_HEADER;
        }
        continue;
      }
    }
    else
    {
      // �������Ȃ�
    }

    // �����܂ł������M�f�[�^����

    rec_data_len = (uint16_t)ret_rx;      // �����܂ł���Δ񕉐�
    DS_analyze_rx_buffer_(p_super, stream, rec_data_len);

    // �t���[���m�菈��
    if (p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_FIXED_FRAME)
    {
      p_stream_config->rx_frame_fix_count_++;
      p_stream_config->rx_frame_fix_time_ = TMGR_get_master_clock();
      p_stream_config->req_tlm_cmd_tx_count_after_last_tx_ = 0;
    }
  }

  // stream�̃e�����r�┻��i�e�����t���[���m��r�┻��j
  for (stream = 0; stream < DS_STREAM_MAX; ++stream)
  {
    DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);
    ObcTime now;
    uint32_t last_tlm_fix_ago;

    // �e�����ȂǂŌ���Ƃ��Ƀm�C�Y�ɂȂ�̂ŁC���肵�Ȃ��Ƃ��� OK �ɂ��Ă���
    p_stream_config->rec_status_.tlm_disruption_status = DS_STREAM_TLM_DISRUPTION_STATUS_OK;

    if (!p_stream_config->is_enabled_ || !p_stream_config->should_monitor_for_tlm_disruption_)
    {
      continue;
    }

    now = TMGR_get_master_clock();
    last_tlm_fix_ago = OBCT_diff_in_msec(&p_stream_config->rx_frame_fix_time_, &now);

    if (last_tlm_fix_ago > p_stream_config->time_threshold_for_tlm_disruption_)
    {
      p_stream_config->rec_status_.tlm_disruption_status = DS_STREAM_TLM_DISRUPTION_STATUS_LOST;
    }
  }

  return (ret_rx < 0) ? DS_ERR_CODE_ERR : DS_ERR_CODE_OK;
}


DS_ERR_CODE DS_analyze_rec_data(DriverSuper* p_super, uint8_t stream, void* p_driver)
{
  DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);

  p_stream_config->ret_from_data_analyzer_ = p_stream_config->data_analyzer_(p_stream_config, p_driver);

  return p_stream_config->ret_from_data_analyzer_;
}


DS_ERR_CODE DS_send_general_cmd(DriverSuper* p_super, uint8_t stream)
{
  DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);

  if (!p_stream_config->is_enabled_)
  {
    p_stream_config->send_status_.status_code = DS_STREAM_SEND_STATUS_DISABLE;
    return DS_ERR_CODE_OK;
  }

  p_stream_config->general_cmd_tx_count_++;
  p_stream_config->general_cmd_tx_time_ = TMGR_get_master_clock();

#ifdef DS_DEBUG
  Printf("DS: send_general_cmd\n");
#endif

  return DS_send_cmd_(p_super, stream);
}


DS_ERR_CODE DS_send_req_tlm_cmd(DriverSuper* p_super, uint8_t stream)
{
  DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);

  if (!p_stream_config->is_enabled_)
  {
    p_stream_config->send_status_.status_code = DS_STREAM_SEND_STATUS_DISABLE;
    return DS_ERR_CODE_OK;
  }

  p_stream_config->req_tlm_cmd_tx_count_++;
  p_stream_config->req_tlm_cmd_tx_count_after_last_tx_++;
  p_stream_config->req_tlm_cmd_tx_time_ = TMGR_get_master_clock();

#ifdef DS_DEBUG
  Printf("DS: send_req_tlm_cmd\n");
#endif

  return DS_send_cmd_(p_super, stream);
}


/**
 * @brief  �R�}���h���M����
 *
 *         DS_send_general_cmd �� DS_send_req_tlm_cmd�̋��ʕ���
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @param  stream   �ǂ�config���g�p���邩�Dstream��0-MAX�Ȃ̂ŁC�p�����ENUM�Ȃǐ錾���Ďg���₷������΂����Ǝv���D
 * @retval DS_ERR_CODE_OK  : ����I��
 * @retval DS_ERR_CODE_ERR : IF_TX �ł̃G���[����
 * @note   ��M�󋵂�G���[���� send_status_ �Ɋi�[����Ă���
 */
static DS_ERR_CODE DS_send_cmd_(DriverSuper* p_super, uint8_t stream)
{
  DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);

  // setter �� validation ������ƁC�������Ȃǂŉ��x�������邱�Ƃ�C
  // ������������ validation �͑ł��グ���Ƃ������ނ���n�㎎�����ɗL�p�Ȃ̂ŁC�����ɒu��
  if (p_stream_config->is_validation_needed_for_send_)
  {
    DS_ERR_CODE ret = DS_validate_stream_config_(p_stream_config);
    if (ret != DS_ERR_CODE_OK)
    {
      p_stream_config->send_status_.status_code = DS_STREAM_SEND_STATUS_VALIDATE_ERR;
      return DS_ERR_CODE_ERR;
    }
  }

  p_stream_config->send_status_.ret_from_if_tx = DS_tx_(p_super, stream);

  if (p_stream_config->send_status_.ret_from_if_tx != 0)
  {
    p_stream_config->send_status_.status_code = DS_STREAM_SEND_STATUS_TX_ERR;
    return DS_ERR_CODE_ERR;
  }

  p_stream_config->send_status_.status_code = DS_STREAM_SEND_STATUS_OK;
  return DS_ERR_CODE_OK;
}

/**
 * @brief  �p����̋@��ɃR�}���h�𔭍s����
 * @note   ���̊֐��̎��s�O�ɁCtx_frame_, tx_frame_size_�̐ݒ肪�K�v�ł���
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @param  stream   �ǂ�config���g�p���邩�Dstream��0-MAX�Ȃ̂ŁC�p�����ENUM�Ȃǐ錾���Ďg���₷������΂����Ǝv���D
 * @retval DS_ERR_CODE_OK (0) : ����I��
 * @retval 0�ȊO : IF_TX �̖߂�l
 */
static int DS_tx_(DriverSuper* p_super, uint8_t stream)
{
  int ret;
  DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);

  if (p_stream_config->tx_frame_size_ == 0) return DS_ERR_CODE_OK;
  if (p_stream_config->tx_frame_ == NULL) return DS_ERR_CODE_OK;

#ifdef DS_DEBUG
  Printf("DS: tx_\n");
#endif

  ret = (*IF_TX[p_super->interface])(p_super->if_config,
                                     p_stream_config->tx_frame_,
                                     (int)p_stream_config->tx_frame_size_);

  if (ret != 0) return ret;
  return DS_ERR_CODE_OK;
}


/**
 * @brief  �p����̋@�킩��̎�M�f�[�^�����邩�m�F���C��M����
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @retval 0    : ��M�f�[�^�Ȃ�
 * @retval ���� : ��M�f�[�^�� [Byte]
 * @retval ���� : IF_RX�̃G���[
 */
static int DS_rx_(DriverSuper* p_super)
{
  int flag;
  int rec_data_len;
  uint8_t stream;
#ifdef DS_DEBUG_SHOW_REC_DATA
  int16_t i;
#endif

  // ���Ȃ��Ƃ��P��stream���L���ł��Crx_frame_size_��0�ȊO�łȂ��Ǝ�M�����͂��Ȃ�
  flag = 0;
  for (stream = 0; stream < DS_STREAM_MAX; ++stream)
  {
    if (! (p_super->stream_config[stream].is_enabled_) ) continue;
    if (p_super->stream_config[stream].rx_frame_size_ != 0)
    {
      flag = 1;
    }
  }
  if (flag == 0) return 0;

  rec_data_len = (*IF_RX[p_super->interface])(p_super->if_config,
                                              p_super->config.rx_buffer_,
                                              DS_RX_BUFFER_SIZE_MAX);

#ifdef DS_DEBUG
  Printf("DS: rx_\n");
#endif

  if (rec_data_len <= 0) return rec_data_len;     // �����̓G���[�R�[�h

#ifdef DS_DEBUG_SHOW_REC_DATA
  Printf("DS: Receive data size is %d bytes, as follow:\n", rec_data_len);
  for (i = 0; i < rec_data_len; i++)
  {
    Printf("%02x ", p_super->config.rx_buffer_[i]);
    if (i % 4 == 3) Printf("   ");
  }
  Printf("\n");
#endif

  return rec_data_len;
}


/**
 * @brief  ��M�t���[����͊֐�
 * @param  *p_super      DriverSuper�\���̂ւ̃|�C���^
 * @param  stream        �ǂ�config���g�p���邩�Dstream��0-MAX�Ȃ̂ŁC�p�����ENUM�Ȃǐ錾���Ďg���₷������΂����Ǝv���D
 * @param  rec_data_len  ��M�f�[�^�̃o�b�t�@�̒���
 * @return void �ڍׂ� DS_StreamRecStatus
 */
static void DS_analyze_rx_buffer_(DriverSuper* p_super,
                                  uint8_t stream,
                                  uint16_t rec_data_len)
{
  // ��͗p��M�o�b�t�@
  // ����ȃf�[�^�Ȃ̂ŁCstatic�ŗ\�ߊm�ۂ��Ă����C���s���̃X�^�b�N�͊��������
  static uint8_t rx_buffer[DS_RX_BUFFER_SIZE_MAX * 2];
  DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);
  uint16_t total_processed_data_len;

  rec_data_len = DS_analyze_rx_buffer_prepare_buffer_(p_super, stream, rx_buffer, rec_data_len);

  total_processed_data_len = DS_analyze_rx_buffer_pickup_(p_stream_config, rx_buffer, rec_data_len);

  DS_analyze_rx_buffer_carry_over_buffer_(p_stream_config, rx_buffer, total_processed_data_len, rec_data_len);
}


/**
 * @brief  ��͗p��M�o�b�t�@�̏���
 *
 *         �J��z���ꂽ�f�[�^�ƍ����M�����f�[�^�̌������s���C��M�f�[�^��͂̏���������
 * @param[in]  *p_super      DriverSuper�\���̂ւ̃|�C���^
 * @param[in]  stream        �ǂ�config���g�p���邩�Dstream��0-MAX�Ȃ̂ŁC�p�����ENUM�Ȃǐ錾���Ďg���₷������΂����Ǝv���D
 * @param[out] *rx_buffer    ��͗p��M�o�b�t�@
 * @param[in]  rec_data_len  ��M�f�[�^�̃o�b�t�@�̒���
 * @return ��͗p��M�o�b�t�@�̒���
 */
static uint16_t DS_analyze_rx_buffer_prepare_buffer_(DriverSuper* p_super,
                                                     uint8_t stream,
                                                     uint8_t* rx_buffer,
                                                     uint16_t rec_data_len)
{
  DS_StreamConfig* p_stream_config = &(p_super->stream_config[stream]);
  uint16_t buffer_offset = 0;

  // �J�z��M�f�[�^�̂Ƃ肱��
  if (p_stream_config->is_rx_buffer_carry_over_)
  {
    memcpy(rx_buffer,
           p_stream_config->rx_buffer_for_carry_over_,
           (size_t)p_stream_config->carry_over_buffer_size_);
    buffer_offset += p_stream_config->carry_over_buffer_size_;
  }

  // �����M���̂Ƃ肱��
  memcpy(&(rx_buffer[buffer_offset]),
         p_super->config.rx_buffer_,
         (size_t)rec_data_len);

  if (p_stream_config->is_rx_buffer_carry_over_)
  {
    rec_data_len += p_stream_config->carry_over_buffer_size_;
  }

  return rec_data_len;
}


/**
 * @brief  �t���[����͊֐�
 *
 *         ��͗p��M�o�b�t�@�𑖍����C�o�C�g�P�ʂŃt���[�����f�[�^���E���Ă���
 * @param  p_stream_config DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer       ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  rec_data_len    ��M�f�[�^�̃o�b�t�@�̒���
 * @return ����̌Ăяo���ő��������o�C�g�����D���̑��̏ڍׂ� DS_StreamRecStatus
 */
static uint16_t DS_analyze_rx_buffer_pickup_(DS_StreamConfig* p_stream_config,
                                             uint8_t* rx_buffer,
                                             uint16_t rec_data_len)
{
  uint16_t total_processed_data_len;
  // ��M�o�b�t�@�̃f�[�^�𑖍����C�K�v�ȃf�[�^���t���[���Ƃ���pickup����֐�
  uint16_t (*pickup_func)(DS_StreamConfig* p_stream_config,
                          uint8_t* rx_buffer,
                          uint16_t total_processed_data_len,
                          uint16_t rec_data_len);
  p_stream_config->rx_frame_head_pos_of_frame_candidate_ = 0;

  // TODO: �r�b�O�f�[�^�Ή�
  if (p_stream_config->rx_frame_size_ > 0 && p_stream_config->rx_frame_size_ < DS_RX_FRAME_SIZE_MAX)
  {
    pickup_func = DS_analyze_rx_buffer_fixed_pickup_;
  }
  else if (p_stream_config->rx_frame_size_ > 0)
  {
    // DS_analyze_rx_buffer_fixed_bigdata_(void)
    pickup_func = NULL;
    return rec_data_len;
  }
  else if (p_stream_config->rx_frame_size_ < 0 && p_stream_config->rx_frame_size_ < DS_RX_FRAME_SIZE_MAX)
  {
    // �t���[���Ƀt���[�����f�[�^���܂܂�Ă��邩�H
    if (p_stream_config->rx_framelength_pos_ >= 0)
    {
      pickup_func = DS_analyze_rx_buffer_variable_pickup_with_rx_frame_size_;
    }
    else
    {
      pickup_func = DS_analyze_rx_buffer_variable_pickup_with_footer_;
    }
  }
  else
  {
    // DS_analyze_rx_buffer_variable_bigdata_(void)
    pickup_func = NULL;
    return rec_data_len;
  }

  // ��M�o�b�t�@����f�[�^���s�b�N�A�b�v���Ă���
  total_processed_data_len = p_stream_config->carry_over_buffer_next_pos_;
  while (total_processed_data_len < rec_data_len)
  {
    uint16_t processed_data_len = pickup_func(p_stream_config,
                                              rx_buffer,
                                              total_processed_data_len,
                                              rec_data_len);
    total_processed_data_len += processed_data_len;

    if (p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_FIXED_FRAME)
    {
      break;
    }

    // �s�������N������C���݂�frame���̐擪 + 1�o�C�g�ڂɑ����ꏊ��߂�
    if (p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_HEADER_MISMATCH ||
        p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_FOOTER_MISMATCH ||
        p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG ||
        p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT)
    {
      total_processed_data_len = (uint16_t)(p_stream_config->rx_frame_head_pos_of_frame_candidate_ + 1);

      // ���̕����ł̏�������̂��߂ɁC�ڍ׃G���[�������݂̃X�e�[�^�X�ɏ㏑������
      p_stream_config->rec_status_.status_code = DS_STREAM_REC_STATUS_FINDING_HEADER;
    }

    // processed_data_len > rec_data_len�Ȃ邱�Ƃ͂��肦�Ȃ����C�O�̈׃`���b�N����H�H
  }

  return total_processed_data_len;
}


/**
 * @brief  �t���[����͊֐���̃f�[�^�J�z�֐�
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer                ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  total_processed_data_len ��M�f�[�^�̃o�b�t�@�̂����C���łɏ������ꂽ�o�C�g��
 * @param  rec_data_len             ��M�f�[�^�̃o�b�t�@�̒���
 * @return void
 */
static void DS_analyze_rx_buffer_carry_over_buffer_(DS_StreamConfig* p_stream_config,
                                                    uint8_t* rx_buffer,
                                                    uint16_t total_processed_data_len,
                                                    uint16_t rec_data_len)
{
  // ��M�f�[�^������Ăяo���Ɉ����p���i�������̃p�^�[������j
  // �t���[���m�肵���ꍇ�i����͎�M�f�[�^������\��������j
  //     ���i�ȃt���[���T��������
  //          --> �t���[���m�肵���f�[�^��OK�Ƃ��C����Ȍ�̉�͂ł��Ȃ�������M�f�[�^�݂̂�����Ɉ����p��
  //     ���i�ȃt���[���T�����L��
  //          --> �m��t���[�������[�U�[���Œe�����\�����l�����C�m��t���[���̐擪 + 1 �o�C�g�ڈȍ~������Ɉ����p��
  //              ����̃w�b�_�T���͂�������n�܂�
  // �t���[���m�肵�Ȃ������ꍇ�i����͎�M�f�[�^�͂Ȃ��j
  //     DS_STREAM_REC_STATUS_FINDING_HEADER �̂Ƃ�
  //          --> �w�b�_���Ȃ������Ƃ������ƂȂ̂ŁC�����p���f�[�^�͂Ȃ�
  //     DS_STREAM_REC_STATUS_FINDING_HEADER �łȂ��Ƃ�
  //          --> �t���[�����̃t���[���擪�ȍ~������Ɉ����p��
  //              ����ɂ��C�t�b�^�s��v�Ȃǂ̕s���������������ꍇ�Ƀt���[���T������蒼����

  p_stream_config->carry_over_buffer_size_ = 0;
  if (p_stream_config->rec_status_.status_code == DS_STREAM_REC_STATUS_FIXED_FRAME)
  {
    if (p_stream_config->is_strict_frame_search_)
    {
      // �m��t���[���̐擪 + 1 �o�C�g�ڈȍ~������Ɉ����p��
      p_stream_config->carry_over_buffer_size_ = (uint16_t)(rec_data_len - p_stream_config->rx_frame_head_pos_of_frame_candidate_ - 1);
      // ����́C�����p�����f�[�^�̐擪����Ăуt���[���T��
      p_stream_config->carry_over_buffer_next_pos_ = 0;
    }
    else
    {
      // �t���[���m�肵�āC��͂ł��Ȃ�������M�f�[�^������ꍇ�C����Ɉ����p��
      p_stream_config->carry_over_buffer_size_ = (uint16_t)(rec_data_len - total_processed_data_len);
      // ����́C�����p�����f�[�^�̐擪����Ăуt���[���T��
      p_stream_config->carry_over_buffer_next_pos_ = 0;
    }
  }
  else
  {
    if (p_stream_config->rec_status_.status_code = DS_STREAM_REC_STATUS_FINDING_HEADER)
    {
      // �����p���f�[�^�͂Ȃ�
      p_stream_config->carry_over_buffer_size_ = 0;
      // ����́C�擪����Ăуt���[���T��
      p_stream_config->carry_over_buffer_next_pos_ = 0;
    }
    else
    {
      // �m��t���[���̐擪�ȍ~������Ɉ����p��
      p_stream_config->carry_over_buffer_size_ = (uint16_t)(rec_data_len - p_stream_config->rx_frame_head_pos_of_frame_candidate_);
      // ����́C�����p�����f�[�^�̓X�L�b�v���C��M�������̂̐擪����t���[��
      p_stream_config->carry_over_buffer_next_pos_ = p_stream_config->carry_over_buffer_size_;
    }
  }

  if (p_stream_config->carry_over_buffer_size_ > 0 && p_stream_config->carry_over_buffer_size_ <= DS_RX_BUFFER_SIZE_MAX)
  {
    uint16_t pos = (uint16_t)(rec_data_len - p_stream_config->carry_over_buffer_size_);
    p_stream_config->is_rx_buffer_carry_over_ = 1;
    memcpy(p_stream_config->rx_buffer_for_carry_over_,
           &(rx_buffer[pos]),
           (size_t)p_stream_config->carry_over_buffer_size_);
  }
  else
  {
    // �����p���T�C�Y�� DS_RX_BUFFER_SIZE_MAX �𒴂����ꍇ�C�����̃L���p�𒴂��Ă��܂��Ă���̂ŁC���Z�b�g�D
    if (p_stream_config->carry_over_buffer_size_ > 0)
    {
      p_stream_config->rec_status_.count_of_carry_over_failures++;
    }
    p_stream_config->is_rx_buffer_carry_over_    = 0;
    p_stream_config->carry_over_buffer_size_     = 0;
    p_stream_config->carry_over_buffer_next_pos_ = 0;
  }

  return;
}


/**
 * @brief  �Œ蒷�t���[����͊֐��i�o�C�g�񏈗��j
 *
 *         ��M�o�b�t�@�̃f�[�^�𑖍����C�K�v�ȃf�[�^���t���[���Ƃ���pickup����
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer                ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  total_processed_data_len ��M�f�[�^�̃o�b�t�@�̂����C���łɏ������ꂽ�o�C�g��
 * @param  rec_data_len             ��M�f�[�^�̃o�b�t�@�̒���
 * @return ����̌Ăяo���ő��������o�C�g�����D���̑��̏ڍׂ� DS_StreamRecStatus
 */
static uint16_t DS_analyze_rx_buffer_fixed_pickup_(DS_StreamConfig* p_stream_config,
                                                   uint8_t* rx_buffer,
                                                   uint16_t total_processed_data_len,
                                                   uint16_t rec_data_len)
{
  uint16_t unprocessed_data_len = (uint16_t)(rec_data_len - total_processed_data_len);      // ���̃L���X�g�͎኱��Ȃ��i�R�[�h���_���I�ɐ�������Ζ��Ȃ����j
  DS_StreamConfig* p = p_stream_config;  // ������ƕϐ������������Ĕz��index�Ȃǂ��݂��炢�̂�...

  if (p->rx_frame_rec_len_ == 0 && p->rx_header_size_ != 0)
  {
    // �܂��w�b�_�̐擪���疢�����̏ꍇ�i�w�b�_�Ȃ����͂����̓X�L�b�v�j
    return DS_analyze_rx_buffer_finding_header_(p_stream_config,
                                                rx_buffer,
                                                total_processed_data_len,
                                                rec_data_len);
  }
  else if (p->rx_frame_rec_len_ < p->rx_header_size_)
  {
    // �w�b�_��M��
    return DS_analyze_rx_buffer_receiving_header_(p_stream_config,
                                                  rx_buffer,
                                                  total_processed_data_len);
  }
  else if (p->rx_frame_rec_len_ < p->rx_frame_size_ - p->rx_footer_size_)
  {
    // �f�[�^��M��
    // �����͍������̂��߂Ɉꊇ����
    uint16_t pickup_data_len;

    // �w�b�_�Ȃ��̏ꍇ�́C�������t���[���擪
    if (p->rx_frame_rec_len_ == 0)
    {
      p->rx_frame_head_pos_of_frame_candidate_ = total_processed_data_len;
    }

    pickup_data_len = (uint16_t)(p->rx_frame_size_ - p->rx_footer_size_ - p->rx_frame_rec_len_);

    // ����őS����M������Ȃ��ꍇ
    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    memcpy(&(p->rx_frame_[p->rx_frame_rec_len_]),
           &(rx_buffer[total_processed_data_len]),
           (size_t)pickup_data_len);

    p->rx_frame_rec_len_ += pickup_data_len;
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_DATA;

    // �t�b�^���Ȃ��Cdata��M�d�؂����ꍇ�̓t���[���m��
    // ���ꂪ�Ȃ��ƁCDS_analyze_rx_buffer_fixed_ ��
    // ���܂��Ɏ�M�����f�[�^�����҂�����t���[�����������ꍇ�ɁC�t���[���m�肪�P�����x��邱�ƂɂȂ�̂�
    if (p->rx_footer_size_ == 0 && p->rx_frame_rec_len_ == p->rx_frame_size_)
    {
      p->rec_status_.status_code = DS_STREAM_REC_STATUS_FIXED_FRAME;
      p->rec_status_.fixed_frame_len = p->rx_frame_rec_len_;
      p->rx_frame_rec_len_ = 0;
    }

    return pickup_data_len;
  }
  else
  {
    // �t�b�^��M�� or �t�b�^�Ȃ��̏ꍇ�̓t���[���m��
    return DS_analyze_rx_buffer_receiving_footer_(p_stream_config,
                                                  rx_buffer,
                                                  total_processed_data_len,
                                                  (uint16_t)(p->rx_frame_size_));
  }
}


/**
 * @brief  �σt���[����͊֐��i�o�C�g�񏈗��j
 *
 *         ��M�o�b�t�@�̃f�[�^�𑖍����C�K�v�ȃf�[�^���t���[���Ƃ���pickup����
 * @note   ��M�t���[���Ƀt���[�����f�[�^�����݂��Ă��邱�Ƃ�O��Ƃ���
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer                ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  total_processed_data_len ��M�f�[�^�̃o�b�t�@�̂����C���łɏ������ꂽ�o�C�g��
 * @param  rec_data_len             ��M�f�[�^�̃o�b�t�@�̒���
 * @return ����̌Ăяo���ő��������o�C�g�����D���̑��̏ڍׂ� DS_StreamRecStatus
 */
static uint16_t DS_analyze_rx_buffer_variable_pickup_with_rx_frame_size_(DS_StreamConfig* p_stream_config,
                                                                         uint8_t* rx_buffer,
                                                                         uint16_t total_processed_data_len,
                                                                         uint16_t rec_data_len)
{
  uint16_t unprocessed_data_len = (uint16_t)(rec_data_len - total_processed_data_len);      // ���̃L���X�g�͎኱��Ȃ��i�R�[�h���_���I�ɐ�������Ζ��Ȃ����j
  DS_StreamConfig* p = p_stream_config;  // ������ƕϐ������������Ĕz��index�Ȃǂ��݂��炢�̂�...
  uint32_t rx_frame_size = DS_analyze_rx_buffer_get_framelength_(p_stream_config);      // �܂���M���Ă��Ȃ��ꍇ�͕s��l�����邱�Ƃɗ��ӂ��邱�ƁI�I

  if (p->rx_frame_rec_len_ == 0 && p->rx_header_size_ != 0)
  {
    // �܂��w�b�_�̐擪���疢�����̏ꍇ�i�w�b�_�Ȃ����͂����̓X�L�b�v�j
    return DS_analyze_rx_buffer_finding_header_(p_stream_config,
                                                rx_buffer,
                                                total_processed_data_len,
                                                rec_data_len);
  }
  else if (p->rx_frame_rec_len_ < p->rx_header_size_)
  {
    // �w�b�_��M��
    return DS_analyze_rx_buffer_receiving_header_(p_stream_config,
                                                  rx_buffer,
                                                  total_processed_data_len);
  }
  else if (p->rx_frame_rec_len_ < p->rx_framelength_pos_ + p->rx_framelength_type_size_)
  {
    // �t���[���T�C�Y�T����
    // �����͍������̂��߂Ɉꊇ����
    uint16_t pickup_data_len;

    // �w�b�_�Ȃ��̏ꍇ�́C�������t���[���擪
    if (p->rx_frame_rec_len_ == 0)
    {
      p->rx_frame_head_pos_of_frame_candidate_ = total_processed_data_len;
    }

    pickup_data_len = (uint16_t)(p->rx_framelength_pos_ + p->rx_framelength_type_size_ - p->rx_frame_rec_len_);

    // ����őS����M������Ȃ��ꍇ
    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    memcpy(&(p->rx_frame_[p->rx_frame_rec_len_]),
           &(rx_buffer[total_processed_data_len]),
           (size_t)pickup_data_len);

    p->rx_frame_rec_len_ += pickup_data_len;
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_FRAMELENGTH;

    // �t���[��������M���I������ꍇ�C�`�F�b�N����
    if (p->rx_frame_rec_len_ >= p->rx_framelength_pos_ + p->rx_framelength_type_size_)
    {
      rx_frame_size = DS_analyze_rx_buffer_get_framelength_(p_stream_config);

      // �o�b�t�@�[�����̓G���[���o���I
      if (rx_frame_size > DS_RX_FRAME_SIZE_MAX)
      {
        p->rec_status_.status_code = DS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG;
        p->rx_frame_rec_len_ = 0;
#ifdef DS_DEBUG
        Printf("DS: RX frame size is too long\n");
#endif
        return 0;       // TODO: �����ŉ���Ԃ����͂����������l���Ă��ǂ������H
      }

      // body�T�C�Y����0�ȏ��v��
      if (rx_frame_size < p->rx_header_size_ + p->rx_footer_size_)
      {
        p->rec_status_.status_code = DS_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT;
        p->rx_frame_rec_len_ = 0;
#ifdef DS_DEBUG
        Printf("DS: RX frame size is too short\n");
#endif
        return 0;       // TODO: �����ŉ���Ԃ����͂����������l���Ă��ǂ������H
      }
    }

    return pickup_data_len;
  }
  else if (p->rx_frame_rec_len_ < rx_frame_size - p->rx_footer_size_)
  {
    // �f�[�^��M��
    // �����͍������̂��߂Ɉꊇ����

    uint16_t pickup_data_len = (uint16_t)(rx_frame_size - p->rx_footer_size_ - p->rx_frame_rec_len_);    // TODO: ���݁C�t���[������uint16_t�𒴂��邱�Ƃ͑z�肵�Ă��Ȃ��I

    // ����őS����M������Ȃ��ꍇ
    if (pickup_data_len > unprocessed_data_len)
    {
      pickup_data_len = unprocessed_data_len;
    }

    memcpy(&(p->rx_frame_[p->rx_frame_rec_len_]),
           &(rx_buffer[total_processed_data_len]),
           (size_t)pickup_data_len);

    p->rx_frame_rec_len_ += pickup_data_len;
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_DATA;

    // �t�b�^���Ȃ��Cdata��M�d�؂����ꍇ�̓t���[���m��
    // ���ꂪ�Ȃ��ƁCDS_analyze_rx_buffer_fixed_ �ō��܂��Ɏ�M�����f�[�^�����҂�����t���[�����������ꍇ�ɁC�t���[���m�肪�P�����x��邱�ƂɂȂ�̂�
    if (p->rx_footer_size_ == 0 && p->rx_frame_rec_len_ == rx_frame_size)
    {
      p->rec_status_.status_code = DS_STREAM_REC_STATUS_FIXED_FRAME;
      p->rx_frame_rec_len_ = 0;
    }

    return pickup_data_len;
  }
  else
  {
    // �t�b�^��M�� or �t�b�^�Ȃ��̏ꍇ�̓t���[���m��
    return DS_analyze_rx_buffer_receiving_footer_(p_stream_config,
                                                  rx_buffer,
                                                  total_processed_data_len,
                                                  (uint16_t)rx_frame_size);
  }
}


/**
 * @brief  �σt���[����͊֐��i�o�C�g�񏈗��j
 *
 *         ��M�o�b�t�@�̃f�[�^�𑖍����C�K�v�ȃf�[�^���t���[���Ƃ���pickup����
 * @note   DS_analyze_rx_buffer_variable_pickup_with_rx_frame_size_ �Ƃ̈Ⴂ�́C�e�������f�[�^���t���[���Ɋ܂܂�邩�ۂ�
 * @note   �t�b�^�����݂��Ă��邱�Ƃ�O��Ƃ���
 * @note   �w�b�_�Ȃ��͔F�߂�D�������C��M�f�[�^�擪����t���[���Ƃ݂Ȃ��̂ŁC�w�b�_�����������������
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer                ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  total_processed_data_len ��M�f�[�^�̃o�b�t�@�̂����C���łɏ������ꂽ�o�C�g��
 * @param  rec_data_len             ��M�f�[�^�̃o�b�t�@�̒���
 * @return ����̌Ăяo���ő��������o�C�g�����D���̑��̏ڍׂ� DS_StreamRecStatus
 */
static uint16_t DS_analyze_rx_buffer_variable_pickup_with_footer_(DS_StreamConfig* p_stream_config,
                                                                  uint8_t* rx_buffer,
                                                                  uint16_t total_processed_data_len,
                                                                  uint16_t rec_data_len)
{
  uint16_t unprocessed_data_len = (uint16_t)(rec_data_len - total_processed_data_len);      // ���̃L���X�g�͎኱��Ȃ��i�R�[�h���_���I�ɐ�������Ζ��Ȃ����j
  DS_StreamConfig* p = p_stream_config;  // ������ƕϐ������������Ĕz��index�Ȃǂ��݂��炢�̂�...

  if (p->rx_frame_rec_len_ == 0 && p->rx_header_size_ != 0)
  {
    // �܂��w�b�_�̐擪���疢�����̏ꍇ�i�w�b�_�Ȃ����͂����̓X�L�b�v�j
    return DS_analyze_rx_buffer_finding_header_(p_stream_config,
                                                rx_buffer,
                                                total_processed_data_len,
                                                rec_data_len);
  }
  else if (p->rx_frame_rec_len_ < p->rx_header_size_)
  {
    // �w�b�_��M��
    return DS_analyze_rx_buffer_receiving_header_(p_stream_config,
                                                  rx_buffer,
                                                  total_processed_data_len);
  }
  else
  {
    // �Ō�܂Ŏ�M���C�t�b�^�̍ŏI������T���D�t�b�^�Ȃ��͂��肦�Ȃ��D
    uint8_t* p_footer_last;     // inclusive
    int32_t  body_data_len;     // �T�C�Y�I�ɂ�u16�ł悢���C�������Ƃ肽���̂�i32�Ƃ��Ă���
    uint16_t processed_data_len;
    uint16_t i;
    uint16_t estimated_rx_frame_size;
    uint16_t pickup_data_len;

    // �w�b�_�Ȃ��̏ꍇ�́C�������t���[���擪
    if (p->rx_frame_rec_len_ == 0)
    {
      p->rx_frame_head_pos_of_frame_candidate_ = total_processed_data_len;
    }

    // �͂��Ă���f�[�^����M�t���[���o�b�t�@�Ɋi�[����
    // �����͍������̂��߂Ɉꊇ����
    pickup_data_len = unprocessed_data_len;
    // �i���Ƀt�b�^����M���Ȃ��ꍇ�Ƀo�b�t�@�[�I�[�o�[�������邱�Ƃ�h��
    if (p->rx_frame_rec_len_ + pickup_data_len > DS_RX_FRAME_SIZE_MAX)
    {
      if (p->rx_frame_rec_len_ >= DS_RX_FRAME_SIZE_MAX)
      {
        // ����ȏ��M�ł��Ȃ����߁C�t�b�^�T�����s�Ƃ��āC���Z�b�g����
        p->rec_status_.status_code = DS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG;
        p->rx_frame_rec_len_ = 0;
#ifdef DS_DEBUG
        Printf("DS: RX frame is too long\n");
#endif
        return 0;   // �����ς݃f�[�^���Ȃ�
      }
      pickup_data_len = (uint16_t)(DS_RX_FRAME_SIZE_MAX - p->rx_frame_rec_len_);
    }
    memcpy(&(p->rx_frame_[p->rx_frame_rec_len_]),
           &(rx_buffer[total_processed_data_len]),
           (size_t)pickup_data_len);

    // �t�b�^�ŏI������T��
    p_footer_last = (uint8_t*)memchr(&(rx_buffer[p->rx_frame_rec_len_]),
                                     (int)(p->rx_footer_[p->rx_footer_size_ - 1]),
                                     (size_t)pickup_data_len);

    if (p_footer_last == NULL)
    {
      // �܂��܂���M����
      p->rx_frame_rec_len_ += pickup_data_len;
      p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_DATA;
      return pickup_data_len;
    }

    processed_data_len = (uint16_t)(p_footer_last - &(rx_buffer[p->rx_frame_rec_len_]) + 1);
    body_data_len = (p_footer_last - rx_buffer + 1) - p->rx_header_size_ - p->rx_footer_size_;
    if (body_data_len < 0)
    {
      // ����̓t�b�^�ł͂Ȃ��̂Ŏ�M���s
      // �܂��܂���M����
      p->rx_frame_rec_len_ += pickup_data_len;
      p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_DATA;
      return pickup_data_len;
    }

    // �t�b�^��┭��
    // �t�b�^�`�F�b�N����
    estimated_rx_frame_size = (uint16_t)(p->rx_header_size_ + body_data_len + p->rx_footer_size_);
    for (i = 0; i < p->rx_footer_size_; i++)
    {
      if (rx_buffer[estimated_rx_frame_size - i - 1] != p->rx_footer_[p->rx_footer_size_ - i - 1])
      {
        // ����̓t�b�^�ł͂Ȃ��̂Ŏ�M���s
        // �܂��܂���M����
        p->rx_frame_rec_len_ += pickup_data_len;
        p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_DATA;
        return pickup_data_len;
      }
    }

    // �t�b�^�m�� �� �t���[���m��
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_FIXED_FRAME;
    p->rec_status_.fixed_frame_len = estimated_rx_frame_size;
    p->rx_frame_rec_len_ = 0;
    return processed_data_len;
  }
}


/**
 * @brief  �t���[����͊֐��i�w�b�_�T���j
 * @note   �w�b�_�����������ꍇ�C�ŏ���1 byte�̂ݏ�������
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer                ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  total_processed_data_len ��M�f�[�^�̃o�b�t�@�̂����C���łɏ������ꂽ�o�C�g��
 * @param  rec_data_len             ��M�f�[�^�̃o�b�t�@�̒���
 * @return ����̌Ăяo���ő��������o�C�g�����D���̑��̏ڍׂ� DS_StreamRecStatus
 */
static uint16_t DS_analyze_rx_buffer_finding_header_(DS_StreamConfig* p_stream_config,
                                                     uint8_t* rx_buffer,
                                                     uint16_t total_processed_data_len,
                                                     uint16_t rec_data_len)
{
  uint16_t unprocessed_data_len = (uint16_t)(rec_data_len - total_processed_data_len);      // ���̃L���X�g�͎኱��Ȃ��i�R�[�h���_���I�ɐ�������Ζ��Ȃ����j
  DS_StreamConfig* p = p_stream_config;  // ������ƕϐ������������Ĕz��index�Ȃǂ��݂��炢�̂�...
  uint8_t* p_header;
  uint16_t processed_data_len;

  // �R�[�h����������΁C�w�b�_���Ȃ��P�[�X�͂����ɓ��B�����Ȃ����C�k���|���������Ă���
  if (p_stream_config->rx_header_ == NULL)
  {
#ifdef DS_DEBUG
    Printf("DS: RX header is invalid\n");
#endif
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_OTHER_ERR;
    return unprocessed_data_len;
  }

  // �܂��w�b�_�̐擪���疢�����̏ꍇ
  // �����͍������̂��߂Ɉꊇ����
  p_header = (uint8_t*)memchr(&(rx_buffer[total_processed_data_len]),
                              (int)(p->rx_header_[0]),
                              (size_t)unprocessed_data_len);

  if (p_header == NULL)
  {
#ifdef DS_DEBUG
    Printf("DS: RX header not found\n");
#endif
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_FINDING_HEADER;
    return unprocessed_data_len;
  }

  processed_data_len = (uint16_t)(p_header - &(rx_buffer[total_processed_data_len]) + 1);

  // �w�b�_�R�s�[�D�z���g��buffer����R�s��ׂ������ǁC������ƃA�h���X�������Ă��ĕ|���̂ŁD�D�D
  p->rx_frame_[p->rx_frame_rec_len_] = p->rx_header_[0];
  p->rx_frame_rec_len_++;

  p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_HEADER;
  p->rx_frame_head_pos_of_frame_candidate_ = (uint16_t)(total_processed_data_len + processed_data_len - 1);
  return processed_data_len;
}


/**
 * @brief  �t���[����͊֐��i�w�b�_��M���j
 * @note   1 byte�̂ݏ�������
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer                ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  total_processed_data_len ��M�f�[�^�̃o�b�t�@�̂����C���łɏ������ꂽ�o�C�g��
 * @return ����̌Ăяo���ő��������o�C�g�����i�܂��C1�Ȃ񂾂��ǁj�D���̑��̏ڍׂ� DS_StreamRecStatus
 */
static uint16_t DS_analyze_rx_buffer_receiving_header_(DS_StreamConfig* p_stream_config,
                                                       uint8_t* rx_buffer,
                                                       uint16_t total_processed_data_len)
{
  DS_StreamConfig* p = p_stream_config;  // ������ƕϐ������������Ĕz��index�Ȃǂ��݂��炢�̂�...

  // �w�b�_��M��
  // ��M���א؂�̂Ƃ��Ȃǂ̏������򂪂߂�ǂ������̂ŁC1byte������������

  // �w�b�_�����������H
  if (rx_buffer[total_processed_data_len] == p->rx_header_[p->rx_frame_rec_len_])
  {
    p->rx_frame_[p->rx_frame_rec_len_] = p->rx_header_[p->rx_frame_rec_len_];
    p->rx_frame_rec_len_++;

    p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_HEADER;
    return 1;
  }
  else
  {
    // �w�b�_���������Ȃ�����
    // DS_STREAM_REC_STATUS_HEADER_MISMATCH �ɂȂ�C�Ăуo�b�t�@�������߂��ăw�b�_�T�����n�߂�
    // ���̌� DS_STREAM_REC_STATUS_FINDING_HEADER �ɖ߂�
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_HEADER_MISMATCH;
    p->rx_frame_rec_len_ = 0;
#ifdef DS_DEBUG
    Printf("DS: RX header is mismatch\n");
#endif
    return 1;
  }
}


/**
 * @brief  �t���[����͊֐��i�t�b�^��M���j
 * @note   1 byte�̂ݏ�������
 * @note   ���݁C�t���[������uint16_t�𒴂��邱�Ƃ͑z�肵�Ă��Ȃ��I
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @param  rx_buffer                ��M�f�[�^�̃o�b�t�@�i�z��j
 * @param  total_processed_data_len ��M�f�[�^�̃o�b�t�@�̂����C���łɏ������ꂽ�o�C�g��
 * @param  rx_frame_size            �t���[���T�C�Y�i�ϒ��t���[���̏ꍇ������̂ŁC�����Ɏ��j
 * @return ����̌Ăяo���ő��������o�C�g�����i�܂��C1�Ȃ񂾂��ǁj�D���̑��̏ڍׂ� DS_StreamRecStatus
 */
static uint16_t DS_analyze_rx_buffer_receiving_footer_(DS_StreamConfig* p_stream_config,
                                                       uint8_t* rx_buffer,
                                                       uint16_t total_processed_data_len,
                                                       uint16_t rx_frame_size)
{
  DS_StreamConfig* p = p_stream_config;  // ������ƕϐ������������Ĕz��index�Ȃǂ��݂��炢�̂�...
  uint16_t rec_footer_pos;

  // �t�b�^�Ȃ��̏ꍇ�̓t���[���m��
  if (p->rx_footer_size_ == 0)
  {
    // �t�b�^�Ȃ�
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_FIXED_FRAME;
    p->rec_status_.fixed_frame_len = p->rx_frame_rec_len_;
    p->rx_frame_rec_len_ = 0;
    return 0;   // �����ς݃f�[�^���Ȃ�
  }

  // �t�b�^��M
  // �������������򂪂߂�ǂ������̂ŁC1byte����������
  rec_footer_pos = (uint16_t)(p->rx_frame_rec_len_ - (rx_frame_size - p->rx_footer_size_));

  // ���҂���Ă���t�b�^����M�ł������H
  // ��M�ł��Ȃ������ꍇ�C DS_STREAM_REC_STATUS_FOOTER_MISMATCH �ɂȂ�C�Ăуo�b�t�@�������߂��ăw�b�_�T�����n�߂�
  // ���̌� DS_STREAM_REC_STATUS_FINDING_HEADER �ɖ߂�
  if (rx_buffer[total_processed_data_len] != p->rx_footer_[rec_footer_pos])
  {
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_FOOTER_MISMATCH;
    p->rx_frame_rec_len_ = 0;
#ifdef DS_DEBUG
    Printf("DS: RX footer is mismatch\n");
#endif
    return 1;
  }

  // �����܂ł����琳�����t�b�^����M����Ă���
  p->rx_frame_[p->rx_frame_rec_len_] = p->rx_footer_[rec_footer_pos];
  p->rx_frame_rec_len_++;

  if (p->rx_frame_rec_len_ == rx_frame_size)
  {
    // �t���[���m��
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_FIXED_FRAME;
    p->rec_status_.fixed_frame_len = p->rx_frame_rec_len_;
    p->rx_frame_rec_len_ = 0;
  }
  else
  {
    // �t���[���m�肹��
    p->rec_status_.status_code = DS_STREAM_REC_STATUS_RECEIVING_FOOTER;
  }

  return 1;
}


/**
 * @brief  �t���[����͒��Ɏ�M�����t���[������t���[�������擾����֐�
 * @note   DS_analyze_rx_buffer_variable_pickup_with_rx_frame_size_ ����Ă΂�邱�Ƃ�z��
 * @param  p_stream_config          DriverSuper�\���̂�DS_StreamConfig
 * @return �t���[����
 */
static uint32_t DS_analyze_rx_buffer_get_framelength_(DS_StreamConfig* p_stream_config)
{
  uint32_t len = 0;
  uint8_t  i;

  for (i = 0; i < p_stream_config->rx_framelength_type_size_; ++i)
  {
    if (i == 0)
    {
      len = p_stream_config->rx_frame_[p_stream_config->rx_framelength_pos_];
    }
    else
    {
      len <<= 8;
      len |= p_stream_config->rx_frame_[p_stream_config->rx_framelength_pos_ + i];
    }
  }

  len += p_stream_config->rx_framelength_offset_;
  return len;
}


/**
 * @brief  DS_StreamConfig�\���̂̏�����
 *
 *         DS_StreamConfig�\���̂����������C�f�t�H���g�l�Ŗ��߂�D
 * @param  p_stream_config  DriverSuper�\���̂�DS_StreamConfig
 * @return DS_ERR_CODE
 */
static DS_ERR_CODE DS_reset_stream_config_(DS_StreamConfig* p_stream_config)
{
  p_stream_config->is_enabled_ = 0;
  p_stream_config->is_strict_frame_search_ = 0;

  p_stream_config->general_cmd_tx_count_               = 0;
  p_stream_config->req_tlm_cmd_tx_count_               = 0;
  p_stream_config->req_tlm_cmd_tx_count_after_last_tx_ = 0;
  p_stream_config->rx_frame_fix_count_                 = 0;

  p_stream_config->general_cmd_tx_time_ = TMGR_get_master_clock();
  p_stream_config->req_tlm_cmd_tx_time_ = TMGR_get_master_clock();
  p_stream_config->rx_frame_fix_time_   = TMGR_get_master_clock();

  p_stream_config->tx_frame_       = NULL;
  p_stream_config->tx_frame_size_  = 0;
  p_stream_config->rx_header_      = NULL;
  p_stream_config->rx_header_size_ = 0;
  p_stream_config->rx_footer_      = NULL;
  p_stream_config->rx_footer_size_ = 0;
  p_stream_config->rx_frame_size_  = 0;

  p_stream_config->rx_framelength_pos_       = -1;
  p_stream_config->rx_framelength_type_size_ = 0;
  p_stream_config->rx_framelength_offset_    = 0;

  p_stream_config->data_analyzer_ = DS_data_analyzer_dummy_;
  p_stream_config->ret_from_data_analyzer_ = DS_ERR_CODE_OK;

  p_stream_config->should_monitor_for_tlm_disruption_ = 0;
  p_stream_config->time_threshold_for_tlm_disruption_ = 60 * 1000;      // ���̒l�͂悭�l���邱��

  p_stream_config->is_validation_needed_for_send_ = 0;
  p_stream_config->is_validation_needed_for_rec_  = 0;

  p_stream_config->rx_frame_rec_len_ = 0;
  p_stream_config->rx_frame_head_pos_of_frame_candidate_ = 0;

  memset(p_stream_config->rx_frame_,
         0x00,
         sizeof(p_stream_config->rx_frame_));

  p_stream_config->is_rx_buffer_carry_over_ = 0;
  p_stream_config->carry_over_buffer_size_ = 0;
  p_stream_config->carry_over_buffer_next_pos_ = 0;
  memset(p_stream_config->rx_buffer_for_carry_over_,
         0x00,
         sizeof(p_stream_config->rx_buffer_for_carry_over_));

  // DS_StreamSendStatus �̏�����
  p_stream_config->send_status_.status_code    = DS_STREAM_SEND_STATUS_DISABLE;
  p_stream_config->send_status_.ret_from_if_tx = 0;

  // DS_StreamRecStatus �̏�����
  p_stream_config->rec_status_.status_code                  = DS_STREAM_REC_STATUS_DISABLE;
  p_stream_config->rec_status_.fixed_frame_len              = 0;
  p_stream_config->rec_status_.tlm_disruption_status        = DS_STREAM_TLM_DISRUPTION_STATUS_OK;
  p_stream_config->rec_status_.count_of_carry_over_failures = 0;

  return DS_ERR_CODE_OK;
}


static DS_ERR_CODE DS_validate_stream_config_(DS_StreamConfig* p_stream_config)
{
  if (!p_stream_config->is_enabled_) return DS_ERR_CODE_OK;

  if (p_stream_config->tx_frame_size_  != 0 && p_stream_config->tx_frame_  == NULL) return DS_ERR_CODE_ERR;
  if (p_stream_config->rx_header_size_ != 0 && p_stream_config->rx_header_ == NULL) return DS_ERR_CODE_ERR;
  if (p_stream_config->rx_footer_size_ != 0 && p_stream_config->rx_footer_ == NULL) return DS_ERR_CODE_ERR;

  if (p_stream_config->rx_frame_size_ > DS_RX_FRAME_SIZE_MAX) return DS_ERR_CODE_ERR;   // [TODO] ���݂�BigData�������i�ڍׂ̓w�b�_�t�@�C���Q�Ɓj�̂��߁C�����Œe��

  if (p_stream_config->rx_frame_size_ < 0)
  {
    // �e�����g���ϒ�
    if (p_stream_config->rx_framelength_pos_ < 0)
    {
      // �t���[���T�C�Y�f�[�^���Ȃ��ꍇ
      // �t�b�^�̑��݂��K�{
      if (p_stream_config->rx_footer_size_ == 0) return DS_ERR_CODE_ERR;
    }
    else
    {
      if (p_stream_config->rx_header_size_ == 0) return DS_ERR_CODE_ERR;       // �ϒ����w�b�_�Ȃ��͑Ή����Ȃ��i�Œ蒷�̂悤�ɂ��ĉ������D�ڍׂ̓w�b�_�t�@�C���Q�Ɓj
      if (p_stream_config->rx_framelength_pos_ < p_stream_config->rx_header_size_) return DS_ERR_CODE_ERR;    // �t���[���T�C�Y���w�b�_�i�܂�Œ�l�j�Ɋ܂܂�邱�Ƃ͂��肦�Ȃ��̂�
      if (!(p_stream_config->rx_framelength_type_size_ == 1 ||
            p_stream_config->rx_framelength_type_size_ == 2 ||
            p_stream_config->rx_framelength_type_size_ == 3 ||
            p_stream_config->rx_framelength_type_size_ == 4 )) return DS_ERR_CODE_ERR;    // ���݂�uint8 to uint32�̂ݑΉ�
    }
  }
  else if (p_stream_config->rx_frame_size_ == 0)
  {
    // �e�����Ȃ�
  }
  else
  {
    // �e�����g���Œ蒷
    if (p_stream_config->rx_frame_size_ < (p_stream_config->rx_header_size_ + p_stream_config->rx_footer_size_)) return DS_ERR_CODE_ERR;
  }

  if (p_stream_config->is_strict_frame_search_)
  {
    // �w�b�_�����邱�Ƃ��O��
    if (p_stream_config->rx_header_size_ == 0) return DS_ERR_CODE_ERR;
  }

  p_stream_config->is_validation_needed_for_send_ = 0;
  p_stream_config->is_validation_needed_for_rec_ = 0;
  return DS_ERR_CODE_OK;
}


static DS_ERR_CODE DS_load_init_setting_dummy_(DriverSuper* p_super)
{
  (void)p_super;
  return DS_ERR_CODE_OK;
}

static DS_ERR_CODE DS_data_analyzer_dummy_(DS_StreamConfig* p_stream_config, void* p_driver)
{
  (void)p_stream_config;
  (void)p_driver;
  return DS_ERR_CODE_OK;
}


// ###### DS_Config Getter/Setter ######
const DS_RecStatus* DSC_get_rec_status(const DriverSuper* p_super)
{
  return &p_super->config.rec_status_;
}

const uint32_t DSC_get_rx_count(const DriverSuper* p_super)
{
  return p_super->config.rx_count_;
}

const uint32_t DSC_get_rx_call_count(const DriverSuper* p_super)
{
  return p_super->config.rx_call_count_;
}

const ObcTime* DSC_get_rx_time(const DriverSuper* p_super)
{
  return &p_super->config.rx_time_;
}

const uint8_t DSC_get_should_monitor_for_rx_disruption(const DriverSuper* p_super)
{
  return p_super->config.should_monitor_for_rx_disruption_;
}

void DSC_enable_monitor_for_rx_disruption(DriverSuper* p_super)
{
  p_super->config.should_monitor_for_rx_disruption_ = 1;
}

void DSC_disable_monitor_for_rx_disruption(DriverSuper* p_super)
{
  p_super->config.should_monitor_for_rx_disruption_ = 0;
}

const uint32_t DSC_get_time_threshold_for_rx_disruption(const DriverSuper* p_super)
{
  return p_super->config.time_threshold_for_rx_disruption_;
}

void DSC_set_time_threshold_for_rx_disruption(DriverSuper* p_super,
                                              const uint32_t time_threshold_for_rx_disruption)
{
  p_super->config.time_threshold_for_rx_disruption_ = time_threshold_for_rx_disruption;
}

const DS_RX_DISRUPTION_STATUS_CODE DSC_get_rx_disruption_status(const DriverSuper* p_super)
{
  return p_super->config.rec_status_.rx_disruption_status;
}

// ###### DS_StreamConfig Getter/Setter ######
const uint8_t DSSC_get_is_enable(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->is_enabled_;
}

void DSSC_enable(DS_StreamConfig* p_stream_config)
{
  p_stream_config->is_enabled_ = 1;
  p_stream_config->is_validation_needed_for_send_ = 1;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

void DSSC_disable(DS_StreamConfig* p_stream_config)
{
  p_stream_config->is_enabled_ = 0;
}

const uint8_t DSSC_get_is_strict_frame_search(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->is_strict_frame_search_;
}

void DSSC_enable_strict_frame_search(DS_StreamConfig* p_stream_config)
{
  p_stream_config->is_strict_frame_search_ = 1;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

void DSSC_disable_strict_frame_search(DS_StreamConfig* p_stream_config)
{
  p_stream_config->is_strict_frame_search_ = 0;
}

const DS_StreamSendStatus* DSSC_get_send_status(const DS_StreamConfig* p_stream_config)
{
  return &p_stream_config->send_status_;
}

const DS_StreamRecStatus* DSSC_get_rec_status(const DS_StreamConfig* p_stream_config)
{
  return &p_stream_config->rec_status_;
}

const uint32_t DSSC_get_general_cmd_tx_count(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->general_cmd_tx_count_;
}

const uint32_t DSSC_get_req_tlm_cmd_tx_count(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->req_tlm_cmd_tx_count_;
}

const uint32_t DSSC_get_req_tlm_cmd_tx_count_after_last_tx(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->req_tlm_cmd_tx_count_after_last_tx_;
}

const uint32_t DSSC_get_rx_frame_fix_count(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->rx_frame_fix_count_;
}

const ObcTime* DSSC_get_general_cmd_tx_time(const DS_StreamConfig* p_stream_config)
{
  return &p_stream_config->general_cmd_tx_time_;
}

const ObcTime* DSSC_get_req_tlm_cmd_tx_time(const DS_StreamConfig* p_stream_config)
{
  return &p_stream_config->req_tlm_cmd_tx_time_;
}

const ObcTime* DSSC_get_rx_frame_fix_time(const DS_StreamConfig* p_stream_config)
{
  return &p_stream_config->rx_frame_fix_time_;
}

void DSSC_set_tx_frame(DS_StreamConfig* p_stream_config,
                       uint8_t* tx_frame)
{
  p_stream_config->tx_frame_ = tx_frame;
  p_stream_config->is_validation_needed_for_send_ = 1;
}

void DSSC_set_tx_frame_size(DS_StreamConfig* p_stream_config,
                            const uint16_t tx_frame_size)
{
  p_stream_config->tx_frame_size_ = tx_frame_size;
  p_stream_config->is_validation_needed_for_send_ = 1;
}

const uint16_t DSSC_get_tx_frame_size(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->tx_frame_size_;
}

const uint8_t* DSSC_get_rx_frame(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->rx_frame_;
}

void DSSC_set_rx_header(DS_StreamConfig* p_stream_config,
                        const uint8_t* rx_header,
                        const uint16_t rx_header_size)
{
  p_stream_config->rx_header_ = rx_header;
  p_stream_config->rx_header_size_ = rx_header_size;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

void DSSC_set_rx_footer(DS_StreamConfig* p_stream_config,
                        const uint8_t* rx_footer,
                        const uint16_t rx_footer_size)
{
  p_stream_config->rx_footer_ = rx_footer;
  p_stream_config->rx_footer_size_ = rx_footer_size;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

void DSSC_set_rx_frame_size(DS_StreamConfig* p_stream_config,
                            const int16_t rx_frame_size)
{
  p_stream_config->rx_frame_size_ = rx_frame_size;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

const uint16_t DSSC_get_rx_header_size(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->rx_header_size_;
}

const uint16_t DSSC_get_rx_footer_size(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->rx_footer_size_;
}

const int16_t DSSC_get_rx_frame_size(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->rx_frame_size_;
}

void DSSC_set_rx_framelength_pos(DS_StreamConfig* p_stream_config,
                                 const int16_t rx_framelength_pos)
{
  p_stream_config->rx_framelength_pos_ = rx_framelength_pos;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

void DSSC_set_rx_framelength_type_size(DS_StreamConfig* p_stream_config,
                                       const uint16_t rx_framelength_type_size)
{
  p_stream_config->rx_framelength_type_size_ = rx_framelength_type_size;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

void DSSC_set_rx_framelength_offset(DS_StreamConfig* p_stream_config,
                                    const uint16_t rx_framelength_offset)
{
  p_stream_config->rx_framelength_offset_ = rx_framelength_offset;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

const uint8_t DSSC_get_should_monitor_for_tlm_disruption(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->should_monitor_for_tlm_disruption_;
}

void DSSC_enable_monitor_for_tlm_disruption(DS_StreamConfig* p_stream_config)
{
  p_stream_config->should_monitor_for_tlm_disruption_ = 1;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

void DSSC_disable_monitor_for_tlm_disruption(DS_StreamConfig* p_stream_config)
{
  p_stream_config->should_monitor_for_tlm_disruption_ = 0;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

const uint32_t DSSC_get_time_threshold_for_tlm_disruption(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->time_threshold_for_tlm_disruption_;
}

void DSSC_set_time_threshold_for_tlm_disruption(DS_StreamConfig* p_stream_config,
                                                const uint32_t time_threshold_for_tlm_disruption)
{
  p_stream_config->time_threshold_for_tlm_disruption_ = time_threshold_for_tlm_disruption;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

const DS_STREAM_TLM_DISRUPTION_STATUS_CODE DSSC_get_tlm_disruption_status(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->rec_status_.tlm_disruption_status;
}

void DSSC_set_data_analyzer(DS_StreamConfig* p_stream_config,
                            DS_ERR_CODE (*data_analyzer)(DS_StreamConfig* p_stream_config, void* p_driver))
{
  p_stream_config->data_analyzer_ = data_analyzer;
  p_stream_config->is_validation_needed_for_rec_ = 1;
}

const DS_ERR_CODE DSSC_get_ret_from_data_analyzer(const DS_StreamConfig* p_stream_config)
{
  return p_stream_config->ret_from_data_analyzer_;
}


// ###### Driver�ėpUtil�֐� ######

CCP_EXEC_STS DS_conv_driver_err_to_ccp_exec_sts(DS_DRIVER_ERR_CODE code)
{
  switch (code)
  {
  case DS_DRIVER_ERR_CODE_ILLEGAL_CONTEXT:
  case DS_DRIVER_ERR_CODE_UNKNOWN_ERR:
    // �S�Ă���ł����̂��́C�v����
    return CCP_EXEC_ILLEGAL_CONTEXT;
  case DS_DRIVER_ERR_CODE_ILLEGAL_PARAMETER:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case DS_DRIVER_ERR_CODE_ILLEGAL_LENGTH:
    return CCP_EXEC_ILLEGAL_LENGTH;
  default:
    // �����ɗ���͈̂ȉ�
    // DS_DRIVER_ERR_CODE_OK
    return CCP_EXEC_SUCCESS;
  }
}


CCP_EXEC_STS DS_conv_cmd_err_to_ccp_exec_sts(DS_CMD_ERR_CODE code)
{
  switch (code)
  {
  case DS_CMD_ILLEGAL_CONTEXT:
    return CCP_EXEC_ILLEGAL_CONTEXT;
  case DS_CMD_ILLEGAL_PARAMETER:
    return CCP_EXEC_ILLEGAL_PARAMETER;
  case DS_CMD_ILLEGAL_LENGTH:
    return CCP_EXEC_ILLEGAL_LENGTH;
  default:
    // �����ɗ���͈̂ȉ��̂R��
    // DS_CMD_OK
    // DS_CMD_DRIVER_SUPER_ERR
    // DS_CMD_UNKNOWN_ERR
    // ���Q�̃G���[��Driver���̖��ŁC������ŃG���[�������ׂ�
    // �����ł� SUCCESS��Ԃ�
    return CCP_EXEC_SUCCESS;
  }
}

#pragma section
