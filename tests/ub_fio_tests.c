#include <glib.h>
#include "ub_fio.h"



void test_fread_UbChain(){
	UbChain* chain = NULL;
	FILE* file = fopen("./tests/testfiles/test_write.chain", "r");
	g_assert_nonnull(file);
	enum UbIOErr err = fread_UbChain(&chain, file);
	g_assert_cmpint(err, ==, UB_IO_OK);
	g_assert_nonnull(chain);
	fclose(file);

	g_assert_cmpstr(chain->name, ==, "chain name");
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=1, .mon=3, .year=1990}).hardness,
			==, 1);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=2, .mon=3, .year=1990}).hardness,
			==, 0);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=4, .mon=3, .year=1990}).hardness,
			==, 0);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=5, .mon=3, .year=1990}).hardness,
			==, 10);

	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=1, .mon=3, .year=1990}).flags
			& UB_GHOST_LINK, ==, 0);

	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=2, .mon=3, .year=1990}).flags
			& UB_GHOST_LINK, !=, 0);

	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=3, .mon=3, .year=1990}).hardness,
			==, 0);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=3, .mon=3, .year=1990}).flags
			& UB_UNINIT_LINK, !=, 0);

	free_UbChain(chain);
}


void test_fwrite_UbChain(){
	UbChain* chain = new_UbChain("chain name");
	add_UbLink(chain,
		   (UbLink){.hardness=1, .flags=0},
		   (UbDate){.day=1, .mon=3, .year=1990});
	add_UbLink(chain,
		   (UbLink){.hardness=0, .flags=UB_GHOST_LINK},
		   (UbDate){.day=2, .mon=3, .year=1990});
	add_UbLink(chain,
		   (UbLink){.hardness=0, .flags=0},
		   (UbDate){.day=4, .mon=3, .year=1990});
	add_UbLink(chain,
		   (UbLink){.hardness=10, .flags=0},
		   (UbDate){.day=5, .mon=3, .year=1990});
	FILE* file = fopen("./tests/testfiles/test_write.chain", "w");
	g_assert_nonnull(file);
	fwrite_UbChain(chain, file);
	fclose(file);
	free_UbChain(chain);
	chain = 0;

	file = fopen("./tests/testfiles/test_write.chain", "r");
	g_assert_nonnull(file);
	enum UbIOErr err = fread_UbChain(&chain, file);
	g_assert_cmpint(err, ==, UB_IO_OK);
	g_assert_nonnull(chain);
	fclose(file);

	g_assert_cmpstr(chain->name, ==, "chain name");
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=1, .mon=3, .year=1990}).hardness,
			==, 1);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=2, .mon=3, .year=1990}).hardness,
			==, 0);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=4, .mon=3, .year=1990}).hardness,
			==, 0);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=5, .mon=3, .year=1990}).hardness,
			==, 10);

	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=2, .mon=3, .year=1990}).flags
			& UB_GHOST_LINK, !=, 0);

	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=3, .mon=3, .year=1990}).hardness,
			==, 0);
	g_assert_cmpint(get_UbLink(chain, (UbDate){.day=3, .mon=3, .year=1990}).flags
			& UB_UNINIT_LINK, !=, 0);

	free_UbChain(chain);
}


int main(int argc, char** argv){
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/set1/fread UbChain test", test_fread_UbChain);
	g_test_add_func("/set1/fwrite UbChain test", test_fwrite_UbChain);
	return g_test_run();

}
