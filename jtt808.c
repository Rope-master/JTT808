#include <jtt808.h>

/**
 * 检查是否只包含连个标识位，如果包含的标识位不是两个（0，1或者不止两个），则这条信息是非法的。
 * @param rawBinarySeq  接收的二进制序列
 * @param rawbinarySeqLen  rawBinarySeq序列数组长度
 * @return ERROR           没有错误将返回  ERR_NONE, 非法序列则返回  ERR_INVALIDATE_MSG
 */
static ERROR validatedIdentifierBits(const BYTE rawBinarySeq[], const int rawbinarySeqLen) {
    int total = 0;
    int i = 0;

    for (i = 0; i < rawbinarySeqLen; i ++) {
        if (0x7e == rawBinarySeq[i]) {
            total += 1;
        }
    }
    return 2 == total ? ERR_NONE : ERR_INVALIDATE_MSG;
}

// 转义相关函数
/**
 * 接收一个从服务端发来的原始二进制序列，
 * 去除标识位并转义，返回只包含消息体、消息体以及校验码的二进制序列
 * @param rawBinarySeq  转义前的原始二进制序列
 * @param binarySeq     转义后的二进制序列
 * @param rawbinarySeqLen rawBinarySeq数组长度
 * @param binarySeqLen binarySeq数组长度
 * @return ERROR 错误类型。
 */
ERROR DoEscapeForReceive(const BYTE rawBinarySeq[], BYTE binarySeq[], const int rawbinarySeqLen, const int binarySeqLen) {
    int i = 0, j = 0;
    ERROR err = ERR_NONE;


    err = validatedIdentifierBits(rawBinarySeq, rawbinarySeqLen);
    if (ERR_NONE != err) {
        return err;
    }

    if (MSG_MIN_LEN > rawbinarySeqLen) {
        return ERR_LENGTH_TOO_SHORT;
    }

    // if (MSG_MAX_LEN < rawbinarySeqLen) {
    //     return ERR_LENGTH_TOO_LONG;
    // }

    for (i = 0; i < rawbinarySeqLen; i++) {

        if (0x7e == rawBinarySeq[i]) {
            continue;
        }

        if (0x7d == rawBinarySeq[i]) {
            if (0x02 == rawBinarySeq[i + 1]) {
                binarySeq[j++] = 0x7e;
                i++;
                continue;
            }
            if (0x01 ==  rawBinarySeq[i + 1]) {
                binarySeq[j++] = 0x7d;
                i++;
                continue;
            }
            return ERR_INVALIDATE_MSG;
        }
        binarySeq[j++] = rawBinarySeq[i];
    }
    return ERR_NONE;
}

/**
 * 接收一个包含消息体、消息体以及校验码的二进制序列，将其转义并加上标识位。
 * @param rawBinarySeq  转义前的原始二进制序列
 * @param binarySeq     转义后的二进制序列
 * @param rawbinarySeqLen rawBinarySeq数组长度
 * @param binarySeqLen binarySeq数组长度
 * @return ERROR 错误类型。
 */
ERROR DoEscapeForSend(const BYTE rawBinarySeq[], BYTE binarySeq[], const int rawbinarySeqLen, const int binarySeqLen) {
    return ERR_NONE;
}

// 解析相关函数
/**
 * 判定当前的消息包是否合法（没有丢失或者被篡改数据）。即验证校验码。
 * @param rawBinarySeq 转义后的原始二进制序列
 * @param len rawBinarySeq数组长度
 * @return BOOL值，TRUE表示合法
 */
BOOL Validate(BYTE rawBinarySeq[], const int len) {
    return TRUE;
}

/**
 * 解析一个包含消息头和消息体的二进制序列，并将其封装成PackageData数据包。
 * 这些数据会存放在PackageData* packageData指针指向的结构体中。
 * @param rawBinarySeq 转义后的原始二进制序列
 * @param packageData  二进制序列中包含的信息会被保存到packageData指针指向的结构体中
 * @param len rawBinarySeq数组长度
 * @return ERROR 错误类型。
 */
ERROR ParseBinarySeq(const BYTE rawBinarySeq[], PackageData* packageData, const int len) {
    return ERR_NONE;
}

/**
 * 将packageData结构体包含的信息转换成二进制序列,这个序列只包含消息头和消息体
 * @param packegeData 指向包含特定信息的结构体的指针
 * @param binarySeq  将转义后的二进制数据存放到binarySeq
 * @param len        binarySeq数组长度
 * @return ERROR     错误类型。
 */
ERROR ToBinarySeq(const PackageData* packegeData, BYTE binarySeq[], const int len) {
    return ERR_NONE;
}

/**
 * 根据传过来的的binarySeq计算校验码，并填充到相应的位置。返回错误类型信息。
 * @param binarySeq  未计算校验码的二进制序列
 * @param len        binarySeq数组长度
 * @return ERROR     错误类型。
 */
ERROR SetCheckSum(BYTE binarySeq[], const int len) {
    return ERR_NONE;
}

/**
 * 将终端手机号转换成BCD码并写入binarySeq序列中。
 * @param binarySeq   未计算校验码也未填写终端号码的二进制序列
 * @param phoneNumber 手机号字符串
 * @param len binarySeq数组长度
 * @return ERROR     错误类型
 */
ERROR EncodePhoneNumber(BYTE binarySeq[], const char* phoneNumber, const int len) {
    return ERR_NONE;
}

/**
 * 将BCD码转换成终端手机号并写入phoneNumber字符数组中
 * @param binarySeq   待解析的二进制序列
 * @param phoneNumber 手机号字符串
 * @param binarySeqLen binarySeq数组长度
 * @param phoneNumberLen phoneNumber数组长度
 * @return ERROR     错误类型
 */
ERROR DecodePhoneNumber(BYTE binarySeq[], char phoneNumber[], const int binarySeqLen, const int phoneNumberLen) {
    return ERR_NONE;
}

/**
 * 对crmb指向的通用应答消息体进行编码，写入binarySeq字节数组。
 * @param crmb       指向通用应答消息体的指针
 * @param binarySeq  要写入的二进制字节数组
 * @param len        binarySeq数组长度
 * @return ERROR     错误类型
 */
ERROR EncodeForCRMB(const CommonRespMsgBody *crmb, BYTE binarySeq[], const int len) {
    return ERR_NONE;
}

/**
 * 对binarySeq字节数组进行编码，写入crmb指向的通用应答消息体
 * @param crmb      指向通用应答消息体的指针
 * @param binarySeq 要读取解码的二进制字节数组
 * @param len       binarySeq数组长度
 * @return ERROR     错误类型
 */
ERROR DecodeForCRMB(CommonRespMsgBody *crmb, const BYTE binarySeq[], const int len) {
    return ERR_NONE;
}

/**
 * 对trmb指向的终端注册消息体进行编码，写入binarySeq字节数组
 * @param trmb      指向终端注册消息体的指针
 * @param binarySeq 要写入的二进制字节数组
 * @param len       binarySeq数组长度
 * @return ERROR     错误类型
 */
ERROR EncodeForTRMB(const TerminalRegisterMsgBody *trmb, BYTE binarySeq[], const int len) {
    return ERR_NONE;
}

/**
 * 对binarySeq字节数组进行编码，写入trmrb指向的终端注册应答消息体
 * @param trmrb     指向终端注册应答消息体的指针
 * @param binarySeq 要读取解码的二进制字节数组
 * @param len       binarySeq数组长度
 * @return ERROR    错误类型
 */
ERROR DecodeForTRMRB(TerminalRegisterMsgRespBody *trmrb, const BYTE binarySeq[], const int len) {
    return ERR_NONE;
}