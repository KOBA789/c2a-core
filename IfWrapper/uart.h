/**
 * @file
 * @brief UART�ʐM���b�p�[�̃N���X�ł��B
 * @note UART�C��̓I�ɂ�UART��LVTTL��z�肵�Ă���
 *       UART�ʐM���b�p�[�́A
 *       UART�̃C���^�[�t�F�[�X���������A
 *       UART�|�[�g�̏������A�f�[�^���M�A�f�[�^��M���s���A
 *       UART�ʐM���b�p�[�̃N���X�ł��B
 *       �X�̋@��̃C���^�[�t�F�[�X���W���[���Ɍp�������Ďg�p���܂��B
 */

#ifndef UART_H_
#define UART_H_

#include <src_user/Library/stdint.h>

/**
 * @enum  PARITY_SETTINGS
 * @brief UART �p�̃p���e�B�ݒ�
 */
typedef enum
{
  PARITY_SETTINGS_NONE, //!< �p���e�B�`�F�b�N�����Ȃ�
  PARITY_SETTINGS_ODD,  //!< ��p���e�B
  PARITY_SETTINGS_EVEN, //!< �����p���e�B
  PARITY_SETTINGS_MARK, //!< �p���e�B����� 1
  PARITY_SETTINGS_SPACE //!< �p���e�B����� 0
} PARITY_SETTINGS;

/**
 * @enum  UART_DATA_LENGTH
 * @brief UART �f�[�^�� (7 or 8)
 */
typedef enum
{
  UART_DATA_LENGTH_7BIT,
  UART_DATA_LENGTH_8BIT
} UART_DATA_LENGTH;

/**
 * @enum  UART_STOP_BIT
 * @brief UART �X�g�b�v�r�b�g
 */
typedef enum
{
  UART_STOP_BIT_1BIT,
  UART_STOP_BIT_15BIT,
  UART_STOP_BIT_2BIT
} UART_STOP_BIT;

/**
 * @enum  UART_ERR_CODE
 * @brief UART�p�̔ėp�G���[�R�[�h
 * @note  �^��int8_t��z�肵�Ă��邪, if_list�Ń��b�v����Ă��邽��UART�̌��J�֐��̕Ԃ�l��int�ƂȂ��Ă���
 *        ��{���Ȃ̂�RX�̕Ԃ�l�͐���������Byte��, �����G���[�R�[�h�ƂȂ��Ă��邽��
 */
typedef enum
{
  UART_UNKNOWN_ERR     = -14, //!< �����s��
  UART_DATA_NEGA_ERR   = -13, //!< ����M�f�[�^�T�C�Y�����̃G���[
  UART_FIFO_FULL_ERR   = -12, //!< TX���ɑ��MFIFO�t��
  UART_RX_ALL_ERR      = -11, //!< �p���e�B and FIFO and �X�g�b�v�r�b�g
  UART_FIFO_STOP_ERR   = -10, //!< FIFO and �X�g�b�v�r�b�g
  UART_PARITY_STOP_ERR = -9,  //!< �p���e�B and �X�g�b�v�r�b�g
  UART_STOP_BIT_ERR    = -8,  //!< �X�g�b�v�r�b�g�G���[
  UART_PARITY_FIFO_ERR = -7,  //!< �p���e�B and FIFO
  UART_FIFO_OVER_ERR   = -6,  //!< FIFO�I�[�o�[����
  UART_PARITY_ERR      = -5,  //!< �p���e�B�G���[
  UART_YET_ERR         = -4,  //!< �`�����l�����I�[�v��
  UART_ALREADY_ERR     = -3,  //!< �`�����l���I�[�v���ς�
  UART_BAUDRATE_ERR    = -2,  //!< �{�[���[�g�ُ�
  UART_CH_ERR          = -1,  //!< �`�����l���ُ� (Port_config�ɖ���)
  UART_OK              = 0    //!< OK��0�𓥏P
} UART_ERR_CODE;

/**
 * @struct UART_Config
 * @brief  UART�|�[�g�̏������A�f�[�^���M�A�f�[�^��M�̍ۂɕK�v�ƂȂ�ݒ�����i�[����\����
 */
typedef struct
{
  uint8_t          ch;              //!< �p����̋@�킪�Ȃ����Ă���|�[�g�ԍ�
  uint32_t         baudrate;        //!< �p����̋@��̃{�[���[�g�l
  PARITY_SETTINGS  parity_settings; //!< �p���e�B�ݒ�
  UART_DATA_LENGTH data_length;     //!< �f�[�^��
  UART_STOP_BIT    stop_bit;        //!< �X�g�b�v�r�b�g
} UART_Config;

/**
 * @brief UART_CONFIG�\���̂��O���[�o���C���X�^���X�Ƃ��Ē�`��, �|�C���^��n�����ƂŃ|�[�g������������.
 * @param[in] my_uart_v ����������UART_CONFIG�\���̂ւ̃|�C���^
 * @return int if_list��int�Ɠ��ꂳ��Ă���
 * @note UART�ʐM���b�p�[�̃N���X���g�p���鎞�͋N�����ɕK�����{���Ă�������.
 *       ���̊֐������s����O��UART_CONFIG�\���̓��̐ݒ�l��ݒ肵�Ă����K�v������܂�.
 */
int UART_init(void* my_uart_v);


/**
 * @brief UART_CONFIG�\���̂ɂĎw�肳�ꂽch����f�[�^����M���܂�
 * @param[in] my_uart_v   : �ΏۂƂ���UART_CONFIG�\���̂ւ̃|�C���^
 * @param[in] data_v      : ��M�f�[�^�i�[��ւ̃|�C���^
 * @param[in] buffer_size : ��M�f�[�^�i�[��̃f�[�^�T�C�Y
 * @retval 0: ��M�f�[�^�Ȃ�
 * @retval ��: ��M�f�[�^�o�C�g��
 * @retval ��: �G���[. UART_ERR_CODE �ɏ]��
 */
int UART_rx(void* my_uart_v, void* data_v, int buffer_size);


/**
 * @brief UART_CONFIG�\���̂ɂĎw�肳�ꂽch�փf�[�^�𑗐M���܂�
 * @param[in] my_uart_v : �ΏۂƂ���UART_CONFIG�\���̂ւ̃|�C���^
 * @param[in] data_v    : ���M�f�[�^�i�[��ւ̃|�C���^
 * @param[in] data_size : ���M�f�[�^�T�C�Y
 * @return int:  UART_ERR_CODE �ɏ]��
 */
int UART_tx(void* my_uart_v, void* data_v, int data_size);


/**
 * @brief �w�肳�ꂽch���J���Ȃ���
 * @param[in] my_uart_v : �J���Ȃ����ۂ̃|�[�g�ݒ�
 * @param[in] reason    : repoen�̗��R�������R�[�h�Ȃ�
 */
int UART_reopen(void* my_uart_v, int reason);

#endif
