/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#include "strmap.h"

#define kmalloc git__malloc
#define kcalloc git__calloc
#define krealloc git__realloc
#define kreallocarray git__reallocarray
#define kfree git__free
#include "khash.h"

__KHASH_TYPE(str, const char *, void *)

__KHASH_IMPL(str, static kh_inline, const char *, void *, 1, kh_str_hash_func, kh_str_hash_equal)

int git_strmap_new(git_strmap **out)
{
	*out = kh_init(str);
	GIT_ERROR_CHECK_ALLOC(*out);

	return 0;
}

void git_strmap_free(git_strmap *map)
{
	kh_destroy(str, map);
}

void git_strmap_clear(git_strmap *map)
{
	kh_clear(str, map);
}

size_t git_strmap_size(git_strmap *map)
{
	return kh_size(map);
}

void *git_strmap_get(git_strmap *map, const char *key)
{
	size_t idx = git_strmap_lookup_index(map, key);
	if (!git_strmap_valid_index(map, idx) ||
	    !git_strmap_has_data(map, idx))
		return NULL;
	return kh_val(map, idx);
}

int git_strmap_set(git_strmap *map, const char *key, void *value)
{
	size_t idx;
	int rval;

	idx = kh_put(str, map, key, &rval);
	if (rval < 0)
		return -1;

	if (rval == 0)
		kh_key(map, idx) = key;

	kh_val(map, idx) = value;

	return 0;
}

int git_strmap_delete(git_strmap *map, const char *key)
{
	khiter_t idx = git_strmap_lookup_index(map, key);
	if (!git_strmap_valid_index(map, idx))
		return GIT_ENOTFOUND;
	git_strmap_delete_at(map, idx);
	return 0;
}

int git_strmap_exists(git_strmap *map, const char *key)
{
	return kh_get(str, map, key) != kh_end(map);
}

size_t git_strmap_lookup_index(git_strmap *map, const char *key)
{
	return kh_get(str, map, key);
}

int git_strmap_valid_index(git_strmap *map, size_t idx)
{
	return idx != kh_end(map);
}

int git_strmap_has_data(git_strmap *map, size_t idx)
{
	return kh_exist(map, idx);
}

const char *git_strmap_key(git_strmap *map, size_t idx)
{
	return kh_key(map, idx);
}

void git_strmap_set_key_at(git_strmap *map, size_t idx, char *key)
{
	kh_val(map, idx) = key;
}

void *git_strmap_value_at(git_strmap *map, size_t idx)
{
	return kh_val(map, idx);
}

void git_strmap_set_value_at(git_strmap *map, size_t idx, void *value)
{
	kh_val(map, idx) = value;
}

void git_strmap_delete_at(git_strmap *map, size_t idx)
{
	kh_del(str, map, idx);
}

int git_strmap_put(git_strmap *map, const char *key, int *err)
{
	return kh_put(str, map, key, err);
}

void git_strmap_insert(git_strmap *map, const char *key, void *value, int *rval)
{
	khiter_t idx = kh_put(str, map, key, rval);

	if ((*rval) >= 0) {
		if ((*rval) == 0)
			kh_key(map, idx) = key;
		kh_val(map, idx) = value;
	}
}

size_t git_strmap_begin(git_strmap *map)
{
	GIT_UNUSED(map);
	return 0;
}

size_t git_strmap_end(git_strmap *map)
{
	return map->n_buckets;
}

int git_strmap_next(
	void **data,
	size_t* iter,
	git_strmap *map)
{
	if (!map)
		return GIT_ERROR;

	while (*iter != git_strmap_end(map)) {
		if (!(git_strmap_has_data(map, *iter))) {
			++(*iter);
			continue;
		}

		*data = git_strmap_value_at(map, *iter);

		++(*iter);

		return GIT_OK;
	}

	return GIT_ITEROVER;
}
