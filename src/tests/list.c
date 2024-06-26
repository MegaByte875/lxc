/* list.c
 *
 * SPDX-License-Identifier: GPL-2.0-only
 *
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <lxc/lxccontainer.h>

static void test_list_func(const char *lxcpath, const char *type,
			   int (*func)(const char *path, char ***names,
				       struct lxc_container ***cret))
{
	int i, n, n2;
	struct lxc_container **clist;
	char **names;

	printf("%-10s Counting containers\n", type);

	n = func(lxcpath, NULL, NULL);
	printf("%-10s Counted %d containers\n", type, n);
	printf("%-10s Get container struct only\n", type);

	n2 = func(lxcpath, NULL, &clist);
	if (n2 != n)
		printf("Warning: first call returned %d, second %d\n", n, n2);

	for (i = 0; i < n2; i++) {
		struct lxc_container *c = clist[i];
		printf("%-10s  Got container struct %s\n", type, c->name);
		lxc_container_put(c);
	}

	if (n2 > 0) {
		free(clist);
		clist = NULL;
	}

	printf("%-10s Get names only\n", type);
	n2 = func(lxcpath, &names, NULL);
	if (n2 != n)
		printf("Warning: first call returned %d, second %d\n", n, n2);

	for (i = 0; i < n2; i++) {
		printf("%-10s  Got container name %s\n", type, names[i]);
		free(names[i]);
	}

	if (n2 > 0) {
		free(names);
		names = NULL;
	}

	printf("%-10s Get names and containers\n", type);
	n2 = func(lxcpath, &names, &clist);
	if (n2 != n)
		printf("Warning: first call returned %d, second %d\n", n, n2);

	for (i = 0; i < n2; i++) {
		struct lxc_container *c = clist[i];
		printf("%-10s  Got container struct %s, name %s\n", type, c->name, names[i]);

		if (strcmp(c->name, names[i]))
			fprintf(stderr, "ERROR: name mismatch!\n");

		free(names[i]);
		lxc_container_put(c);
	}

	if (n2 > 0) {
		free(names);
		free(clist);
	}
}

int main(int argc, char *argv[])
{
	const char *lxcpath = NULL;

	if (argc > 1)
		lxcpath = argv[1];

	test_list_func(lxcpath, "Defined:", list_defined_containers);
	test_list_func(lxcpath, "Active:", list_active_containers);
	test_list_func(lxcpath, "All:", list_all_containers);

	exit(EXIT_SUCCESS);
}
