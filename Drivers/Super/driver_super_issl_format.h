/**
 * @file
 * @brief  Driver�̂��߂�ISSL�t�H�[�}�b�g�ʐM�̂��߂̌��J��`�C�֐��Ȃ�
 */
#ifndef DRIVER_SUPER_ISSL_FORMAT_H_
#define DRIVER_SUPER_ISSL_FORMAT_H_

#include "driver_super.h"
#include <src_user/Library/stdint.h>


#define DS_ISSLFMT_STX_SIZE             (2)          //!< ISSL�W���t�H�[�}�b�g��STX�̃T�C�Y
#define DS_ISSLFMT_ETX_SIZE             (2)          //!< ISSL�W���t�H�[�}�b�g��ETX�̃T�C�Y
#define DS_ISSLFMT_LEN_SIZE             (2)          //!< ISSL�W���t�H�[�}�b�g��LEN�̃T�C�Y
#define DS_ISSLFMT_CRC_SIZE             (2)          //!< ISSL�W���t�H�[�}�b�g��CRC�̃T�C�Y
#define DS_ISSLFMT_COMMON_HEADER_SIZE   (DS_ISSLFMT_STX_SIZE + DS_ISSLFMT_LEN_SIZE)  //!< ISSL�W���t�H�[�}�b�g�̃R�����w�b�_�[�̃T�C�Y
#define DS_ISSLFMT_COMMON_FOOTER_SIZE   (DS_ISSLFMT_ETX_SIZE + DS_ISSLFMT_CRC_SIZE)  //!< ISSL�W���t�H�[�}�b�g�̃R�����t�b�^�[�̃T�C�Y
#define DS_ISSLFMT_CMD_HEADER_SIZE      (4)          //!< ISSL�W���t�H�[�}�b�g��CMD�w�b�_�[�̃T�C�Y
#define DS_ISSLFMT_TLM_HEADER_SIZE      (4)          //!< ISSL�W���t�H�[�}�b�g��TLM�w�b�_�[�̃T�C�Y
#define DS_ISSLFMT_STX_1ST_BYTE         (0xEB)       /*!< ISSL�W���t�H�[�}�b�g��STX 1st byte�̎擾
                                                          �����C2A�`��/��C2A�`�����ʂ�TLM HEADER�d�l�ł���  */
#define DS_ISSLFMT_STX_2ND_BYTE         (0x90)       /*!< ISSL�W���t�H�[�}�b�g��STX 2nd byte�̎擾
                                                          �����C2A�`��/��C2A�`�����ʂ�TLM HEADER�d�l�ł���  */
#define DS_ISSLFMT_ETX_1ST_BYTE         (0xC5)       /*!< ISSL�W���t�H�[�}�b�g��ETX 1st byte�̎擾
                                                          �����C2A�`��/��C2A�`�����ʂ�TLM HEADER�d�l�ł���  */
#define DS_ISSLFMT_ETX_2ND_BYTE         (0x79)       /*!< ISSL�W���t�H�[�}�b�g��ETX 2nd byte�̎擾
                                                          �����C2A�`��/��C2A�`�����ʂ�TLM HEADER�d�l�ł���  */


// [TODO] �ȉ��͖{����TCP�̒�`���玝���Ă������DTCP�̐������I������炻������
#define DS_C2AFMT_TCP_CMD_PRIMARY_HEADER_SIZE     (6)     //!< C2A�ԒʐM��TCPCMD�̂�Primary header size
#define DS_C2AFMT_TCP_CMD_SECONDARY_HEADER_SIZE   (1)     //!< C2A�ԒʐM��TCPCMD�̂�Secondary header size
#define DS_C2AFMT_TCP_TLM_PRIMARY_HEADER_SIZE     (6)     //!< C2A�ԒʐM��TCP��TLM��Primary header size
#define DS_C2AFMT_TCP_TLM_SECONDARY_HEADER_SIZE   (7)     //!< C2A�ԒʐM��TCP��TLM��Secondary header size�DADU��������


/**
 * @brief  TLM Version�̎擾
 * @note   �t���[�����m�肵�Ă���Ƃ��ɌĂяo������
 * @note   ����͔�C2A�`���Ŏg����TLM HEADER�d�l�ł���
 * @param  p_stream_config DriverSuper�\���̂�DS_StreamConfig
 * @return �o�[�W�����i�o�[�W���� 0x01�ł�uint8_t�����������̂��߂�uint32_t�ŕԂ��j
 */
uint32_t DS_ISSLFMT_get_tlm_version(const DS_StreamConfig* p_stream_config);

/**
 * @brief  TLM count�̎擾
 * @note   �t���[�����m�肵�Ă���Ƃ��ɌĂяo������
 * @note   ����͔�C2A�`���Ŏg����TLM HEADER�d�l�ł���
 * @param  version         TLM Version
 * @param  p_stream_config DriverSuper�\���̂�DS_StreamConfig
 * @return TLM count�i�o�[�W���� 0x01�ł�uint8_t�����������̂��߂�uint32_t�ŕԂ��j
 */
uint32_t DS_ISSLFMT_get_tlm_count(const uint32_t version, const DS_StreamConfig* p_stream_config);

/**
 * @brief  TLM id�̎擾
 * @note   �t���[�����m�肵�Ă���Ƃ��ɌĂяo������
 * @note   ����͔�C2A�`���Ŏg����TLM HEADER�d�l�ł���
 * @param  version         TLM Version
 * @param  p_stream_config DriverSuper�\���̂�DS_StreamConfig
 * @return id�i�o�[�W���� 0x01�ł�uint16_t�����������̂��߂�uint32_t�ŕԂ��j
 */
uint32_t DS_ISSLFMT_get_tlm_id(const uint32_t version, const DS_StreamConfig* p_stream_config);

/**
 * @brief  TLM length�̎擾
 * @note   �t���[�����m�肵�Ă���Ƃ��ɌĂяo������
 * @note   �����C2A�`��/��C2A�`�����ʂ�TLM HEADER�d�l�ł���
 * @param  p_stream_config DriverSuper�\���̂�DS_StreamConfig
 * @return id�i�o�[�W���� 0x01�ł�uint16_t�����������̂��߂�uint32_t�ŕԂ��j
 */
uint32_t DS_ISSLFMT_get_tlm_length(const DS_StreamConfig* p_stream_config);

/**
 * @brief  CRC�̌v�Z
 * @note   CRC-16-IBM���g��
 * @note   �����C2A�`��/��C2A�`�����ʂ�TLM HEADER�d�l�ł���
 * @param  c CRC���v�Z����f�[�^�̃|�C���^
 * @return n �f�[�^��
 */
uint16_t DS_ISSLFMT_calc_crc(const unsigned char* c, size_t n);

/**
 * @brief  TLM id�̎擾
 * @note   �t���[�����m�肵�Ă���Ƃ��ɌĂяo������
 * @note   �����C2A�`���Ŏg����TLM HEADER�d�l�ł���
 * @param  p_stream_config DriverSuper�\���̂�DS_StreamConfig
 * @return id
 */
uint8_t DS_C2AFMT_get_tlm_id(const DS_StreamConfig* p_stream_config);

#endif
