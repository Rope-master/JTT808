#ifndef JTT808_H
#define JTT808_H

/// 消息的最小长度，14字节(只有消息头的最短长度)
#define MSG_MIN_LEN 14
// 消息的最大长度，???字节
// #define MSG_MAX_LEN 18

enum ERROR {
    ERR_NONE             = 0,
    ERR_INVALIDATE_MSG   = 1,
    ERR_LENGTH_TOO_SHORT = 2,
    ERR_LENGTH_TOO_LONG  = 3
};

enum EncryptionType {
    NONE = 0,
    RSA  = 1
};

enum BOOL {
    FALSE = 0,
    TRUE  = 1
};

enum CommonReplyResult {
    CRR_SUCCESS     = 0,
    CRR_FAILED      = 1,
    CRR_WRONG_MSG   = 2,
    CRR_UNSUPPORTED = 3,
    CRR_ALARM       = 4
};

enum ColorCode {
    CC_NONE    = 0,
    CC_BLUE    = 1,
    CC_YELLOW  = 2,
    CC_BLACK   = 3,
    CC_WHITE   = 4,
    CC_OTHER   = 9
};

enum RegisterReplyResult {
    RRR_SUCCESS                     = 0,
    RRR_CAR_ALREADY_REGISTERED      = 1,
    RRR_NO_THE_CAR                  = 2,
    RRR_TERMINAL_ALREADY_REGISTERED = 3,
    RRR_NO_THE_TERMINAL             = 4
};

enum MsgId {
    MI_ServerCommonReply            = 0x8001,
    MI_TerminalCommonReply          = 0x0001,
    MI_TerminalHeartbeat            = 0x0002,
    MI_TerminalRegister             = 0x0100,
    MI_TerminalRegisterReponseReply = 0x8100,
    MI_TerminalLogoff               = 0x0003,
    MI_TerminalAuthentication       = 0x0102

};

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   DWORD;
typedef unsigned char  BCD;

typedef struct _MsgBodyProperties {
    BYTE           reservedBit;
    BOOL           hasSubPackage;
    EncryptionType encryptionType;
    int           msgLenth;
} MsgBodyProperties;

typedef struct _MsgPackagingItem {
    WORD total;
    WORD packegeSeq; 
} MsgPackagingItem;

typedef struct _MsgHeader {
    WORD              msgId;
    MsgBodyProperties msgBodyProperties;
    char              terminalPhone[20];
    WORD              flowId;
    MsgPackagingItem  msgPackagingItem; 
} MsgHeader;

typedef struct _PackageData {
    MsgHeader msgHeader;
    BYTE*     msgBody; 
} PackageData;

typedef struct _CommonRespMsgBody {
    WORD                replyFlowId;
    WORD                replyId;
    CommonReplyResult   replyCode; 
} CommonRespMsgBody;

typedef struct _TerminalRegisterMsgBody {
    WORD provinceId;
    WORD cityId;
    char manufacturerId[10];
    char terminalType[10];
    char terminalId[10];
    ColorCode licensePlateColor;
    char licensePlate[10]; 
} TerminalRegisterMsgBody;

typedef struct _TerminalRegisterMsgRespBody {
    WORD replyFlowId;
    RegisterReplyResult replyCode;
    char replyToken[256]; 
} TerminalRegisterMsgRespBody;

#ifdef __cplusplus
extern  "C" {
#endif

// 转义相关函数

ERROR DoEscapeForReceive(const BYTE rawBinarySeq[], BYTE binarySeq[], const int rawbinarySeqLen, const int binarySeqLen);
ERROR DoEscapeForSend(const BYTE rawBinarySeq[], BYTE binarySeq[], const int rawbinarySeqLen, const int binarySeqLen);

// 解析相关函数
BOOL Validate(const BYTE rawBinarySeq[], const int len);
ERROR ParseBinarySeq(const BYTE rawBinarySeq[], PackageData* packageData, const int len);
ERROR ToBinarySeq(const PackageData* packegeData, BYTE binarySeq[], const int len);
ERROR SetCheckSum(BYTE binarySeq[], const int len);
ERROR EncodePhoneNumber(BYTE binarySeq[], const char* phoneNumber, const int len);
ERROR DecodePhoneNumber(BYTE binarySeq[], char phoneNumber[], const int binarySeqLen, const int phoneNumberLen);
ERROR EncodeForCRMB(const CommonRespMsgBody *crmb, BYTE binarySeq[], const int len);
ERROR DecodeForCRMB(CommonRespMsgBody *crmb, const BYTE binarySeq[], const int len);
ERROR EncodeForTRMB(const TerminalRegisterMsgBody *trmb, BYTE binarySeq[], const int len);
ERROR DecodeForTRMRB(TerminalRegisterMsgRespBody *trmrb, const BYTE binarySeq[], const int len);


#ifdef __cplusplus
}
#endif


#endif