#include <stdlib.h>
#include <check.h>
#include <string.h>
#include <stdio.h>
#include "../src/uri.h"
#include "common.h"

static void setup(void)
{
}

static void teardown(void)
{
}

void assert_uri(struct uri *uri, const char *uri_s, char *scheme, char *opt, 
        char *hostname, int port, char *path, char *query, char *fragment)
{
    char diag[4096];

#define assert_uri_field(uri, f, diag) assert_str_msg(uri->f, f, diag)

    snprintf(diag, 4096, 
        "%s -> s: %s o: %s h: %s p: %d pa: %s q: %s f: %s\n", 
        s(uri_s), s(uri->scheme), s(uri->authority.opt), 
        s(uri->authority.hostname), uri->authority.port, s(uri->path), 
        s(uri->query), s(uri->fragment));

    assert_uri_field(uri, scheme, diag);
    assert_uri_field((&(uri->authority)), opt, diag);
    assert_uri_field((&(uri->authority)), hostname, diag);
    assert_uri_field(uri, path, diag);
    assert_uri_field(uri, query, diag);
    assert_uri_field(uri, fragment, diag);
    fail_if(uri->authority.port != port, diag);
}

static void test_uri(const char *uri_s, char *scheme, char *opt, char *hostname, 
        int port, char *path, char *query, char *fragment)
{
    struct uri uri;

    memset(&uri, 0, sizeof(uri));
    if (uri_parse(&uri, uri_s) != 0) {
        fail(uri_s);
        return;
    }
    assert_uri(&uri, uri_s, scheme, opt, hostname, port, path, query, fragment);
    uri_destroy(&uri);
}

START_TEST(test_uri_parse_scheme)
{
    struct uri uri;

    ck_assert_int_eq(uri_parse(&uri, ":"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, ""), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http:"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http:/"), -1); uri_destroy(&uri);

    test_uri("h.t+tp://hostname", "h.t+tp", NULL, "hostname", 0, NULL, NULL, NULL);
} END_TEST 

START_TEST(test_uri_parse_authority)
{
    struct uri uri;
    int ret;

    ck_assert_int_eq(uri_parse(&uri, "http://[ho:st:name"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http://[]"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http:///"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http://?"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http://#"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http://:"), -1); uri_destroy(&uri);
    ck_assert_int_eq(uri_parse(&uri, "http://"), -1); uri_destroy(&uri);

    test_uri("http://[ho:st:name]", "http", NULL, "ho:st:name", 0, NULL, NULL, NULL);
    test_uri("http://[ho:st:name]:", "http", NULL, "ho:st:name", 0, NULL, NULL, NULL);
    test_uri("http://[ho:st:name]:81", "http", NULL, "ho:st:name", 81, NULL, NULL, NULL);
    test_uri("http://hostname", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    test_uri("http://hostname:", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    test_uri("http://hostname:81", "http", NULL, "hostname", 81, NULL, NULL, NULL);
    test_uri("http://@[ho:st:name]", "http", NULL, "ho:st:name", 0, NULL, NULL, NULL);
    test_uri("http://@[ho:st:name]:", "http", NULL, "ho:st:name", 0, NULL, NULL, NULL);
    test_uri("http://@[ho:st:name]:81", "http", NULL, "ho:st:name", 81, NULL, NULL, NULL);
    test_uri("http://@hostname", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    test_uri("http://@hostname:", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    test_uri("http://@hostname:81", "http", NULL, "hostname", 81, NULL, NULL, NULL);
    test_uri("http://user:pwd$something@hostname", "http", "user:pwd$something", "hostname", 0, NULL, NULL, NULL);
    test_uri("http://user:pwd$something@hostname:", "http", "user:pwd$something", "hostname", 0, NULL, NULL, NULL);
    test_uri("http://user:pwd$something@hostname:81", "http", "user:pwd$something", "hostname", 81, NULL, NULL, NULL);
    test_uri("http://user:pwd$something@[ho:st:name]", "http", "user:pwd$something", "ho:st:name", 0, NULL, NULL, NULL);
    test_uri("http://user:pwd$something@[ho:st:name]:", "http", "user:pwd$something", "ho:st:name", 0, NULL, NULL, NULL);
} END_TEST 

START_TEST(test_uri_parse_path)
{
    test_uri("http://hostname/", "http", NULL, "hostname", 0, "/", NULL, NULL);
    test_uri("http://hostname/a", "http", NULL, "hostname", 0, "/a", NULL, NULL);
    test_uri("http://hostname/a/", "http", NULL, "hostname", 0, "/a/", NULL, NULL);
    test_uri("http://hostname/a/b", "http", NULL, "hostname", 0, "/a/b", NULL, NULL);
    test_uri("http://hostname/a/b/", "http", NULL, "hostname", 0, "/a/b/", NULL, NULL);
    test_uri("http://hostname/a/b/c", "http", NULL, "hostname", 0, "/a/b/c", NULL, NULL);
    test_uri("http://hostname/a/b/c/", "http", NULL, "hostname", 0, "/a/b/c/", NULL, NULL);
    test_uri("http://hostname/res.ext", "http", NULL, "hostname", 0, "/res.ext", NULL, NULL);
    test_uri("http://hostname/a/res.ext", "http", NULL, "hostname", 0, "/a/res.ext", NULL, NULL);
    test_uri("http://hostname/a/b/res.ext", "http", NULL, "hostname", 0, "/a/b/res.ext", NULL, NULL);
    test_uri("http://hostname/a/b/c/res.ext", "http", NULL, "hostname", 0, "/a/b/c/res.ext", NULL, NULL);

} END_TEST 

START_TEST(test_uri_parse_query)
{
    test_uri("http://hostname?", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    test_uri("http://hostname?a=b&b=c", "http", NULL, "hostname", 0, NULL, "a=b&b=c", NULL);

} END_TEST 

START_TEST(test_uri_parse_fragment)
{
    test_uri("http://hostname#", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    test_uri("http://hostname#frag", "http", NULL, "hostname", 0, NULL, NULL, "frag");

} END_TEST 

START_TEST(test_uri_parse_mix)
{
    test_uri("http://[ho:st:name]/", "http", NULL, "ho:st:name", 0, "/", NULL, NULL);
    test_uri("http://[ho:st:name]:/", "http", NULL, "ho:st:name", 0, "/", NULL, NULL);
    test_uri("http://[ho:st:name]:81/", "http", NULL, "ho:st:name", 81, "/", NULL, NULL);

    test_uri("http://[ho:st:name]/a/b", "http", NULL, "ho:st:name", 0, "/a/b", NULL, NULL);
    test_uri("http://[ho:st:name]:/a/b", "http", NULL, "ho:st:name", 0, "/a/b", NULL, NULL);
    test_uri("http://[ho:st:name]:81/a//b", "http", NULL, "ho:st:name", 81, "/a//b", NULL, NULL);

    test_uri("http://[ho:st:name]/a/b?", "http", NULL, "ho:st:name", 0, "/a/b", NULL, NULL);
    test_uri("http://[ho:st:name]:/a/b?", "http", NULL, "ho:st:name", 0, "/a/b", NULL, NULL);
    test_uri("http://[ho:st:name]:81/a//b?", "http", NULL, "ho:st:name", 81, "/a//b", NULL, NULL);

    test_uri("http://[ho:st:name]/a/b?query", "http", NULL, "ho:st:name", 0, "/a/b", "query", NULL);
    test_uri("http://[ho:st:name]:/a/b?query", "http", NULL, "ho:st:name", 0, "/a/b", "query", NULL);
    test_uri("http://[ho:st:name]:81/a//b?query", "http", NULL, "ho:st:name", 81, "/a//b", "query", NULL);

    test_uri("http://[ho:st:name]/a/b#", "http", NULL, "ho:st:name", 0, "/a/b", NULL, NULL);
    test_uri("http://[ho:st:name]/a/b#frag", "http", NULL, "ho:st:name", 0, "/a/b", NULL, "frag");
    test_uri("http://[ho:st:name]:/a/b#frag", "http", NULL, "ho:st:name", 0, "/a/b", NULL, "frag");
    test_uri("http://[ho:st:name]:81/a//b#frag", "http", NULL, "ho:st:name", 81, "/a//b", NULL, "frag");

    test_uri("http://[ho:st:name]/a/b?query#", "http", NULL, "ho:st:name", 0, "/a/b", "query", NULL);
    test_uri("http://[ho:st:name]/a/b?query#frag", "http", NULL, "ho:st:name", 0, "/a/b", "query", "frag");
    test_uri("http://[ho:st:name]:/a/b?query#frag", "http", NULL, "ho:st:name", 0, "/a/b", "query", "frag");
    test_uri("http://[ho:st:name]:81/a//b?query#frag", "http", NULL, "ho:st:name", 81, "/a//b", "query", "frag");

    test_uri("http://[ho:st:name]?#", "http", NULL, "ho:st:name", 0, NULL, NULL, NULL);
    test_uri("http://[ho:st:name]/?#", "http", NULL, "ho:st:name", 0, "/", NULL, NULL);

    test_uri("http://hostname/", "http", NULL, "hostname", 0, "/", NULL, NULL);
    test_uri("http://hostname:/", "http", NULL, "hostname", 0, "/", NULL, NULL);
    test_uri("http://hostname:81/", "http", NULL, "hostname", 81, "/", NULL, NULL);

    test_uri("http://hostname/a/b", "http", NULL, "hostname", 0, "/a/b", NULL, NULL);
    test_uri("http://hostname:/a/b", "http", NULL, "hostname", 0, "/a/b", NULL, NULL);
    test_uri("http://hostname:81/a//b", "http", NULL, "hostname", 81, "/a//b", NULL, NULL);

    test_uri("http://hostname/a/b?", "http", NULL, "hostname", 0, "/a/b", NULL, NULL);
    test_uri("http://hostname:/a/b?", "http", NULL, "hostname", 0, "/a/b", NULL, NULL);
    test_uri("http://hostname:81/a//b?", "http", NULL, "hostname", 81, "/a//b", NULL, NULL);

    test_uri("http://hostname/a/b?query", "http", NULL, "hostname", 0, "/a/b", "query", NULL);
    test_uri("http://hostname:/a/b?query", "http", NULL, "hostname", 0, "/a/b", "query", NULL);
    test_uri("http://hostname:81/a//b?query", "http", NULL, "hostname", 81, "/a//b", "query", NULL);

    test_uri("http://hostname/a/b#", "http", NULL, "hostname", 0, "/a/b", NULL, NULL);
    test_uri("http://hostname/a/b#frag", "http", NULL, "hostname", 0, "/a/b", NULL, "frag");
    test_uri("http://hostname:/a/b#frag", "http", NULL, "hostname", 0, "/a/b", NULL, "frag");
    test_uri("http://hostname:81/a//b#frag", "http", NULL, "hostname", 81, "/a//b", NULL, "frag");

    test_uri("http://hostname/a/b?query#", "http", NULL, "hostname", 0, "/a/b", "query", NULL);
    test_uri("http://hostname/a/b?query#frag", "http", NULL, "hostname", 0, "/a/b", "query", "frag");
    test_uri("http://hostname:/a/b?query#frag", "http", NULL, "hostname", 0, "/a/b", "query", "frag");
    test_uri("http://hostname:81/a//b?query#frag", "http", NULL, "hostname", 81, "/a//b", "query", "frag");

    test_uri("http://hostname?#", "http", NULL, "hostname", 0, NULL, NULL, NULL);
    test_uri("http://hostname/?#", "http", NULL, "hostname", 0, "/", NULL, NULL);

} END_TEST 

Suite *uri_suite(void)
{
    Suite *s = suite_create("uri");

    /* Core test case */
    TCase *tc_core = tcase_create("Core");
    tcase_add_checked_fixture(tc_core, setup, teardown);
    tcase_add_test(tc_core, test_uri_parse_authority);
    tcase_add_test(tc_core, test_uri_parse_scheme);
    tcase_add_test(tc_core, test_uri_parse_path);
    tcase_add_test(tc_core, test_uri_parse_query);
    tcase_add_test(tc_core, test_uri_parse_fragment);
    tcase_add_test(tc_core, test_uri_parse_mix);
    suite_add_tcase(s, tc_core);

    return s;
}
