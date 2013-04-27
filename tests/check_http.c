#include <stdlib.h>
#include <check.h>
#include <string.h>
#include <stdio.h>
#include "../src/http.h"
#include "common.h"

static void setup(void)
{
}

static void teardown(void)
{
}

#define STATUS_LINE_ASSERT_FAIL(l, s) \
    fail_if(http_status_line_parse(&(l), s) != -1);

#define REQUEST_LINE_ASSERT_FAIL(l, s) \
    fail_if(http_request_line_parse(&(l), s) != -1);

START_TEST(test_http_status_line_parse)
{
    struct http_status_line sline;


    STATUS_LINE_ASSERT_FAIL(sline, "   ");
    STATUS_LINE_ASSERT_FAIL(sline, "   \r\n");
    STATUS_LINE_ASSERT_FAIL(sline, "   \r\n  ");
    STATUS_LINE_ASSERT_FAIL(sline, "InvalidVersion 200 reason msg");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1 ");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1 INVALIDCODE");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1 INVALIDCODE ");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1 200");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1 200 ");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1 INVALIDCODE reasonmsg\r\n");
    STATUS_LINE_ASSERT_FAIL(sline, "HTTP/1.1 200 reason msg");

    fail_unless(http_status_line_parse(&sline, "HTTP/2.1 200 reason msg\r\n") == 0);
    ck_assert_int_eq(sline.version.major, 2);
    ck_assert_int_eq(sline.version.minor, 1);
    ck_assert_int_eq(sline.code, 200);
    assert_str(sline.reason, "reason msg");
    http_status_line_destroy(&sline);

} END_TEST

START_TEST(test_http_request_line_parse) 
{
    struct http_request_line rline;

    REQUEST_LINE_ASSERT_FAIL(rline, "");
    REQUEST_LINE_ASSERT_FAIL(rline, "   ");
    REQUEST_LINE_ASSERT_FAIL(rline, "   \r\n");
    REQUEST_LINE_ASSERT_FAIL(rline, "   \r\n  ");
    REQUEST_LINE_ASSERT_FAIL(rline, " http://hostname HTTP/1.1\r\n");
    REQUEST_LINE_ASSERT_FAIL(rline, "Invalid()Method http://hostname HTTP/1.1\r\n");
    REQUEST_LINE_ASSERT_FAIL(rline, "GET http://hostname Invalid version\r\n");
    REQUEST_LINE_ASSERT_FAIL(rline, "METHOD http://hostname HTTP/2.1");
    REQUEST_LINE_ASSERT_FAIL(rline, "METHOD ");
    REQUEST_LINE_ASSERT_FAIL(rline, "METHOD http://hostname");

    fail_unless(http_request_line_parse(&rline, "METHOD http://hostname HTTP/2.1\r\n") == 0);
    assert_str_msg(rline.method, "METHOD", rline.method);
    assert_uri(&rline.uri, "http://hostname", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    ck_assert_int_eq(rline.version.major, 2);
    ck_assert_int_eq(rline.version.minor, 1);
    http_request_line_destroy(&rline);
} END_TEST

/* TODO: find two headers with coliding hash and test it */
START_TEST(test_http_headers)
{
    http_hdr_table *t;

    t = http_headers_init();

    fail_if(http_header_parse(t, "") == 0);
    fail_if(http_header_parse(t, "field") == 0);
    fail_if(http_header_parse(t, "field\r\n") == 0);
    fail_if(http_header_parse(t, "field:") == 0);
    fail_if(http_header_parse(t, "field:\r\n") == 0);
    fail_if(http_header_parse(t, "field:value") == 0);
    fail_if(http_header_parse(t, ":value") == 0);
    fail_if(http_header_parse(t, ":value\r\n") == 0);

    fail_unless(http_header_parse(t, "Host: dsl.sk\r\n") == 0);
    assert_str(http_header_get(t, "Host"), "dsl.sk");
    assert_str(http_header_get(t, "host"), "dsl.sk");
    assert_str(http_header_get(t, "HOST"), "dsl.sk");

    http_headers_destroy(t);
} END_TEST

Suite *http_suite(void)
{
    Suite *s = suite_create("http");

    TCase *tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_http_request_line_parse);
    tcase_add_test(tc_core, test_http_status_line_parse);
    tcase_add_test(tc_core, test_http_headers);
    suite_add_tcase(s, tc_core);

    return s;
}
