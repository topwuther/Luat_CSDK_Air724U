/*********************************************************************************
 * 文件名称: aliyun_iot_common_error.h
 * 作       者:
 * 版       本:
 * 日       期: 2016-05-30
 * 描       述:
 * 其       它:
 * 历       史:
 **********************************************************************************/
#ifndef ALIYUN_IOT_COMMON_ERROR_H
#define ALIYUN_IOT_COMMON_ERROR_H

typedef enum IOT_RETURN_CODES
{
    ERROR_DEVICE_NOT_EXSIT = -311,
    ERROR_NET_TIMEOUT = -310,
    ERROR_CERT_VERIFY_FAIL  = -309,
    ERROR_NET_SETOPT_TIMEOUT = -308,
    ERROR_NET_SOCKET = -307,
    ERROR_NET_CONNECT = -306,
    ERROR_NET_BIND = -305,
    ERROR_NET_LISTEN = -304,
    ERROR_NET_RECV = -303,
    ERROR_NET_SEND = -302,
    ERROR_NET_CONN = -301,
    ERROR_NET_UNKNOWN_HOST = -300,

    CCP_CERT_EXPIRE_ERROR  = -132,
    CCP_DEVICE_NOT_EXSIT_ERROR = 131,
    CCP_INVALID_PAYLOAD = -130,
    CCP_UNACK_LIST_FULL = -129,
	CCP_REPUBLISH_LIST_FULL = -128,
    CCP_INVALID_TOPIC_NAME = -127,
    CCP_DATA_ERROR = -126,
    CCP_DATA_TOO_LONG = -125,
    CCP_BUFFER_TOO_SHORT = -124,
    CCP_INVALID_REMAIN_LENGTH = -123,
    CCP_NOT_CONNECTED = -122,
    CCP_RECONNECT_ERROR = -121,
    CCP_CONNECT_ERROR = -120,
    CCP_CONNECT_NETWORK_ERROR = -119,
    CCP_INVALID_MESSAGE_TYPE = -118,
    CCP_CLIENT_STATUS_ERROR = -117,
    CCP_SEND_PACKET_ERROR = -116,
    CCP_READ_PACKET_ERROR  = -115,
    CCP_CREATE_THREAD_ERROR = -114,
    CCP_CANCEL_THREAD_ERROR = -113,
    CCP_INVALID_PARAMETER = -112,
    CCP_SERVER_SIGN_ERROR = -111,
    CCP_URL_ENCODE_ERROR = -110,
    CCP_MALLOC_ERROR = -109,
    CCP_RSA_ENCRYPT_ERROR = -108,
    CCP_GEN_RSA_KEY_ERROR = -107,
    CCP_AES_ENCRYPT_ERROR = -106,
    CCP_AES_DECRYPT_ERROR = -105,
    CCP_PARSE_SERVER_ADDR_ERROR = -104,
    CCP_GET_SID_ERROR = -103,
    CCP_GET_SERVER_INFO_ERROR = -102,
    CCP_BASE64_ENCODE_ERROR = -101,
    CCP_BASE64_DECODE_ERROR = -100,

    MQTT_SUB_INFO_NOT_FOUND_ERROR = -42,
    MQTT_PUSH_TO_LIST_ERROR = -41,
    MQTT_TOPIC_FORMAT_ERROR = -40,
    NETWORK_RECONNECT_TIMED_OUT_ERROR = -39,/** Returned when the Network is disconnected and the reconnect attempt has timed out */
    MQTT_CONNACK_UNKNOWN_ERROR = -38,/** Connect request failed with the server returning an unknown error */
    MQTT_CONANCK_UNACCEPTABLE_PROTOCOL_VERSION_ERROR = -37,/** Connect request failed with the server returning an unacceptable protocol version error */
    MQTT_CONNACK_IDENTIFIER_REJECTED_ERROR = -36,/** Connect request failed with the server returning an identifier rejected error */
    MQTT_CONNACK_SERVER_UNAVAILABLE_ERROR = -35,/** Connect request failed with the server returning an unavailable error */
    MQTT_CONNACK_BAD_USERDATA_ERROR = -34,/** Connect request failed with the server returning a bad userdata error */
    MQTT_CONNACK_NOT_AUTHORIZED_ERROR = -33,/** Connect request failed with the server failing to authenticate the request */
    MQTT_CONNECT_ERROR = -32,
    MQTT_CREATE_THREAD_ERROR= -31,
    MQTT_PING_PACKET_ERROR= -30,
    MQTT_CONNECT_PACKET_ERROR= -29,
    MQTT_CONNECT_ACK_PACKET_ERROR= -28,
    MQTT_NETWORK_CONNECT_ERROR = -27,
    MQTT_STATE_ERROR = -26,
    MQTT_SUBSCRIBE_PACKET_ERROR= -25,
    MQTT_SUBSCRIBE_ACK_PACKET_ERROR= -24,
    MQTT_SUBSCRIBE_QOS_ERROR= -23,
    MQTT_UNSUBSCRIBE_PACKET_ERROR = -22,
    MQTT_PUBLISH_PACKET_ERROR= -21,
    MQTT_PUBLISH_QOS_ERROR= -20,
    MQTT_PUBLISH_ACK_PACKET_ERROR= -19,
    MQTT_PUBLISH_COMP_PACKET_ERROR = -18,
    MQTT_PUBLISH_REC_PACKET_ERROR= -17,
    MQTT_PUBLISH_REL_PACKET_ERROR= -16,
    MQTT_UNSUBSCRIBE_ACK_PACKET_ERROR= -15,
    MQTT_NETWORK_ERROR = -14,
    MQTT_PUBLISH_ACK_TYPE_ERROR= -13,

    ERROR_NO_SUPPORT = -12,
    ERROR_NO_PERSISTENCE = -11,
    ERROR_HTTP_BREAK = -10,
	ERROR_NULL_VALUE = -9,
    ERROR_HTTP_CONN = -8,                    /**< Connection failed. */
    ERROR_HTTP_PARSE = -7,                   /**< A URL parse error occurred. */
    ERRO_HTTP_UNRESOLVED_DNS = -6,           /**< Could not resolve the hostname. */
    ERROR_HTTP_PRTCL = -5,                   /**< A protocol error occurred. */
    ERROR_HTTP = -4,                         /**< An unknown error occurred. */
    ERROR_HTTP_CLOSED = -3,                  /**< Connection was closed by a remote host. */
    NULL_VALUE_ERROR = -2,
    FAIL_RETURN = -1,
    SUCCESS_RETURN = 0,
    HTTP_RETRIEVE_MORE_DATA = 1              /**< More data needs to be retrieved. */
} IOT_RETURN_CODES_E;

typedef enum CERTIFICATE_VERIFY_RESULT
{
    CERTIFICATE_NORMALLY = 0,
    CERTIFICATE_EXPIRED = -1111,
    CERTIFICATE_MISMATCH = -2222,
}CERTIFICATE_VERIFY_RESULT_E;

//#define ALI_IOT_TRACE
/**
 * @brief Debug level trace logging macro.
 *
 * Macro to print message function entry and exit
 */
#ifdef ALI_IOT_TRACE
#define IOT_FUNC_ENTRY    \
    {\
    printf("FUNC_ENTRY:   %s L#%d \n", __PRETTY_FUNCTION__, __LINE__);  \
    }
#define IOT_FUNC_EXIT    \
    {\
    printf("FUNC_EXIT:   %s L#%d \n", __PRETTY_FUNCTION__, __LINE__);  \
    }
#define IOT_FUNC_EXIT_RC(x)    \
    {\
    printf("FUNC_EXIT:   %s L#%d Return Code : %d \n", __PRETTY_FUNCTION__, __LINE__, x);  \
    return x; \
    }
#else

#define IOT_FUNC_ENTRY
#define IOT_FUNC_EXIT
#define IOT_FUNC_EXIT_RC(x) { return x; }
#endif

#endif
