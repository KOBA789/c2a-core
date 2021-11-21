/**
 * @file   driver_super.h
 * @brief  �e����Z���T�E�A�N�`���G�[�^���Ƃ̃C���^�[�t�F�[�X�h���C�o�Q�̃X�[�p�[�N���X
 *
 *         DriverSuper Class ��
 *         �e����Z���T�E�A�N�`���G�[�^���Ƃ̃C���^�[�t�F�[�X���������C
 *         �������C�R�}���h���s�C�e�����g�����N�G�X�g�C�e�����g����M�C�e�����g����͂Ȃǂ��s���C�h���C�o�Q�̃X�[�p�[�N���X�ł��D
 *         �X�̋@��̃C���^�[�t�F�[�X�h���C�o�Ɍp�������Ďg�p���܂��D
 */
#ifndef DRIVER_SUPER_H_
#define DRIVER_SUPER_H_

#include <src_user/IfWrapper/if_list.h>
#include <src_user/Library/stdint.h>
#include "../../Library/endian_memcpy.h"        // �p�X���s��Ȏ��������R�[�h�ނŎg����悤��
#include "../../System/TimeManager/time_manager.h"
// #include <src_user/CmdTlm/CCSDS/TCPacket.h>


#define DS_STREAM_MAX          (3)         /*!< DS_StreamConfig�̍ő吔
                                                uint8_t ��z��          */
#define DS_RX_BUFFER_SIZE_MAX  (1024)      //!< ��M�f�[�^�o�b�t�@�̍ő咷
#define DS_RX_FRAME_SIZE_MAX   (1024)      //!< ��M�f�[�^�t���[���̍ő咷

#include <src_user/Settings/DriverSuper/driver_super_params.h>

/**
 * @enum   DS_DRIVER_ERR_CODE
 * @brief  Driver �̔ėp�G���[�R�[�h
 *
 *         Driver ���̊e��setter ���̊֐��Ԃ�l�Ŏg���邱�Ƃ�z��
 * @note   uint8_t ��z��
 * @note   �������֐��Ăяo�����ɂ��ẮC DS_INIT_ERR_CODE ��p���邱��
 * @note   ��M�֐��Ăяo�����ɂ��ẮC DS_REC_ERR_CODE ��p���邱��
 * @note   �ڑ���@��֑���Cmd�Ăяo�����ɂ��ẮC DS_CMD_ERR_CODE ��p���邱��
 * @note   DI �� Cmd �̕Ԃ�l�ł��� CCP_EXEC_STS �Ƃ̐������𑽏��ӎ����Ă���
 * @note   CCP_EXEC_STS �ւ̕ϊ��� DS_conv_driver_err_to_ccp_exec_sts ��p����
 * @note   ���ڍׂȃG���[����Ԃ������ꍇ�́C Driver ���ƂɓƎ� enum ���`���ėǂ�
 */
typedef enum
{
  DS_DRIVER_ERR_CODE_OK = 0,                //!< OK
  DS_DRIVER_ERR_CODE_ILLEGAL_CONTEXT,       //!< CCP_EXEC_ILLEGAL_CONTEXT �ɑΉ�
  DS_DRIVER_ERR_CODE_ILLEGAL_PARAMETER,     //!< CCP_EXEC_ILLEGAL_PARAMETER �ɑΉ�
  DS_DRIVER_ERR_CODE_ILLEGAL_LENGTH,        //!< CCP_EXEC_ILLEGAL_PARAMETER �ɑΉ�
  DS_DRIVER_ERR_CODE_UNKNOWN_ERR = 255      //!< UNKNOWN ERR
} DS_DRIVER_ERR_CODE;


/**
 * @enum   DS_INIT_ERR_CODE
 * @brief  Driver �̏������֐��̃G���[�R�[�h
 * @note   uint8_t ��z��
 */
typedef enum
{
  DS_INIT_OK = 0,               //!< OK
  DS_INIT_DS_INIT_ERR,          //!< DS_init �ł̃G���[
  DS_INIT_PARAMETER_ERR,        //!< �������p�����^�G���[
  DS_INIT_OTHER_ERR,            //!< ���̑��̃G���[
  DS_INIT_UNKNOWN_ERR = 255     //!< UNKNOWN ERR
} DS_INIT_ERR_CODE;


/**
 * @enum   DS_REC_ERR_CODE
 * @brief  Driver �̎�M�֐��̃G���[�R�[�h
 * @note   uint8_t ��z��
 */
typedef enum
{
  DS_REC_OK = 0,                //!< OK
  DS_REC_DS_RECEIVE_ERR,        //!< DS_receive �ł̃G���[
  DS_REC_ANALYZE_ERR,           //!< DS_analyze_rec_data �ł̃G���[
  DS_REC_OTHER_ERR,             //!< ���̑��̃G���[
  DS_REC_UNKNOWN_ERR = 255      //!< UNKNOWN ERR
} DS_REC_ERR_CODE;


/**
 * @enum   DS_CMD_ERR_CODE
 * @brief  �eDI�� Driver �ɃR�}���h�𑗂�Ƃ��ɁC����I�Ɏg���R�[�h
 * @note   uint8_t ��z��
 * @note   DI �� Cmd �̕Ԃ�l�ł��� CCP_EXEC_STS �Ƃ̐������𑽏��ӎ����Ă���
 * @note   CCP_EXEC_STS �ւ̕ϊ��� DS_conv_cmd_err_to_ccp_exec_sts ��p����
 */
typedef enum
{
  DS_CMD_OK                 = 0,   //!< OK��0�ł��邱�Ƃ�ۏ؂���
  DS_CMD_ILLEGAL_CONTEXT    = 1,   //!< CCP_EXEC_ILLEGAL_CONTEXT �ɑΉ��DDI�ł݂邱�Ƃ������͂��DHW�ˑ�����Driver�ł݂�
  DS_CMD_ILLEGAL_PARAMETER  = 2,   //!< CCP_EXEC_ILLEGAL_PARAMETER �ɑΉ��D�q�[�^�[�̌��ȂǁCHW�ˑ�����Drivre�ł݂�
  DS_CMD_ILLEGAL_LENGTH     = 3,   //!< CCP_EXEC_ILLEGAL_LENGTH �ɑΉ��D����͊�{�I�ɂ�DI�Ō���͂��Ȃ̂ŁC�g���Ȃ����Ƃ�z��
  DS_CMD_DRIVER_SUPER_ERR   = 4,   //!< DriverSuper���C�܂�z���̒჌�C���[�ŃG���[���N�����ꍇ
  DS_CMD_UNKNOWN_ERR        = 255
} DS_CMD_ERR_CODE;


/**
 * @enum   DS_ERR_CODE
 * @brief  DriverSuper �̔ėp�G���[�R�[�h
 * @note   uint8_t ��z��
 * @note   DriverSuper �ŗp���C Drive �ł͗p���Ȃ�
 */
typedef enum
{
  DS_ERR_CODE_OK   = 0,     //!< �Ԃ�l�͌p�����IF��ŏ㏑������邽�߁COK��0�ł��邱�Ƃ��K�{
  DS_ERR_CODE_ERR  = 1
} DS_ERR_CODE;


/**
 * @enum   DS_RX_DISRUPTION_STATUS_CODE
 * @brief  ��M�r�┻��R�[�h
 * @note   uint8_t ��z��
 */
typedef enum
{
  DS_RX_DISRUPTION_STATUS_OK   = 0,  //!< ��M�r�₵�Ă��Ȃ� or ��M�r�┻������Ȃ����
  DS_RX_DISRUPTION_STATUS_LOST = 1   //!< ��M�r�⌟�o
} DS_RX_DISRUPTION_STATUS_CODE;


/**
 * @struct DS_RecStatus
 * @brief  IF ��M��
 */
typedef struct
{
  int                          ret_from_if_rx;       //!< IF_RX �̕Ԃ�l
  DS_RX_DISRUPTION_STATUS_CODE rx_disruption_status; //!< ��M�r�┻��
} DS_RecStatus;


/**
 * @enum   DS_STREAM_SEND_STATUS_CODE
 * @brief  DS_StreamSendStatus �ł����D�R�}���h��M�̏�Ԃ�����
 * @note   uint8_t ��z��
 */
typedef enum
{
  DS_STREAM_SEND_STATUS_OK,
  DS_STREAM_SEND_STATUS_DISABLE,
  DS_STREAM_SEND_STATUS_TX_ERR,                //!< IF_TX�ŃG���[
  DS_STREAM_SEND_STATUS_VALIDATE_ERR,          //!< ���M�O��validate_config�ŃG���[
  DS_STREAM_SEND_STATUS_OTHER_ERR
} DS_STREAM_SEND_STATUS_CODE;


/**
 * @struct DS_StreamSendStatus
 * @brief  �t���[�����M��
 */
typedef struct
{
  DS_STREAM_SEND_STATUS_CODE status_code;       //!< status
  int                        ret_from_if_tx;    //!< IF_TX �̕Ԃ�l
  // ����ڍ׏����g������Ȃ�C�����ɓ����
} DS_StreamSendStatus;


/**
 * @enum   DS_STREAM_REC_STATUS_CODE
 * @brief  DS_StreamRecStatus �ł����D�e������M�̏�ԑJ�ڂ�����
 * @note   uint8_t��z��
 */
typedef enum
{
  DS_STREAM_REC_STATUS_FINDING_HEADER,
  DS_STREAM_REC_STATUS_RECEIVING_HEADER,
  DS_STREAM_REC_STATUS_RECEIVING_FRAMELENGTH, //!< �ϒ��t���[���ł́C�t���[�����f�[�^����M��
  DS_STREAM_REC_STATUS_RECEIVING_DATA,
  DS_STREAM_REC_STATUS_RECEIVING_FOOTER,
  DS_STREAM_REC_STATUS_FIXED_FRAME,
  DS_STREAM_REC_STATUS_DISABLE,
  DS_STREAM_REC_STATUS_HEADER_MISMATCH,
  DS_STREAM_REC_STATUS_FOOTER_MISMATCH,
  DS_STREAM_REC_STATUS_RX_FRAME_TOO_LONG,
  DS_STREAM_REC_STATUS_RX_FRAME_TOO_SHORT,
  DS_STREAM_REC_STATUS_RX_ERR,                //!< IF_RX �ŃG���[
  DS_STREAM_REC_STATUS_VALIDATE_ERR,          //!< ��M�O��validate_config�ŃG���[
  DS_STREAM_REC_STATUS_OTHER_ERR
} DS_STREAM_REC_STATUS_CODE;


/**
 * @enum   DS_STREAM_RX_DISRUPTION_STATUS_CODE
 * @brief  �e�����r�┻��R�[�h
 * @note   uint8_t ��z��
 */
typedef enum
{
  DS_STREAM_TLM_DISRUPTION_STATUS_OK   = 0,  //!< �e�����r�₵�Ă��Ȃ� or �e�����r�┻������Ȃ����
  DS_STREAM_TLM_DISRUPTION_STATUS_LOST = 1   //!< �e�����r�⌟�o
} DS_STREAM_TLM_DISRUPTION_STATUS_CODE;


/**
 * @struct DS_StreamRecStatus
 * @brief  �t���[����M��
 */
typedef struct
{
  DS_STREAM_REC_STATUS_CODE            status_code;                   //!< status
  uint16_t                             fixed_frame_len;               //!< �t���[���m�肵���Ƃ��̃t���[������
  DS_STREAM_TLM_DISRUPTION_STATUS_CODE tlm_disruption_status;         //!< �e�����r�┻��
  uint32_t                             count_of_carry_over_failures;  /*!< ��M�o�b�t�@�̌J�z�Ɏ��s������
                                                                           DS_receive �̌Ăяo���p�x�����������邱�Ƃ����� */
  // ����ڍ׏����g������Ȃ�C�����ɓ����iref. EQU Driver Super �� DRIVE_Super_rec�Ȃǁj
} DS_StreamRecStatus;


/**
 * @struct DS_Config
 * @brief  DriverSuper �̐ݒ�
 *
 *         �eIF�͂�����p�����Ă����D
 */
typedef struct
{
  // �y���[�U�[�ݒ�^�擾�l�z�iDS_Config�̕ϐ��͂��ׂĂ�Driver�������J�Ƃ���j
  DS_RecStatus rec_status_;                                 //!< IF��M��

  uint32_t rx_count_;                                       //!< �Ȃɂ�����̃f�[�^�̎�M��
  uint32_t rx_call_count_;                                  //!< DS_receive �Ăяo����

  ObcTime  rx_time_;                                        //!< �Ȃɂ�����̃f�[�^�̎�M����

  uint8_t  should_monitor_for_rx_disruption_;               //!< ��M�r�┻������邩�H
  uint32_t time_threshold_for_rx_disruption_;               //!< ��M�r�┻���臒l [ms]

  // �y���������ŗp����l�z
  uint8_t rx_buffer_[DS_RX_BUFFER_SIZE_MAX];                //!< �f�[�^��M�o�b�t�@

  DS_ERR_CODE (*load_init_setting)(struct DriverSuper* p_super);
                                                            /*!< DS_init �Ń��[�h����C�h���C�o�̏����ݒ�̐ݒ�֐�
                                                                 DS_reset_config �ł̐ݒ���I�[�o�[���[�h����
                                                                 �Ԃ�l�� DS_ERR_CODE */
} DS_Config;


/**
 * @struct DS_StreamConfig
 * @brief  DriverSuperStream�̐ݒ�
 */
struct DS_StreamConfig
{
  // �y�p����܂Ō��J�z
  // ����Ȃ�
  // setter/getter�ő��삷��

  // �y���[�U�[�ݒ�^�擾�l�z�iDS_StreamConfig�̕ϐ��͂��ׂĂ�Driver�������J�Ƃ���j
  uint8_t  is_enabled_;                                     //!< �L�����H

  uint8_t  is_strict_frame_search_;                         /*!< ���i�ȃt���[���T�����L�����H
                                                                 �m�C�Y�������╡��stream�g�p���Ƀt���[����M�R������S�ɂȂ������[�h
                                                                 - OFF�̏ꍇ�i�ʏ�͂�����j
                                                                    �w�b�_�������āC�t���[��������������C���̃t���[���̓t���[����₩��T������
                                                                 - ON�̏ꍇ
                                                                    �w�b�_�������āC�t���[��������������C���̃t���[���́C�������w�b�_�擪�̎��o�C�g����T������
                                                                    ���s���Ԃ͒����Ȃ�
                                                                 ����stream��m�C�Y�������Ă��܂����ꍇ�ȂǁC�{���w�b�_�łȂ��������w�b�_�Ƃ��ĔF�����Ă��܂��ꍇ�ɗL��������ƁC
                                                                 �_���I�ȃt���[����M�R��̊m��������Ȃ��O�ɋ߂Â��邱�Ƃ��ł���D
                                                                 �w�b�_������t���[���̏ꍇ�̂݁C�L���ɂł��� */

  DS_StreamSendStatus send_status_;                         //!< �t���[�����M��
  DS_StreamRecStatus  rec_status_;                          //!< �t���[����M��

  uint32_t general_cmd_tx_count_;                           //!< �ʏ�R�}���h���M��
  uint32_t req_tlm_cmd_tx_count_;                           //!< �e�����v���R�}���h���M��
  uint32_t req_tlm_cmd_tx_count_after_last_tx_;             /*!< �Ō�Ƀe��������M���Ă���̃e�����v���R�}���h���M��
                                                                 ���ꂪ 0 �łȂ��ꍇ�C�e�������ŐV�ł͂Ȃ��\�������� */
  uint32_t rx_frame_fix_count_;                             //!< �t���[����M�m���

  ObcTime  general_cmd_tx_time_;                            //!< �ʏ�R�}���h�ŏI���M����
  ObcTime  req_tlm_cmd_tx_time_;                            //!< �e�����v���R�}���h�ŏI���M����
  ObcTime  rx_frame_fix_time_;                              //!< �t���[���m�莞��

  uint8_t  *tx_frame_;                                      //!< �R�}���h�t���[��
  uint16_t tx_frame_size_;                                  /*!< �R�}���h�t���[���T�C�Y
                                                                 ���M�f�[�^���Ȃ��ꍇ�� 0 */

  uint8_t  rx_frame_[DS_RX_FRAME_SIZE_MAX];                 /*!< �f�[�^��M�t���[���o�b�t�@
                                                                 DS_RX_FRAME_SIZE_MAX �𒴂���悤�ȋ���ȃt���[���i�r�b�O�f�[�^�j�ɂ͖��Ή��i���������\��j
                                                                 �Ή�������ꍇ�C���̔z��ϐ����O���̑傫�Ȕz��̃|�C���^�ɏ㏑������K�v������D */

  const uint8_t* rx_header_;                                //!< ��M�f�[�^�̃w�b�_
  uint16_t rx_header_size_;                                 /*!< ��M�f�[�^�̃w�b�_�T�C�Y
                                                                 �w�b�_���Ȃ��ꍇ��0�ɐݒ�
                                                                 ���̏ꍇ�C��{�I�ɂ͌Œ蒷�i rx_frame_size �����j���g���D
                                                                 �w�b�_���Ȃ��C�ϒ��̏ꍇ�́C��M�O�i�Ⴆ�� DS_send_req_tlm_cmd �Ăяo���O�j ��
                                                                 rx_frame_size_ ��ݒ肷�邱�ƂŌŒ蒷�̂悤�Ɉ������ƂőΉ�����D
                                                                 �܂��C���������� Validation ��ʂ����߂ɂ��C�����l�͓K�؂Ȑ����ɂ��Ă������� */
  const uint8_t  *rx_footer_;                               //!< ��M�f�[�^�̃t�b�^
  uint16_t rx_footer_size_;                                 /*!< ��M�f�[�^�̃t�b�^�T�C�Y
                                                                 �w�b�_���Ȃ��ꍇ��0�ɐݒ� */
  int16_t  rx_frame_size_;                                  /*!< ��M�f�[�^�i�e�����g���j�t���[���T�C�Y
                                                                 ��M�f�[�^���Ȃ��ꍇ��0�ɐݒ�
                                                                 ��M�f�[�^���ς̏ꍇ�͕����ɐݒ� */
  int16_t  rx_framelength_pos_;                             /*!< ��M�f�[�^���̃t���[���T�C�Y�f�[�^�̑��݂���ꏊ�i�擪���琔���ĉ� byte �ڂɈʒu���邩�D0 �N�Z�j
                                                                 ��M�f�[�^���ϒ��̏ꍇ�̂ݎg�p�����D
                                                                 �t���[���T�C�Y�f�[�^���Ȃ��ꍇ�ɂ͕��ɐݒ肷��D
                                                                 �ϒ��ł��t���[���T�C�Y�f�[�^�̂Ȃ��t���[���́C�t�b�^���ݒ肳��Ă���ꍇ�͗L���ł���D
                                                                 �������C�t�b�^�̒T�����K�v�Ȃ��߁C���s���Ԃ͎኱�x���Ȃ�D
                                                                 �������C�w�b�_�����邱�Ƃ𐄏�����D�w�b�_���Ȃ��ꍇ�́C��M�����f�[�^�̖`������t���[���Ƃ݂Ȃ��D
                                                                 ��M�����t���[��������M�O�ɔ������Ă���ꍇ�́C
                                                                 �w�b�_���Ȃ��ꍇ�̂Ƃ��Ɠ��l�ɁC��M�O�i�Ⴆ�� DS_send_req_tlm_cmd �Ăяo���O�j ��
                                                                 rx_frame_size_ ��ݒ肷�邱�ƂŁC�Œ蒷�̂悤�Ɉ������ƂőΉ����邱�Ƃ𐄏�����D */
  uint16_t rx_framelength_type_size_;                       /*!< �t���[���T�C�Y�f�[�^�̌^�T�C�Y [Byte]
                                                                 ��M�f�[�^���ϒ��̏ꍇ�̂ݎg�p�����D
                                                                 �Ⴆ�� uint8 �Ȃ� 1�C uint32 �Ȃ� 4 */
  uint16_t rx_framelength_offset_;                          /*!< �t���[���T�C�Y�f�[�^�̃I�t�Z�b�g�l
                                                                 ��M�f�[�^���ϒ��̏ꍇ�̂ݎg�p�����
                                                                 �t���[���T�C�Y�f�[�^�ɂ��ϒ��f�[�^�̉�͂́u�t���[���̑S�T�C�Y�v�ɂ��s���邪�C
                                                                 �@��̒��ɂ̓w�b�_�ƃt�b�^�̕��͏������f�[�^���Ƃ��ăT�C�Y���\�������ꍇ������
                                                                 ���̏ꍇ�̃T�C�Y�����̂��߂Ɏg��
                                                                 �t���[���T�C�Y�f�[�^���u�t���[���̑S�T�C�Y�v�������Ă���ꍇ�ɂ�0�ɐݒ肷�� */

  uint8_t  should_monitor_for_tlm_disruption_;              //!< �e�����r�┻������邩�H
  uint32_t time_threshold_for_tlm_disruption_;              //!< �e�����r�┻���臒l [ms]

  DS_ERR_CODE (*data_analyzer_)(struct DS_StreamConfig* p_stream_config, void* p_driver);
                                                            /*!< ��M�f�[�^�̉�͊֐�
                                                                 p_driver �͌p����@��̃h���C�o�\���̂Ȃ�
                                                                 �Ԃ�l�� DS_ERR_CODE */
  DS_ERR_CODE ret_from_data_analyzer_;                      //!< data_analyzer_ �̕Ԃ�l

  // �y���������ŗp����l�z
  uint8_t  is_validation_needed_for_send_;                  //!< ���M�O�ɐݒ�l�� Validation ���K�v���H
  uint8_t  is_validation_needed_for_rec_;                   //!< ��M�O�ɐݒ�l�� Validation ���K�v���H

  uint16_t rx_frame_rec_len_;                               //!< ��M�f�[�^�t���[���̎�M�ς�Byte�Drx_frame_ �ɑ΂��鑀��|�C���^�ɂȂ�
  uint16_t rx_frame_head_pos_of_frame_candidate_;           /*!< ��M�o�b�t�@��͎��ɁC�t���[�����Ƃ����t���[���̐擪�ʒu�i0 �N�Z�j
                                                                 DS_analyze_rx_buffer_fixed_, DS_analyze_rx_buffer_variable_ ����Ă΂��֐��ŁC
                                                                 �t���[����₪���W�J���ȃG���[�i�t�b�^�̕s��v��t���[�����̕s�����j�����N�������ɁC
                                                                 �ēx�t���[����T���ł���悤�ɂ��邽�߂Ɏg�� */

  uint8_t  is_rx_buffer_carry_over_;                        //!< �J�z�����M�f�[�^�����邩�H
  uint16_t carry_over_buffer_size_;                         //!< �J�z�����M�f�[�^�̃T�C�Y
  uint16_t carry_over_buffer_next_pos_;                     //!< ����T�����n�߂�o�b�t�@�ʒu�i0 �N�Z�j
  uint8_t  rx_buffer_for_carry_over_[DS_RX_BUFFER_SIZE_MAX];
                                                            /*!< �t���[���m�肵���Ƃ��ɁC���̌�ɑ����Ă�����M�f�[�^���J�z�����߂̕ۑ��p�o�b�t�@
                                                                 ���̎�M���ɂ܂Ƃ߂ď��������� */
};
typedef struct DS_StreamConfig DS_StreamConfig;
// �� (*data_analyzer_)(DS_StreamConfig* p_stream_config, void* p_driver); �� DS_StreamConfig ���O���Q�Ƃł��Ȃ��̂ŁC�����������������Ă��邪�C�C�C�����Ɨǂ����������肻���D


/**
 * @struct DriverSuper
 * @brief  DriverSuper�̐ݒ�
 *         �eIF�͂�����p�����Ă����D
 */
struct DriverSuper
{
  // �y�p����܂Ō��J�z
  IF_LIST_ENUM      interface;                              //!< �p����̋@��̎g�pIF
  void              *if_config;                             //!< IF�ݒ�

  DS_Config         config;                                 //!< DriverSuper�̐ݒ�

  DS_StreamConfig   stream_config[DS_STREAM_MAX];           /*!< DriverSuperStream
                                                                 index���Ⴂ���̂قǗD��i�ɍ��シ�邩���D���s���x����j�D
                                                                 �g������F[0]�݂̂������āC�e�������Ɏd���� TLM ID �Ȃǂ� data_analyzer_ ���ŏ����𕪊�
                                                                 �g������F[0]�����e�����ƈ�ʃR�}���h�Ŏg���C[1]�ȍ~����������R�}���h�E�e�����g���Ŏg��
                                                                 ���C�܂����R�Ɏg���Ă��炦���� */
};
typedef struct DriverSuper DriverSuper;
// �� (*load_init_setting)(struct DriverSuper* p_super); �� DriverSuper ���O���Q�Ƃł��Ȃ��̂ŁC�����������������Ă��邪�C�C�C�����Ɨǂ����������肻���D


// ###### DriverSuper��{�֐� ######

/**
 * @brief  �p����̋@����DriverSuper������������
 *
 *         DriverSuper�\���̂��p����Drive�\���̂̃����o�Ƃ��Ē�`�i�p���j���C�|�C���^��n�����ƂŃ|�[�g������������D
 *         �����āC�\���̓��̏��������K�v�ȕϐ�������������D
 *         �f�t�H���g�l�̏㏑���� load_init_setting �ōs��
 * @note   DriverSuper���g�p���鎞�͋N�����ɕK�����{���邱��
 * @param  *p_super           ����������DriverSuper�\���̂ւ̃|�C���^
 * @param  *if_config         ����������Driver�ŗp�����Ă���IF��config�\����
 * @param  *load_init_setting DriverSuper�̏����ݒ胍�[�h�֐��|�C���^
 * @return DS_ERR_CODE
 */
DS_ERR_CODE DS_init(DriverSuper* p_super,
                    void* if_config,
                    DS_ERR_CODE (*load_init_setting)(DriverSuper* p_super));

/**
 * @brief  DriverSuper�̃��Z�b�g
 * @note   DS_init���ŌĂ΂�Ă���D
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @return DS_ERR_CODE
 */
DS_ERR_CODE DS_reset(DriverSuper* p_super);

/**
 * @brief  DriverSuper�̐ݒ�ɕs�����������Ă��Ȃ����`�F�b�N����
 *
 *         Driver�̐ݒ��ς����ꍇ�͖���Ăяo�����Ƃ𐄏�����
 * @note   DS_init���ŌĂ΂�Ă���D
 * @note   �����̊Ǘ��t���O��ύX���Ă���̂ŁC p_super �Ɍ�����const���͂Ȃ�
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @return DS_ERR_CODE
 */
DS_ERR_CODE DS_validate_config(DriverSuper* p_super);

/**
 * @brief  ��M�o�b�t�@���N���A����
 *
 *         �Ⴆ�΁C�w�b�_�Ȃ��e�����̏ꍇ�C�r���ŃS�~�f�[�^������ƈȌシ�ׂẴt���[��������Ă��܂��D
 *         ���̂悤�ȂƂ��iCRC�G���[���ł�Ƃ��C��M�f�[�^�����炩�ɂ��������ꍇ�j�ɁCbuffer����x�N���A���C
 *         ���ɓ͂��f�[�^����t���[����͂�擪����s���悤�ɂ��邽�߂ɗp����D
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @return DS_ERR_CODE
 */
DS_ERR_CODE DS_clear_rx_buffer(DriverSuper* p_super);

/**
 * @brief  �p����̋@�킩��e�����g������M����
 *
 *         �t���[�����m�肳���āCrx_frame_�ɂ����܂ŁD��� (data_analyzer_) �͂��Ȃ��̂Ńh���C�o�� DS_analyze_rec_data ���Ăяo������
 *         ����́C����stream�ł��e����������ID�Ȃǂŉ�͂�ς������Ƃ��Ȃǂ��z�肳��邽��
 * @note   �p����̋@��̃f�[�^�o�͎�����葁�������Œ���I�Ɏ��s���邱��
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @retval DS_ERR_CODE_OK  : IF_RX �ł̃G���[�Ȃ�
 * @retval DS_ERR_CODE_ERR : IF_RX �ł̃G���[����
 * @note   ��M�󋵂�G���[���� rec_status_ �Ɋi�[����Ă���
 */
DS_ERR_CODE DS_receive(DriverSuper* p_super);

/**
 * @brief  data_analyzer_ ���Ăяo���C��M�f�[�^����͂���D
 *
 *         DS_receive �ɂăf�[�^����M������C DSSC_get_rec_status(p_stream_config)->status_code �� DS_STREAM_REC_STATUS_FIXED_FRAME �Ȃ�ΌĂяo���D
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @param  stream   �ǂ�stream_config���g�p���邩�Dstream��0-MAX�Ȃ̂ŁC�p�����ENUM�Ȃǐ錾���Ďg���₷������΂����Ǝv���D
 * @param  p_driver �p����@��̃h���C�o�\���̂ȂǁDdata_analyzer_ �̑������D
 * @return DS_ERR_CODE : data_analyzer_ �̕Ԃ�l�����̂܂ܕԂ�
 * @note   data_analyzer_ �̕Ԃ�l�́C ret_from_data_analyzer_ �ɂ��ۑ������D
 */
DS_ERR_CODE DS_analyze_rec_data(DriverSuper* p_super, uint8_t stream, void* p_driver);

/**
 * @brief  �p����̋@��Ɉ�ʃR�}���h�𔭍s����
 *
 *         ���̃R�}���h�𑗂������Ƃɂ���ă��X�|���X���Ԃ��Ă��邱�Ƃ�z�肵�Ă��Ȃ��i���̏ꍇ�� DS_send_req_tlm_cmd ���g���j
 * @note   ���̊֐��̎��s�O�ɁCtx_frame, tx_frame_size�̐ݒ肪�K�v�ł���
 * @note   ����͊��N���X�Ȃ��߁C�A�m�}�����s�͍s��Ȃ��D�p����ŕԂ�l�����ēK�؂ɃA�m�}�����s���邱��
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @param  stream   �ǂ�stream_config���g�p���邩�Dstream��0-MAX�Ȃ̂ŁC�p�����ENUM�Ȃǐ錾���Ďg���₷������΂����Ǝv���D
 * @retval DS_ERR_CODE_OK  : ����I��
 * @retval DS_ERR_CODE_ERR : IF_TX �ł̃G���[����
 * @note   ��M�󋵂�G���[���� send_status_ �Ɋi�[����Ă���
 */
DS_ERR_CODE DS_send_general_cmd(DriverSuper* p_super, uint8_t stream);

/**
 * @brief  �p����̋@��Ƀe�����v���R�}���h�𔭍s����
 *
 *         �e�����ɂ��Ă� DS_receive �Ŏ󂯎��D
 * @note   ���̊֐��̎��s�O�ɁCtx_frame, tx_frame_size�̐ݒ肪�K�v�ł���
 * @param  *p_super DriverSuper�\���̂ւ̃|�C���^
 * @param  stream   �ǂ�stream_config���g�p���邩�Dstream��0-MAX�Ȃ̂ŁC�p�����ENUM�Ȃǐ錾���Ďg���₷������΂����Ǝv���D
 * @retval DS_ERR_CODE_OK  : ����I��
 * @retval DS_ERR_CODE_ERR : IF_TX �ł̃G���[����
 * @note   ��M�󋵂�G���[���� send_status_ �Ɋi�[����Ă���
 */
DS_ERR_CODE DS_send_req_tlm_cmd(DriverSuper* p_super, uint8_t stream);

// ###### DS_Config Getter/Setter ######
const DS_RecStatus* DSC_get_rec_status(const DriverSuper* p_super);
const uint32_t DSC_get_rx_count(const DriverSuper* p_super);
const uint32_t DSC_get_rx_call_count(const DriverSuper* p_super);
const ObcTime* DSC_get_rx_time(const DriverSuper* p_super);

const uint8_t DSC_get_should_monitor_for_rx_disruption(const DriverSuper* p_super);
void DSC_enable_monitor_for_rx_disruption(DriverSuper* p_super);
void DSC_disable_monitor_for_rx_disruption(DriverSuper* p_super);
const uint32_t DSC_get_time_threshold_for_rx_disruption(const DriverSuper* p_super);
void DSC_set_time_threshold_for_rx_disruption(DriverSuper* p_super,
                                              const uint32_t time_threshold_for_rx_disruption);
const DS_RX_DISRUPTION_STATUS_CODE DSC_get_rx_disruption_status(const DriverSuper* p_super);

// ###### DS_StreamConfig Getter/Setter ######
const uint8_t DSSC_get_is_enabled(const DS_StreamConfig* p_stream_config);
void DSSC_enable(DS_StreamConfig* p_stream_config);
void DSSC_disable(DS_StreamConfig* p_stream_config);

const uint8_t DSSC_get_is_strict_frame_search(const DS_StreamConfig* p_stream_config);
void DSSC_enable_strict_frame_search(DS_StreamConfig* p_stream_config);
void DSSC_disable_strict_frame_search(DS_StreamConfig* p_stream_config);

const DS_StreamSendStatus* DSSC_get_send_status(const DS_StreamConfig* p_stream_config);
const DS_StreamRecStatus* DSSC_get_rec_status(const DS_StreamConfig* p_stream_config);

const uint32_t DSSC_get_general_cmd_tx_count(const DS_StreamConfig* p_stream_config);
const uint32_t DSSC_get_req_tlm_cmd_tx_count(const DS_StreamConfig* p_stream_config);
const uint32_t DSSC_get_req_tlm_cmd_tx_count_after_last_tx(const DS_StreamConfig* p_stream_config);
const uint32_t DSSC_get_rx_frame_fix_count(const DS_StreamConfig* p_stream_config);

const ObcTime* DSSC_get_general_cmd_tx_time(const DS_StreamConfig* p_stream_config);
const ObcTime* DSSC_get_req_tlm_cmd_tx_time(const DS_StreamConfig* p_stream_config);
const ObcTime* DSSC_get_rx_frame_fix_time(const DS_StreamConfig* p_stream_config);

void DSSC_set_tx_frame(DS_StreamConfig* p_stream_config, uint8_t* tx_frame);
void DSSC_set_tx_frame_size(DS_StreamConfig* p_stream_config,
                            const uint16_t tx_frame_size);
const uint16_t DSSC_get_tx_frame_size(const DS_StreamConfig* p_stream_config);

const uint8_t* DSSC_get_rx_frame(const DS_StreamConfig* p_stream_config);
void DSSC_set_rx_header(DS_StreamConfig* p_stream_config,
                        const uint8_t* rx_header,
                        const uint16_t rx_header_size);
void DSSC_set_rx_footer(DS_StreamConfig* p_stream_config,
                        const uint8_t* rx_footer,
                        const uint16_t rx_footer_size);
void DSSC_set_rx_frame_size(DS_StreamConfig* p_stream_config,
                            const int16_t rx_frame_size);
const uint16_t DSSC_get_rx_header_size(const DS_StreamConfig* p_stream_config);
const uint16_t DSSC_get_rx_footer_size(const DS_StreamConfig* p_stream_config);
const int16_t DSSC_get_rx_frame_size(const DS_StreamConfig* p_stream_config);

void DSSC_set_rx_framelength_pos(DS_StreamConfig* p_stream_config,
                                 const int16_t rx_framelength_pos);
void DSSC_set_rx_framelength_type_size(DS_StreamConfig* p_stream_config,
                                       const uint16_t rx_framelength_type_size);
void DSSC_set_rx_framelength_offset(DS_StreamConfig* p_stream_config,
                                    const uint16_t rx_framelength_offset);

const uint8_t DSSC_get_should_monitor_for_tlm_disruption(const DS_StreamConfig* p_stream_config);
void DSSC_enable_monitor_for_tlm_disruption(DS_StreamConfig* p_stream_config);
void DSSC_disable_monitor_for_tlm_disruption(DS_StreamConfig* p_stream_config);
const uint32_t DSSC_get_time_threshold_for_tlm_disruption(const DS_StreamConfig* p_stream_config);
void DSSC_set_time_threshold_for_tlm_disruption(DS_StreamConfig* p_stream_config,
                                                const uint32_t time_threshold_for_tlm_disruption);
const DS_STREAM_TLM_DISRUPTION_STATUS_CODE DSSC_get_tlm_disruption_status(const DS_StreamConfig* p_stream_config);

void DSSC_set_data_analyzer(DS_StreamConfig* p_stream_config,
                            DS_ERR_CODE (*data_analyzer)(DS_StreamConfig* p_stream_config, void* p_driver));
const DS_ERR_CODE DSSC_get_ret_from_data_analyzer(const DS_StreamConfig* p_stream_config);


// ###### Driver�ėpUtil�֐� ######

/**
 * @brief  DS_DRIVER_ERR_CODE ���� CCP_EXEC_STS �ւ̕ϊ��֐�
 *
 *         DI ���� Driver �̊֐����Ăяo�����Ƃ��̃G���[�R�[�h�̕ϊ��ɗp����
 * @note   �ėpUtil�֐�
 * @param  DS_DRIVER_ERR_CODE
 * @return CCP_EXEC_STS
 */
CCP_EXEC_STS DS_conv_driver_err_to_ccp_exec_sts(DS_DRIVER_ERR_CODE code);

/**
 * @brief  DS_CMD_ERR_CODE ���� CCP_EXEC_STS �ւ̕ϊ��֐�
 *
 *         DI ���� Driver �̊֐����Ăяo�����Ƃ��̃G���[�R�[�h�̕ϊ��ɗp����
 * @note   �ėpUtil�֐�
 * @param  DS_CMD_ERR_CODE
 * @return CCP_EXEC_STS
 */
CCP_EXEC_STS DS_conv_cmd_err_to_ccp_exec_sts(DS_CMD_ERR_CODE code);

#endif
