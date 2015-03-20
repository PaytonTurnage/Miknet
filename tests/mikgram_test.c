#include <check.h>
#include <stdint.h>

#include "miknet/mikgram.h"
#include "miknet/mikdef.h"

START_TEST(test_create)
{
	char hello[] = "Hello";
	mikgram_t gram;

	/* Proper use. */
	ck_assert_int_eq(mikgram(&gram, hello, 6), MIKERR_NONE);
	ck_assert(gram.data != NULL);
	ck_assert(gram.len > 6);
	ck_assert_int_eq(((uint8_t *)gram.data)[0], 6);
	ck_assert_int_eq(((uint8_t *)gram.data)[1], 0);

	/* Bad inputs. */
	ck_assert_int_eq(mikgram(&gram, NULL, 6), MIKERR_BAD_PTR);
	ck_assert_int_eq(mikgram(NULL, hello, 6), MIKERR_BAD_PTR);
	ck_assert_int_eq(mikgram(&gram, hello, 0), MIKERR_BAD_LENGTH);
	ck_assert_int_eq(mikgram(&gram, hello, SIZE_MAX), MIKERR_GRAM_SIZE);

	mikgram_close(&gram);
}
END_TEST

START_TEST(test_gram_detection)
{
	char hello[] = "Hello";
	mikgram_t gram;

	mikgram(&gram, hello, 6);

	/* Proper use. */
	ck_assert_int_eq(mikgram_check(gram.data, 1024), 6);
	ck_assert_int_eq(mikgram_check(gram.data, 3), MIKERR_BAD_LENGTH);

	/* Bad inputs. */
	ck_assert_int_eq(mikgram_check(NULL, 10), MIKERR_BAD_PTR);

	mikgram_close(&gram);
}
END_TEST

START_TEST(test_gram_extraction)
{
	char hello[] = "Hello";
	char buffer[6] = {0};
	mikgram_t gram;

	mikgram(&gram, hello, 6);

	/* Proper use. */
	ck_assert_int_eq(	mikgram_extract(gram.data, gram.len, buffer, 6),
				MIKERR_NONE);
	ck_assert_int_eq(memcmp(buffer, hello, 6), 0);

	/* Bad inputs. */
	ck_assert_int_eq(	mikgram_extract(NULL, 10, buffer, 10),
				MIKERR_BAD_PTR);
	ck_assert_int_eq(	mikgram_extract(gram.data, gram.len, NULL, 10),
				MIKERR_BAD_PTR);
	ck_assert_int_eq(	mikgram_extract(gram.data, 0, buffer, 10),
				MIKERR_BAD_LENGTH);
	mikgram_close(&gram);
}
END_TEST

Suite *mikgram_suite()
{
	Suite *suite = suite_create("mikgram_suite");
	TCase *gram_units = tcase_create("mikgram_units");

	tcase_add_test(gram_units, test_create);
	tcase_add_test(gram_units, test_gram_detection);
	tcase_add_test(gram_units, test_gram_extraction);
	suite_add_tcase(suite, gram_units);

	return suite;
}

int main(int argc, char **argv)
{
	int failure_count;
	Suite *mikgram = mikgram_suite();
	SRunner *runner = srunner_create(mikgram);

	srunner_run_all(runner, CK_NORMAL);
	failure_count = srunner_ntests_failed(runner);
	srunner_free(runner);

	return failure_count;
}
