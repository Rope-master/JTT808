#define BOOST_TEST_MODULE JTT808_TEST
#include <boost/test/included/unit_test.hpp>
#include <jtt808.h>

namespace tt = boost::test_tools;

BOOST_AUTO_TEST_SUITE(DoEscapeForReceive_0x7e_0x7d_Escape_Test_Suite)
BOOST_AUTO_TEST_CASE(failed_to_escape_because_too_short)
{
    BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x7e};
    BOOST_TEST_REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE)) == ERR_LENGTH_TOO_SHORT);
}

BOOST_AUTO_TEST_CASE(failed_to_escape_because_invalidate_masseage_with_none_indentifier)
{
    BYTE raw[] = {0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BOOST_TEST_REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE)) == ERR_INVALIDATE_MSG);
}

BOOST_AUTO_TEST_CASE(failed_to_escape_because_invalidate_masseage_with_one_indentifier)
{
    BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BOOST_TEST_REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE)) == ERR_INVALIDATE_MSG);
}

BOOST_AUTO_TEST_CASE(failed_to_escape_because_invalidate_masseage_with_more_than_two_indentifier)
{
    BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e};
    BOOST_TEST_REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE)) == ERR_INVALIDATE_MSG);
}

BOOST_AUTO_TEST_CASE(_0x7e_escape)
{
    BYTE expect[] = {0x30, 0x7e, 0x08, 0x70, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x70, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
    BYTE res[12] = {};
    DoEscapeForReceive(raw, res, sizeof(raw)/sizeof(BYTE));
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}

BOOST_AUTO_TEST_CASE(_0x7d_escape)
{
    BYTE expect[] = {0x30, 0x08, 0x7d, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE raw[] = {0x7e, 0x30, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
    BYTE res[12] = {};
    DoEscapeForReceive(raw, res, sizeof(raw)/sizeof(BYTE));
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}

BOOST_AUTO_TEST_CASE(_0x7e_0x7d_escape)
{
    BYTE expect[] = {0x30, 0x7e, 0x08, 0x7d, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
    BYTE res[12] = {};
    DoEscapeForReceive(raw, res, sizeof(raw)/sizeof(BYTE));
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(DoEscapeForSend_Test)
BOOST_AUTO_TEST_CASE(failed_to_escape_because_too_short)
{
    BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x7e};
    BOOST_TEST_REQUIRE(DoEscapeForSend(raw, NULL, sizeof(raw)/sizeof(BYTE), 0) == ERR_LENGTH_TOO_SHORT);
}

BOOST_AUTO_TEST_CASE(_0x7e_escape)
{
    BYTE raw[] = {0x30, 0x7e, 0x08, 0x70, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE expect[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x70, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
    BYTE res[15] = {};
    DoEscapeForSend(raw, res, sizeof(raw)/sizeof(BYTE), sizeof(expect)/sizeof(BYTE));
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}

BOOST_AUTO_TEST_CASE(_0x7d_escape)
{
    BYTE raw[] = {0x30, 0x08, 0x7d, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE expect[] = {0x7e, 0x30, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
    BYTE res[15] = {};
    DoEscapeForSend(raw, res, sizeof(raw)/sizeof(BYTE), sizeof(expect)/sizeof(BYTE));
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}

BOOST_AUTO_TEST_CASE(_0x7e_0x7d_escape)
{
    BYTE raw[] = {0x30, 0x7e, 0x08, 0x7d, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
    BYTE expect[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
    BYTE res[16] = {};
    DoEscapeForSend(raw, res, sizeof(raw)/sizeof(BYTE), sizeof(expect)/sizeof(BYTE));
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(EncodePhoneNumber_Test)
BOOST_AUTO_TEST_CASE(should_encode_as_expect1)
{
    BYTE res[10] = {};
    JTT_ERROR err = ERR_NONE;
    BYTE expect[10] = {0x0, 0x0, 0x0, 0x0, /*01*/ 0x01, /*52*/ 0x52, /*77*/ 0x77,
        /*36*/0x36, /*25*/0x25, /*81*/ 0x81
    };

    err = EncodePhoneNumber(res, "15277362581");
    BOOST_TEST_REQUIRE(err == ERR_NONE);
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}

BOOST_AUTO_TEST_CASE(should_encode_as_expect2)
{
    BYTE res[10] = {};
    JTT_ERROR err = ERR_NONE;
    BYTE expect[10] = {0x0, 0x0, 0x0,0x0, /*01*/ 0x01, /*52*/ 0x52, /*77*/ 0x77,
        /*36*/0x36, /*25*/0x25, /*81*/ 0x81
    };
    BOOST_TEST_REQUIRE(err == ERR_NONE);
    EncodePhoneNumber(res, "015277362581");
    BOOST_TEST_REQUIRE(expect == res, tt::per_element());
}

BOOST_AUTO_TEST_CASE(failed_because_invalidate_phone_number)
{
    JTT_ERROR err = ERR_NONE;
    err = EncodePhoneNumber(NULL, "1");
    BOOST_TEST_REQUIRE(err == ERR_INVALIDATE_PHONE);

    err = ERR_NONE;
    err = EncodePhoneNumber(NULL, "");
    BOOST_TEST_REQUIRE(err == ERR_INVALIDATE_PHONE);

    err = ERR_NONE;
    err = EncodePhoneNumber(NULL, "152773625810000000000");
    BOOST_TEST_REQUIRE(err == ERR_INVALIDATE_PHONE);

    err = ERR_NONE;
    err = EncodePhoneNumber(NULL, "ffgfd165.48-");
    BOOST_TEST_REQUIRE(err == ERR_INVALIDATE_PHONE);
}
BOOST_AUTO_TEST_SUITE_END()

BOOST_AUTO_TEST_SUITE(DecodePhoneNumber_Test)
BOOST_AUTO_TEST_CASE(_should_decode_as_expect)
{
    BYTE raw[10] = {0x0, 0x0, 0x0,0x0, /*01*/ 0x01, /*52*/ 0x52, /*77*/ 0x77,
        /*36*/0x36, /*25*/0x25, /*81*/ 0x81
    };
    char res[13];
    const char *exp = "015277362581";
    JTT_ERROR err = ERR_NONE;
    err = DecodePhoneNumber(raw, res);
    BOOST_TEST_REQUIRE(err == ERR_NONE);
    BOOST_TEST_REQUIRE(res[12] == '\0');
    BOOST_TEST_REQUIRE(exp == res, "expect: " << exp << ", but res: " << res);
}
BOOST_AUTO_TEST_SUITE_END()


BOOST_AUTO_TEST_SUITE(EncodeForCRMB_Test)
    //  0,1-消息ID; 2,3-消息体属性； 4,5,6,7,8,9-终端手机号
    // 10,11,-消息流水号; 
    // 前提：有消息包封装项 =》12,13,14,15-消息包封装项; 
    // 16至检验码前一字节-消息体
    // 前提：无消息包封装项 12至检验码前一字节-消息体
BOOST_AUTO_TEST_CASE(_should_encode_as_expect_without_subpackage1)
{
    CommonRespMsgBody crmb;
    BYTE binarySeq[20] = {0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 
                        //  10， 11，  12， 13， 14
                            0x0, 0x0, 0x0, 0x0, 0x0,
                        //  15，  16  , 17 , 18, 19
                            0x0, 0x0, 0x0, 0x0, 0x0};

    BYTE exp[20] = {0x0, 0x0, 0x0, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0, 
                //  10， 11，  12， 13， 14
                    0x0, 0x0, 0x0, 0x1, 0x0,
                    0x1};
    crmb.replyFlowId = 1;
    crmb.replyId     = 1;
    crmb.replyCode   = CRR_SUCCESS;
    EncodeForCRMB(&crmb,  binarySeq);
    BOOST_TEST_REQUIRE(exp == binarySeq, tt::per_element());

}
BOOST_AUTO_TEST_CASE(_should_encode_as_expect_without_subpackage2)
{
    CommonRespMsgBody crmb;
    BYTE binarySeq[20] = {0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 
                        //  10， 11，  12， 13， 14
                            0x0, 0x0, 0x0, 0x0, 0x0,
                        //  15，  16  , 17 , 18, 19
                            0x0, 0x0, 0x0, 0x0, 0x0};
    BYTE exp[20] = {0x0, 0x0, 0x0, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0, 
                //  10， 11，  12， 13， 14
                    0x0, 0x0, 0x0, 0x64, 0x0,
                    0x64, 0x1};
    crmb.replyFlowId = 100;
    crmb.replyId     = 100;
    crmb.replyCode   = CRR_FAILED;
    EncodeForCRMB(&crmb,  binarySeq);
    BOOST_TEST_REQUIRE(exp == binarySeq, tt::per_element());

}
BOOST_AUTO_TEST_CASE(_should_encode_as_expect_without_subpackage3)
{
    CommonRespMsgBody crmb;
    BYTE binarySeq[20] = {0x0, 0x0, 0x0, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 
                        //  10， 11，  12， 13， 14
                            0x0, 0x0, 0x0, 0x0, 0x0,
                        //  15，  16  , 17 , 18, 19
                            0x0, 0x0, 0x0, 0x0, 0x0};
    BYTE exp[20] = {0x0, 0x0, 0x0, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0, 
                    0x0, 0x0, 0x0, 0x64, 0xff,
                    0xff, 0x1};
    crmb.replyFlowId = 100;
    crmb.replyId     = 65535;
    crmb.replyCode   = CRR_FAILED;
    EncodeForCRMB(&crmb,  binarySeq);
    BOOST_TEST_REQUIRE(exp == binarySeq, tt::per_element());

}

BOOST_AUTO_TEST_CASE(_should_encode_as_expect_with_subpackage1)
{
    CommonRespMsgBody crmb;

    BYTE binarySeq[30] = {0x0, 0x0, 0x24, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 
                        //  10， 11，  12， 13， 14
                            0x0, 0x0, 0x0, 0x0, 0x0,
                        //  15，  16  , 17 , 18, 19
                            0x0, 0x0, 0x0, 0x0, 0x0};

    BYTE exp[30] = {0x0, 0x0, 0x24, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0,
                //  15，  16  , 17 , 18, 19
                    0x0, 0x0, 0x1, 0x0, 0x1};
    crmb.replyFlowId = 1;
    crmb.replyId     = 1;
    crmb.replyCode   = CRR_SUCCESS;
    EncodeForCRMB(&crmb,  binarySeq);
    BOOST_TEST_REQUIRE(exp == binarySeq, tt::per_element());

}

BOOST_AUTO_TEST_CASE(_should_encode_as_expect_with_subpackage2)
{
    CommonRespMsgBody crmb;
    BYTE binarySeq[30] = {0x0, 0x0, 0x24, 0x0, 0x0,
                            0x0, 0x0, 0x0, 0x0, 0x0, 
                        //  10， 11，  12， 13， 14
                            0x0, 0x0, 0x0, 0x0, 0x0,
                        //  15，  16  , 17 , 18, 19
                            0x0, 0x0, 0x0, 0x0, 0x0};
    BYTE exp[30] = {0x0, 0x0, 0x24, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0,
                    0x0, 0x0, 0x0, 0x0, 0x0,
                //  15，  16  , 17 , 18, 19
                    0x0, 0x0, 0x64, 0xff, 0xff};
    crmb.replyFlowId = 100;
    crmb.replyId     = 65535;
    crmb.replyCode   = CRR_SUCCESS;
    EncodeForCRMB(&crmb,  binarySeq);
    BOOST_TEST_REQUIRE(exp == binarySeq, tt::per_element());

}

BOOST_AUTO_TEST_SUITE_END()

