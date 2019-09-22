#include "jtt808.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
// 测试
// #include <stdio.h>

/**
 * 检查是否只包含连个标识位，如果包含的标识位不是两个（0，1或者不止两个），则这条信息是非法的。
 * @param rawBinarySeq  接收的二进制序列
 * @param rawbinarySeqLen  rawBinarySeq序列数组长度
 * @return JTT_ERROR           没有错误将返回  ERR_NONE, 非法序列则返回  ERR_INVALIDATE_MSG
 */
static JTT_ERROR validatedIdentifierBits(const BYTE rawBinarySeq[], const int rawbinarySeqLen) {
    int total = 0;
    int i = 0;

    for (i = 0; i < rawbinarySeqLen; i ++) {
        if (0x7e == rawBinarySeq[i]) {
            total += 1;
        }
    }
    return 2 == total ? ERR_NONE : ERR_INVALIDATE_MSG;
}
/**
 *  返回标识位的索引
 * @param rawBinarySeq 转义前的原始二进制序列
 * @param len          rawBinarySeq序列数组长度
 * @param startIndex   标识位头部索引
 * @param endIndex     标识位尾部索引
 * @return JTT_ERROR        错误类型。
 */
static JTT_ERROR searchForIdentifierBitsStartAndEndIndex(const BYTE rawBinarySeq[], const int rawbinarySeqLen, int *startIndex, int *endIndex) {
    int i = 0, j = rawbinarySeqLen - 1;
    JTT_ERROR err = ERR_NONE;

    err = validatedIdentifierBits(rawBinarySeq, rawbinarySeqLen);
    if (ERR_NONE != err) {
        return err;
    }

    for (i = 0; i < rawbinarySeqLen; i++) {
        if (0x7e == rawBinarySeq[i]) {
            *startIndex = i;
            break;
        }
    }

    for (j = rawbinarySeqLen - 1; j >= 0; j ++) {
        if (0x7e == rawBinarySeq[j]) {
            *endIndex = j;
            break;
        }
    }
    return ERR_NONE;
}

// 转义相关函数
/**
 * 接收一个从服务端发来的原始二进制序列，
 * 去除标识位并转义，返回只包含消息体、消息体以及校验码的二进制序列
 * @param rawBinarySeq  转义前的原始二进制序列
 * @param binarySeq     转义后的二进制序列
 * @param rawbinarySeqLen rawBinarySeq数组长度
 * @return JTT_ERROR 错误类型。
 */
//  * @param binarySeqLen binarySeq数组长度
JTT_ERROR DoEscapeForReceive(const BYTE rawBinarySeq[], BYTE binarySeq[], const int rawbinarySeqLen/*, const int binarySeqLen*/) {
    int i = 0, j = 0;
    JTT_ERROR err = ERR_NONE;


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
 * @return JTT_ERROR 错误类型。
 */
JTT_ERROR DoEscapeForSend(const BYTE rawBinarySeq[], BYTE binarySeq[], const int rawbinarySeqLen, const int binarySeqLen) {
    int i = 0, j = 0;

    if (rawbinarySeqLen > binarySeqLen ) {
        return ERR_LENGTH_TOO_SHORT;
    }

    binarySeq[j++] = 0x7e; //标识位头
    for (int i = 0; i < rawbinarySeqLen; i++) {
        if (0x7e == rawBinarySeq[i]) {
            binarySeq[j++] = 0x7d;
            binarySeq[j++] = 0x02;
            continue;
        }

        if (0x7d == rawBinarySeq[i]) {
            binarySeq[j++] = 0x7d;
            binarySeq[j++] = 0x01;
            continue;
        }
        binarySeq[j++] = rawBinarySeq[i];
    }
    binarySeq[j++] = 0x7e; //标识位尾
    return ERR_NONE;
}

// 解析相关函数
/**
 * 判定当前的消息包是否合法（没有丢失或者被篡改数据）。即验证校验码。
 * @param rawBinarySeq 转义后的原始二进制序列
 * @param len rawBinarySeq数组长度
 * @return BOOL值，TRUE表示合法
 */
bool Validate(const BYTE rawBinarySeq[], const int len) {
    int startIndex = 0, endIndex = 0;
    int i = 0;
    int checkSum = 0, calculateSum = 0;

    searchForIdentifierBitsStartAndEndIndex(rawBinarySeq, len, &startIndex, &endIndex);

    if (0 >= endIndex || len - 1 <= startIndex || startIndex > endIndex) {
        return false;
    }

    checkSum = rawBinarySeq[endIndex - 1];

    // 恒等率，X ^ 0 == X 
    for (i = startIndex + 1; i < endIndex - 1; i++) {
        calculateSum ^= rawBinarySeq[i];
    }
    // 归零率, X ^ X == 0
    if (checkSum ^ calculateSum == 0) {
        return true;
    }

    return false;
}

/**
 * 解析一个包含消息头和消息体的二进制序列，并将其封装成PackageData数据包。
 * 这些数据会存放在PackageData* packageData指针指向的结构体中。
 * @param rawBinarySeq 转义后的原始二进制序列
 * @param packageData  二进制序列中包含的信息会被保存到packageData指针指向的结构体中
 * @param len rawBinarySeq数组长度
 * @return JTT_ERROR 错误类型。
 */
JTT_ERROR DecodeForMsgHeader(const BYTE rawBinarySeq[], PackageData* packageData, const int len) {
    if (12 > len) {
        return ERR_LENGTH_TOO_SHORT;
    } 
    if (false == Validate(rawBinarySeq, len)) {
        return ERR_INVALIDATE_MSG;
    }

    

    // 0, 1  -   消息 ID
    packageData->msgHeader.msgId =  ((WORD)((rawBinarySeq[0] << 8) & 0xff)) + ((WORD)(rawBinarySeq[1] & 0xff));

    // 2,3 - 消息体属性

    packageData->msgHeader.msgBodyProperties.reservedBit = rawBinarySeq[2] & 0xc0;
    packageData->msgHeader.msgBodyProperties.hasSubPackage =  (rawBinarySeq[2] & 0x20) ? true : false;
    packageData->msgHeader.msgBodyProperties.encryptionType = (rawBinarySeq[2] & 0x04) ? RSA : NONE;
    packageData->msgHeader.msgBodyProperties.msgLenth = (int)(((rawBinarySeq[2] & 0x03) << 8) + (rawBinarySeq[3] & 0xff));
    // 4,5,6,7,8,9 - 终端手机号
    DecodePhoneNumber(rawBinarySeq, packageData->msgHeader.terminalPhone);

    // 10,11 - 消息流水号
    packageData->msgHeader.flowId = ((WORD)((rawBinarySeq[10] << 8) & 0xff)) + ((WORD)(rawBinarySeq[11] & 0xff));

    if (true == packageData->msgHeader.msgBodyProperties.hasSubPackage) {
        // 12, 13 消息总包数
        packageData->msgHeader.msgPackagingItem.total = ((WORD)((rawBinarySeq[12] << 8) & 0xff)) + ((WORD)(rawBinarySeq[13] & 0xff));
        // 14, 15 包序号
        packageData->msgHeader.msgPackagingItem.total = ((WORD)((rawBinarySeq[14] << 8) & 0xff)) + ((WORD)(rawBinarySeq[15] & 0xff));
    }
    return ERR_NONE;
}

/**
 * 将packageData结构体包含的信息转换成二进制序列,这个序列只包含消息头和消息体
 * @param packegeData 指向包含特定信息的结构体的指针
 * @param binarySeq  将转义后的二进制数据存放到binarySeq
 * @return JTT_ERROR     错误类型。
 */
//  * @param len        binarySeq数组长度
JTT_ERROR EncodeForMsgHeader(const PackageData* packageData, BYTE binarySeq[]/*, const int len*/) {
    // 0, 1  -   消息 ID
    binarySeq[0] = (BYTE) ((packageData->msgHeader.msgId >> 8) & 0xff);
    binarySeq[1] = (BYTE) (packageData->msgHeader.msgId  & 0xff);

    // 2,3 - 消息体属性
    binarySeq[2] = packageData->msgHeader.msgBodyProperties.reservedBit & 0xc0;
    if (true == packageData->msgHeader.msgBodyProperties.hasSubPackage) {
        binarySeq[2] |= (1 << 5);
    }

    switch(packageData->msgHeader.msgBodyProperties.encryptionType) {
        case RSA: {
            binarySeq[2] &= ~(1 << 4); 
            binarySeq[2] &= ~(1 << 3);
            binarySeq[2] &= ~(1 << 2);
            break;
        }
        default: {
            binarySeq[2] &= ~(1 << 4); 
            binarySeq[2] &= ~(1 << 3);
             binarySeq[2] |= (1 << 2);
            break;
        }
    }

    binarySeq[2] =  packageData->msgHeader.msgBodyProperties.msgLenth & 0x300;
    binarySeq[3] =  packageData->msgHeader.msgBodyProperties.msgLenth & 0xff;
    EncodePhoneNumber(binarySeq, packageData->msgHeader.terminalPhone);

    // 10,11 - 消息流水号
    binarySeq[10] = (BYTE)((packageData->msgHeader.flowId >> 8) & 0xff);
    binarySeq[11] = (BYTE)(packageData->msgHeader.flowId & 0xff);

    if (true == packageData->msgHeader.msgBodyProperties.hasSubPackage) {
        // 12, 13 消息总包数
        binarySeq[12] = (BYTE)((packageData->msgHeader.msgPackagingItem.total >> 8) & 0xff);
        binarySeq[13] = (BYTE)(packageData->msgHeader.msgPackagingItem.total & 0xff);

        // 14, 15 包序号
        binarySeq[14] = (BYTE)((packageData->msgHeader.msgPackagingItem.packegeSeq >> 8) & 0xff);
        binarySeq[15] = (BYTE)(packageData->msgHeader.msgPackagingItem.packegeSeq & 0xff);
    }

    return ERR_NONE;
}

/**
 * 根据传过来的的binarySeq计算校验码，并填充到相应的位置。返回错误类型信息。
 * @param binarySeq  未计算校验码的二进制序列
 * @param len        binarySeq数组长度
 * @return JTT_ERROR     错误类型。
 */
JTT_ERROR SetCheckSum(BYTE binarySeq[], const int len) {
    int startIndex = 0, endIndex = 0;
    int i = 0;

    searchForIdentifierBitsStartAndEndIndex(binarySeq, len, &startIndex, &endIndex);

    if (0 >= endIndex || len - 1 <= startIndex || startIndex > endIndex) {
        return ERR_INVALIDATE_MSG;
    }
    // binarySeq[endIndex - 1] 即为校验码所在位
    // 利用恒等率，必须初始化该位为0 
    binarySeq[endIndex - 1] = 0;

    for (i = startIndex + 1; i < endIndex - 1; i++) {
        binarySeq[endIndex - 1] ^= binarySeq[i];
    }
    return ERR_NONE;
}

/**
 * 将终端手机号转换成BCD码并写入binarySeq序列中。
 * （暂时不支持港澳台地区的手机号，作者不了解这些地区的手机号规则，
 * 知道详情的朋友可以给作者发邮件 @email crazypandas@aliyun.com 或者提交Issue）
 * @param binarySeq   未计算校验码也未填写终端号码的二进制序列
 * @param phoneNumber 手机号字符串
 * @return JTT_ERROR     错误类型
 */
//  * @param len binarySeq数组长度
JTT_ERROR EncodePhoneNumber(BYTE binarySeq[], const char* phoneNumber/*, const int len*/) {
    int phoneNumberLen = 0;
    char *thePhoneNumber = NULL;
    int i = 0;
    BYTE high = 0, low = 0;
    //  0,1-消息ID; 2,3-消息体属性； 4,5,6,7,8,9,9-终端手机号
    int phoneNumberIndex = 4;

    phoneNumberLen = strlen(phoneNumber);
    // 中国大陆地区手机号目前(2019)是11位，不足12位补零 
    if (phoneNumberLen < 11 || phoneNumberLen > 12) {
        return ERR_INVALIDATE_PHONE;
    }
    thePhoneNumber = (char*) malloc(sizeof(char) * (strlen(phoneNumber) + 2));
    thePhoneNumber[0] = '\0';

    // 中国大陆地区手机号目前(2019)是11位，不足12位补零 
    if (phoneNumberLen == 11) {
        thePhoneNumber[0] = '0';
        thePhoneNumber[1] = '\0';
    }
    strncat(thePhoneNumber, phoneNumber, strlen(phoneNumber));

    for (i = 0; i < strlen(thePhoneNumber); i+=2) {
        if (!isdigit(thePhoneNumber[i]) || !isdigit(thePhoneNumber[i+1])) {
            return ERR_INVALIDATE_PHONE;
        }
        high = thePhoneNumber[i] - '0';
        low = thePhoneNumber[i+1] - '0';
        
        binarySeq[phoneNumberIndex++] = ((high << 4) | low);
    }

    if (NULL != thePhoneNumber) {
        free(thePhoneNumber);
        thePhoneNumber = NULL;
    }
    return ERR_NONE;
}

/**
 * 将BCD码转换成终端手机号并写入phoneNumber字符数组中
 * @param binarySeq   待解析的二进制序列
 * @param phoneNumber 手机号字符串
 * @return JTT_ERROR     错误类型
 */
//  * @param binarySeqLen binarySeq数组长度
//  * @param phoneNumberLen phoneNumber数组长度
JTT_ERROR DecodePhoneNumber(const BYTE binarySeq[], char phoneNumber[]/*, const int binarySeqLen, const int phoneNumberLen*/) {
    //  0,1-消息ID; 2,3,-消息体属性； 4,5,6,7,8,9,-终端手机号
    int phoneNumberIndex = 4;
    BYTE high = 0, low = 0;
    phoneNumber[0] = '\0';
    char ch[2] = "";
    for (phoneNumberIndex = 4; phoneNumberIndex < 10; phoneNumberIndex++) {
        high = (binarySeq[phoneNumberIndex] & 0xf0) >> 4;
        low  = binarySeq[phoneNumberIndex] & 0x0f;
        ch[0] =  high + '0';
        strncat(phoneNumber, ch, 1);
        ch[0] =  low + '0';
        strncat(phoneNumber, ch, 1);
    }
    return ERR_NONE;
}

/**
 * 对crmb指向的通用应答消息体进行编码，写入binarySeq字节数组。
 * @param crmb       指向通用应答消息体的指针
 * @param binarySeq  要写入的二进制字节数组
 * @return JTT_ERROR     错误类型
 */
//  * @param len        binarySeq数组长度
JTT_ERROR EncodeForCRMB(const CommonRespMsgBody *crmb, BYTE binarySeq[]/*, const int len*/) {
    //  0,1-消息ID; 2,3-消息体属性； 4,5,6,7,8,9-终端手机号
    // 10,11,-消息流水号; 
    // 前提：有消息包封装项 =》12,13,14,15-消息包封装项; 
    // 16至检验码前一字节-消息体
    // 前提：无消息包封装项 12至检验码前一字节-消息体

    int startIndex = 12;
    // 消息体属性占两个字节，高字节的第6位是分包信息位
    // 如果有子包
    if ((binarySeq[2] >> 5) & 1) {
        startIndex = 16;
    }
    // 应答流水号 startIndex ~ startIndex + 1;
    binarySeq[startIndex] = (BYTE)((crmb->replyFlowId >> 8) & 0xff);
    binarySeq[startIndex + 1] = (BYTE)(crmb->replyFlowId & 0xff);

    //  应答 ID startIndex+2 ~ startIndex+3；
    binarySeq[startIndex + 2] = (BYTE)((crmb->replyId >> 8) & 0xff);
    binarySeq[startIndex + 3] = (BYTE)(crmb->replyId & 0xff);

    // 结果 startIndex+4
    binarySeq[startIndex + 4] = (BYTE)(crmb->replyCode & 0xff);

    return ERR_NONE;
}

/**
 * 对binarySeq字节数组进行编码，写入crmb指向的通用应答消息体
 * @param crmb      指向通用应答消息体的指针
 * @param binarySeq 要读取解码的二进制字节数组
 * @return JTT_ERROR     错误类型
 */
//  * @param len       binarySeq数组长度
JTT_ERROR DecodeForCRMB(CommonRespMsgBody *crmb, const BYTE binarySeq[]/*, const int len*/) {
    //  0,1-消息ID; 2,3-消息体属性； 4,5,6,7,8,9-终端手机号
    // 10,11,-消息流水号; 
    // 前提：有消息包封装项 =》12,13,14,15-消息包封装项; 
    // 16至检验码前一字节-消息体
    // 前提：无消息包封装项 12至检验码前一字节-消息体

    // int hasSubpackage = 0;
    int startIndex = 12;
    // 消息体属性占两个字节，高字节的第6位是分包信息位
    if ((binarySeq[2] >> 5) & 1) {
        startIndex = 16;
    }


    // 应答流水号 startIndex ~ startIndex + 1;
    crmb->replyFlowId = (((WORD)(binarySeq[startIndex])) << 8) + binarySeq[startIndex+1];

    //  应答 ID startIndex+2 ~ startIndex+3；
    crmb->replyId = ((WORD)(binarySeq[startIndex+2]) << 8)+ binarySeq[startIndex+3];

    // 结果 startIndex+4
    crmb->replyCode = (CommonReplyResult)binarySeq[startIndex + 4];

    return ERR_NONE;
}

/**
 * 对trmb指向的终端注册消息体进行编码，写入binarySeq字节数组
 * @param trmb      指向终端注册消息体的指针
 * @param binarySeq 要写入的二进制字节数组
 * @return JTT_ERROR     错误类型
 */
//  * @param len       binarySeq数组长度
JTT_ERROR EncodeForTRMB(const TerminalRegisterMsgBody *trmb, BYTE binarySeq[]/*, const int len*/) {
    int i = 0, index = 0;
    // searchForBodyStartIndex(binarySeq, &startIndex);
    int startIndex = 12;
    // 消息体属性占两个字节，高字节的第6位是分包信息位
    if ((binarySeq[2] >> 5) & 1) {
        startIndex = 16;
    }
    // ‭2   +   2   +   5   +   20   +   7   +   1   +   10 =‬ 47
    memset(binarySeq+startIndex, 0x00, 47);
    // startIndex~startIndex+1 -  省域 ID
    binarySeq[startIndex] = (BYTE)((trmb->provinceId >> 8) & 0xff);
    binarySeq[startIndex + 1] = (BYTE)(trmb->provinceId & 0xff);

    // startIndex+2~startIndex+3 -  市县域 ID 
    binarySeq[startIndex + 2] = (BYTE)((trmb->cityId >> 8) & 0xff);
    binarySeq[startIndex + 3] = (BYTE)(trmb->cityId & 0xff);

    // startIndex+4~startIndex+8 -  制造商 ID
    memcpy(binarySeq+startIndex+4, trmb->manufacturerId, 5);

    // startIndex+9~ startIndex+28 - 终端型号
    memcpy(binarySeq+startIndex+9, trmb->terminalType, 20);

    // startIndex+29 ~ startIndex+35 - 终端 ID
    memcpy(binarySeq+startIndex+29, trmb->terminalId, 7);
    
    // startIndex+36 - 车牌颜色
    binarySeq[startIndex+36] = (BYTE)(trmb->licensePlateColor & 0xff);

    // startIndex+37 - 车辆标识
    memcpy(binarySeq+startIndex+37, trmb->licensePlate, 10);
    return ERR_NONE;
}

/**
 * 对binarySeq字节数组进行编码，写入trmrb指向的终端注册应答消息体
 * @param trmrb     指向终端注册应答消息体的指针
 * @param binarySeq 要读取解码的二进制字节数组
 * @return JTT_ERROR    错误类型
 */
//  * @param len       binarySeq数组长度
JTT_ERROR DecodeForTRMRB(TerminalRegisterMsgRespBody *trmrb, const BYTE binarySeq[]/*, const int len*/) {
    int msgBodyLen = 0;
    int startIndex = 12;
    // 消息体属性占两个字节，高字节的第6位是分包信息位
    if ((binarySeq[2] >> 5) & 1) {
        startIndex = 16;
    }
    msgBodyLen = (int)(((binarySeq[2] & 0x03) << 8) + (binarySeq[3] & 0xff));

    // startIndex~startIndex+1 - 应答流水号
    trmrb->replyFlowId = (WORD)(((binarySeq[startIndex] << 8) & 0xff ) + (binarySeq[startIndex+1] & 0xff));

    // startIndex+2 - 结果

    trmrb->replyCode = (RegisterReplyResult)binarySeq[startIndex+2];
    memset(trmrb->replyToken, 0, 256);
    if (RRR_SUCCESS == trmrb->replyCode) {
        memcpy(trmrb->replyToken, &binarySeq[startIndex+3], msgBodyLen - 3);
    }
    return ERR_NONE;
}