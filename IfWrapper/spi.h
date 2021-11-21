/**
 * @file spi.h
 * @brief SPI�ʐM(Master��)�̃��b�p�[�N���X�ł��B
 * @note SPI�ʐM���b�p�[�́A
 *       SPI�̃C���^�[�t�F�[�X���������A
 *       SPI�|�[�g�̏������A�f�[�^���M�A�f�[�^��M���s���A
 *       SPI�ʐM���b�p�[�̃N���X�ł��B
 *       �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp�������Ďg�p���܂��B
 */
#ifndef SPI_H_
#define SPI_H_

#include <src_user/Library/stdint.h>

 /**
  * @enum  SPI_MODE
  * @brief SPI���샂�[�h�̐ݒ�
  * @note  �^��uint8_t��z��
  */
typedef enum
{
  SPI_MODE_0 = 0,  //!< CPOL=0, CPHA = 0
  SPI_MODE_1 = 1,  //!< CPOL=0, CPHA = 1
  SPI_MODE_2 = 2,  //!< CPOL=1, CPHA = 0
  SPI_MODE_3 = 3,  //!< CPOL=1, CPHA = 1
} SPI_MODE;

/**
 * @enum  SPI_MODE
 * @brief SPI���샂�[�h�̐ݒ�
 * @note  �^��uint8_t��z��
 */
typedef enum
{
  SPI_TX_CS_STATE_LOW  = 0,   //!< ���M���CS��Low�̂܂܂ɂ��A���̂܂܎�M����Ȃǂ��s��
  SPI_TX_CS_STATE_HIGH = 1,   //!< ���M���CS��High�ɂ���
} SPI_TX_CS_STATE;

/**
 * @struct SPI_Config
 * @brief SPI�|�[�g�̏������A�f�[�^���M�E��M�̍ۂɕK�v�ƂȂ�ݒ�����i�[����\���̂ł��B
 */
typedef struct
{
  uint8_t  comm_ch;            //!< �p����̋@�킪�Ȃ����Ă���ʐM�|�[�g�ԍ�
  uint8_t  gpio_ch;            //!< �p����̋@�킪�Ȃ����Ă���Chip Select�pGPIO�|�[�g�ԍ�
  uint32_t frequency_khz;      //!< SPI�N���b�N�̎��g�� �P��:kHz
  SPI_MODE mode;               //!< SPI���샂�[�h
  uint8_t  tx_data_for_rx;     //!< RX��������邽�߂ɒ���I�ɑ���TX�f�[�^
  uint16_t rx_length;          //!< ��M����f�[�^���A��M����O�ɐݒ肷��
  SPI_TX_CS_STATE tx_cs_state; //!< ���M������Chip Select��High�ɂ��邩�ǂ����̃t���O
} SPI_Config;

/**
 * @enum  SPI_ERR_CODE
 * @brief SPI�p�̔ėp�G���[�R�[�h
 * @note  �^��int8_t��z�肵�Ă��邪, if_list�Ń��b�v����Ă��邽��SPI�̌��J�֐��̕Ԃ�l��int�ƂȂ��Ă���
 *        ��{���Ȃ̂�RX�̕Ԃ�l�͐���������Byte��, �����G���[�R�[�h�ƂȂ��Ă��邽��
 */
typedef enum
{
  SPI_ERR_UNKNOWN = -20,          //!< �����s��
  SPI_ERR_RX_BUFFER_SMALL = -14,  //!< ��M�o�b�t�@�T�C�Y����M�f�[�^����菬����
  SPI_ERR_DATA_NEGA = -13,        //!< ����M�f�[�^�T�C�Y�����̃G���[
  SPI_ERR_DATA_LARGE = -12,       //!< ����M�f�[�^�T�C�Y���傫������
  SPI_ERR_GPIO = -5,              //!< GPIO�֘A�G���[
  SPI_ERR_YET = -4,               //!< �`�����l�����I�[�v��
  SPI_ERR_ALREADY = -3,           //!< �`�����l���I�[�v���ς�
  SPI_ERR_FREQUENCY = -2,         //!< ���g���ُ�
  SPI_ERR_CH = -1,                //!< �`�����l���ُ� (Port_config�ɖ���)
  SPI_ERR_OK = 0,                 //!< OK��0�𓥏P
} SPI_ERR_CODE;

/**
 * @brief SPI_Config�\���̂��O���[�o���C���X�^���X�Ƃ��Ē�`���A�|�C���^��n�����ƂŃ|�[�g�����������܂��B
 * @param[in] my_spi_v ����������SPI_Config�\���̂ւ̃|�C���^
 * @return int SPI_ERR_CODE�Ƃ�������, if_list��int�Ɠ��ꂳ��Ă���B
 * @note SPI�ʐM���b�p�[�̃N���X���g�p���鎞�͋N�����ɕK�����{���Ă��������B
 *       ���̊֐������s����O��SPI_Config�\���̓��̐ݒ�l��ݒ肵�Ă����K�v������܂��B
 */
int SPI_init(void* my_spi_v);

/**
 * @brief SPI_Config�\���̂ɂĎw�肳�ꂽch����f�[�^����M���܂�
 * @param[in] my_spi_v    : �ΏۂƂ���SPI_Config�\���̂ւ̃|�C���^
 * @param[in] data_v      : ��M�f�[�^�i�[��ւ̃|�C���^
 * @param[in] buffer_size : ��M�f�[�^�i�[��̃o�b�t�@�T�C�Y�ASPI�ʐM�ɂ���M�f�[�^�����������K�v������
 * @retval 0: ��M�f�[�^�Ȃ�
 * @retval ��: ��M�f�[�^�o�C�g��
 * @retval ��: �G���[. SPI_ERR_CODE �ɏ]��
 * @note ���̊֐�����SPI�ǂݏo���v���𑗐M���Ă��܂�
 */
int SPI_rx(void* my_spi_v, void* data_v, int buffer_size);

/**
 * @brief SPI_Config�\���̂ɂĎw�肳�ꂽch�փf�[�^�𑗐M���܂�
 * @param[in] my_spi_v : �ΏۂƂ���SPI_Config�\���̂ւ̃|�C���^
 * @param[in] data_v     : ���M�f�[�^�i�[��ւ̃|�C���^
 * @param[in] count      : ���M�f�[�^�T�C�Y
 * @return int:  SPI_ERR_CODE �ɏ]��
 */
int SPI_tx(void* my_spi_v, void* data_v, int data_size);

/**
 * @brief �w�肳�ꂽch���J���Ȃ���
 * @param[in] my_spi_v �J���Ȃ����ۂ̃|�[�g�ݒ�
 * @param[in] reason   reopen���闝�R�D0�͐��푀������� TODO: reason��enum�����H
 * @return int:  SPI_ERR_CODE �ɏ]��
 */
int SPI_reopen(void* my_spi_v, int reason);

/**
 * @brief rx_length��ݒ肷��
 * @param[in] my_spi_v  : �ΏۂƂ���SPI_Config�\���̂ւ̃|�C���^
 * @param[in] rx_length : �ݒ肷���M�f�[�^��
 * @return �Ȃ�
 */
void SPI_set_rx_length(void* my_spi_v, const uint16_t rx_length);

/**
 * @brief cs_up_after_tx��ݒ肷��
 * @param[in] my_spi_v       : �ΏۂƂ���SPI_Config�\���̂ւ̃|�C���^
 * @param[in] tx_cs_state    : �ݒ肷��tx_cs_state
 * @return �Ȃ�
 */
void SPI_set_tx_cs_state(void* my_spi_v, const SPI_TX_CS_STATE tx_cs_state);

#endif
