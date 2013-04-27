#include <check.h>
#include <stdlib.h>

Suite *uri_suite(void);
Suite *http_suite(void);
Suite *log_suite(void);

int run_suite(Suite *s)
{
    int number_failed;

    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    return number_failed;
}

int main(void)
{
    int number_failed;

    Suite *suri = uri_suite();
    Suite *shttp = http_suite();
    Suite *slog = log_suite();

    if (run_suite(suri)) return EXIT_FAILURE;
    if (run_suite(shttp)) return EXIT_FAILURE;
    if (run_suite(slog)) return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
