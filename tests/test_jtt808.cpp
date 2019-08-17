#include "catch.hpp"
#include <jtt808.h>
#include <cstdio>

static BOOL byteArrayEQ(const BYTE expect[], const BYTE given[], const int len) {
    int i = 0;
    char buffer[50];

    for (i = 0; i < len; i++) {
       
        if (expect[i] != given[i]) {
            sprintf(buffer, "expect: %x, given: %x\n", expect[i], given[i]);
            UNSCOPED_INFO(buffer);
            return FALSE;
        }
    }
    return TRUE;
}

TEST_CASE("DoEscapeForReceive - 0x7e, 0x7d Escape Test", "[DoEscapeForReceive]") {

    SECTION("failed to escape because too short") {
        BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x7e};
        REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE), 0) == ERR_LENGTH_TOO_SHORT);
    }

    SECTION("failed to escape because invalidate masseage with none indentifier") {
        BYTE raw[] = {0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE), 0) == ERR_INVALIDATE_MSG);
    }

    SECTION("failed to escape because invalidate masseage with one indentifier") {
        BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE), 0) == ERR_INVALIDATE_MSG);
    }

    SECTION("failed to escape because invalidate masseage with more than two indentifier") {
        BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e, 0x7e};
        REQUIRE(DoEscapeForReceive(raw, NULL, sizeof(raw)/sizeof(BYTE), 0) == ERR_INVALIDATE_MSG);
    }

    SECTION("0x7e escape") {
        BYTE expect[] = {0x30, 0x7e, 0x08, 0x70, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x70, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
        BYTE res[12] = {};
        DoEscapeForReceive(raw, res, sizeof(raw)/sizeof(BYTE), 12);
        REQUIRE(TRUE == byteArrayEQ(expect, res, 12));
    }

    SECTION("0x7d escape") {
        BYTE expect[] = {0x30, 0x08, 0x7d, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        BYTE raw[] = {0x7e, 0x30, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
        BYTE res[12] = {};
        DoEscapeForReceive(raw, res, sizeof(raw)/sizeof(BYTE), 12);
        REQUIRE(TRUE == byteArrayEQ(expect, res, 12));
    }

    SECTION("0x7e, 0x7d escape") {
        BYTE expect[] = {0x30, 0x7e, 0x08, 0x7d, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        BYTE raw[] = {0x7e, 0x30, 0x7d, 0x02, 0x08, 0x7d, 0x01, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7e};
        BYTE res[12] = {};
        DoEscapeForReceive(raw, res, sizeof(raw)/sizeof(BYTE), 12);
        REQUIRE(TRUE == byteArrayEQ(expect, res, 12));
    }
}