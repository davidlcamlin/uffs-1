/*
    Copyright (C) 2005-2008  Ricky Zheng <ricky_gz_zheng@yahoo.co.nz>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/
/**
 * \file helper_cmds.c
 * \brief helper commands for test uffs
 * \author Ricky Zheng
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "uffs/uffs_config.h"
#include "uffs/uffs_public.h"
#include "uffs/uffs_fs.h"
#include "uffs/uffs_utils.h"
#include "uffs/uffs_core.h"
#include "cmdline.h"

#define PFX "cmd:  "

BOOL cmdFormat(const char *tail)
{
	URET ret;
	const char *mount = "/";
	uffs_Device *dev;

	if (tail) {
		mount = cli_getparam(tail, NULL);
	}
	uffs_Perror(UFFS_ERR_NORMAL, PFX"Formating %s ... ", mount);

	dev = uffs_GetDevice(mount);
	if (dev == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't get device from mount point.\n");
	}
	else {
		ret = uffs_FormatDevice(dev);
		if (ret != U_SUCC) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Format fail.\n");
		}
		else {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Format succ.\n");
		}
	}
	return TRUE;	
}

BOOL cmdMkf(const char *tail)
{
	uffs_Object *f;
	const char *name;
	URET ret;
	int oflags = UO_RDWR | UO_CREATE;

	if (tail == NULL) {
		return FALSE;
	}

	name = cli_getparam(tail, NULL);
	
	f = uffs_GetObject();
	if (f == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Fail to get object.\n");
		return TRUE;
	}

	ret = uffs_CreateObject(f, name, oflags, US_IREAD|US_IWRITE);
	if (ret == U_FAIL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Create %s fail, err: %d\n", name, f->err);
		uffs_PutObject(f);
		return TRUE;
	}
	
	uffs_Perror(UFFS_ERR_NORMAL, PFX"Create %s succ.\n", name);
	uffs_CloseObject(f);
	uffs_PutObject(f);
	
	return TRUE;
}

BOOL cmdMkdir(const char *tail)
{
	uffs_Object *f;
	const char *name;
	URET ret;
	int oflags = UO_RDWR | UO_CREATE | UO_DIR;

	if (tail == NULL) {
		return FALSE;
	}

	name = cli_getparam(tail, NULL);
	
	f = uffs_GetObject();
	if (f == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Fail to get object.\n");
		return TRUE;
	}

	ret = uffs_CreateObject(f, name, oflags, US_IREAD|US_IWRITE);
	if (ret == U_FAIL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Create %s fail, err: %d\n", name, f->err);
		uffs_PutObject(f);
		return TRUE;
	}
	
	uffs_Perror(UFFS_ERR_NORMAL, PFX"Create %s succ.\n", name);
	uffs_CloseObject(f);
	uffs_PutObject(f);
	
	return TRUE;
}


static int CountObjectUnder(const char *dir)
{
	uffs_FindInfo find = {0};
	int count = 0;
	URET ret;

	ret = uffs_OpenFindObject(&find, dir);
	if (ret == U_SUCC) {
		ret = uffs_FindFirstObject(NULL, &find);
		while(ret == U_SUCC) {
			count++;
			ret = uffs_FindNextObject(NULL, &find);
		}
		uffs_CloseFindObject(&find);
	}

	return count;
}

BOOL cmdPwd(const char *tail)
{
	uffs_Perror(UFFS_ERR_NORMAL, PFX"not supported since v1.2.0.\n");
	return TRUE;
}

BOOL cmdCd(const char *tail)
{
	uffs_Perror(UFFS_ERR_NORMAL, PFX"Not supported since v1.2.0\n");
	return TRUE;
}

BOOL cmdLs(const char *tail)
{
	uffs_FindInfo find = {0};
	uffs_ObjectInfo info;
	URET ret;
	int count = 0;
	char buf[MAX_PATH_LENGTH+2];
	char *name = (char *)tail;
	char *sub;

	if (name == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Must provide file/dir name.\n");
		return FALSE;
	}

	ret = uffs_OpenFindObject(&find, name);
	if (ret == U_FAIL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open '%s'\n", name);
		return TRUE;
	}

	uffs_Perror(UFFS_ERR_NORMAL, "------name----size----serial--\n");
	ret = uffs_FindFirstObject(&info, &find);
	while(ret == U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, "%9s", info.info.name);
		if (info.info.attr & FILE_ATTR_DIR) {
			strcpy(buf, name);
			sub = buf;
			if (name[strlen(name)-1] != '/') sub = strcat(buf, "/");
			sub = strcat(sub, info.info.name);
			sub = strcat(sub, "/");
			uffs_Perror(UFFS_ERR_NORMAL, "/  \t<%2d>\t", CountObjectUnder(sub));
		}
		else {
			uffs_Perror(UFFS_ERR_NORMAL, "   \t %2d \t", info.len);
		}
		uffs_Perror(UFFS_ERR_NORMAL, "%d\n", info.serial);
		count++;
		ret = uffs_FindNextObject(&info, &find);
	}
	
	uffs_CloseFindObject(&find);

	uffs_Perror(UFFS_ERR_NORMAL, "Total: %d objects.\n", count);

	return TRUE;
}

BOOL cmdRm(const char *tail)
{
	const char *name = NULL;
	if (tail == NULL) return FALSE;
	name = cli_getparam(tail, NULL);
	if (uffs_DeleteObject(name) == U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Delete '%s' succ.\n", name);
	}
	else {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Delete '%s' fail!\n", name);
	}
	return TRUE;
}

BOOL cmdRen(const char *tail)
{
	const char *oldname;
	char *newname;

	if (tail == NULL) return FALSE;
	oldname = cli_getparam(tail, &newname);
	if (newname == NULL) return FALSE;
	if (uffs_RenameObject(oldname, newname) == U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Rename from '%s' to '%s' succ.\n", oldname, newname);
	}
	else {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Rename from '%s' to '%s' fail!\n", oldname, newname);
	}
	return TRUE;
}
BOOL cmdSt(const char *tail)
{
	uffs_Device *dev;
	const char *mount = "/";
	uffs_stat *s;
	TreeNode *node;

	if (tail) {
		mount = cli_getparam(tail, NULL);
	}

	dev = uffs_GetDevice(mount);
	if (dev == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't get device from mount point %s\n", mount);
		return TRUE;
	}

	s = &(dev->st);

	uffs_Perror(UFFS_ERR_NORMAL, "----------- basic info -----------\n");
	uffs_Perror(UFFS_ERR_NORMAL, "TreeNode size:         %d\n", sizeof(TreeNode));
	uffs_Perror(UFFS_ERR_NORMAL, "MaxCachedBlockInfo:    %d\n", MAX_CACHED_BLOCK_INFO);
	uffs_Perror(UFFS_ERR_NORMAL, "MaxPageBuffers:        %d\n", MAX_PAGE_BUFFERS);
	uffs_Perror(UFFS_ERR_NORMAL, "MaxDirtyPagesPerBlock: %d\n", MAX_DIRTY_PAGES_IN_A_BLOCK);
	uffs_Perror(UFFS_ERR_NORMAL, "MaxPathLength:         %d\n", MAX_PATH_LENGTH);
	uffs_Perror(UFFS_ERR_NORMAL, "MaxObjectHandles:      %d\n", MAX_OBJECT_HANDLE);

	uffs_Perror(UFFS_ERR_NORMAL, "----------- statistics for '%s' -----------\n", mount);
	uffs_Perror(UFFS_ERR_NORMAL, "Block Erased: %d\n", s->blockEraseCount);
	uffs_Perror(UFFS_ERR_NORMAL, "Write Page:   %d\n", s->pageWriteCount);
	uffs_Perror(UFFS_ERR_NORMAL, "Write Spare:  %d\n", s->spareWriteCount);
	uffs_Perror(UFFS_ERR_NORMAL, "Read Page:    %d\n", s->pageReadCount);
	uffs_Perror(UFFS_ERR_NORMAL, "Read Spare:   %d\n", s->spareReadCount);
	uffs_Perror(UFFS_ERR_NORMAL, "Disk total:   %d\n", uffs_GetDeviceTotal(dev));
	uffs_Perror(UFFS_ERR_NORMAL, "Disk Used:    %d\n", uffs_GetDeviceUsed(dev));
	uffs_Perror(UFFS_ERR_NORMAL, "Disk Free:    %d\n", uffs_GetDeviceFree(dev));
	uffs_Perror(UFFS_ERR_NORMAL, "Block size:   %d\n", dev->attr->block_data_size);
	uffs_Perror(UFFS_ERR_NORMAL, "Total blocks: %d of %d\n", (dev->par.end - dev->par.start + 1), dev->attr->total_blocks);
	if (dev->tree.bad) {
		uffs_Perror(UFFS_ERR_NORMAL, "Bad blocks: ");
		node = dev->tree.bad;
		while(node) {
			uffs_Perror(UFFS_ERR_NORMAL, "%d, ", node->u.list.block);
			node = node->u.list.next;
		}
		uffs_Perror(UFFS_ERR_NORMAL, "\n");
	}

	uffs_BufInspect(dev);

	uffs_PutDevice(dev);
	return TRUE;

}


BOOL cmdCp(const char *tail)
{
	const char *src;
	char *des;
	char buf[100];
	uffs_Object *f1 = NULL, *f2 = NULL;
	int len;
	BOOL src_local = FALSE, des_local = FALSE;
	FILE *fp1 = NULL, *fp2 = NULL;

	if (!tail) return FALSE;
	src = cli_getparam(tail, &des);
	if (!des) return FALSE;

	if (memcmp(src, "::", 2) == 0) {
		src += 2;
		src_local = TRUE;
	}
	if (memcmp(des, "::", 2) == 0) {
		des += 2;
		des_local = TRUE;
	}
	f1 = uffs_GetObject();
	f2 = uffs_GetObject();
	if (!f1 || !f2) goto fail_2;
	
	if (src_local) {
		if ((fp1 = fopen(src, "rb")) == NULL) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open %s for copy.\n", src);
			goto fail_1;
		}
	}
	else {
		if (uffs_OpenObject(f1, src, UO_RDONLY, US_IREAD) != U_SUCC) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open %s for copy.\n", src);
			goto fail_1;
		}
	}

	if (des_local) {
		if ((fp2 = fopen(des, "wb")) == NULL) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open %s for copy.\n", des);
			goto fail_1;
		}
	}
	else {
		if (uffs_OpenObject(f2, des, UO_RDWR|UO_CREATE|UO_TRUNC, US_IREAD|US_IWRITE) != U_SUCC) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open %s for copy.\n", des);
			goto fail_1;
		}
	}

	while (	(src_local ? (feof(fp1) == 0) : (uffs_EndOfFile(f1) == 0)) ) {
		if (src_local) {
			len = fread(buf, 1, sizeof(buf), fp1);
		}
		else {
			len = uffs_ReadObject(f1, buf, sizeof(buf));
		}
		if (len == 0) break;
		if (len < 0) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"read file %s fail ?\n", src);
			break;
		}
		if (des_local) {
			if ((int)fwrite(buf, 1, len, fp2) != len) {
				uffs_Perror(UFFS_ERR_NORMAL, PFX"write file %s fail ? \n", des);
				break;
			}
		}
		else {
			if (uffs_WriteObject(f2, buf, len) != len) {
				uffs_Perror(UFFS_ERR_NORMAL, PFX"write file %s fail ? \n", des);
				break;
			}
		}
	}

fail_1:
	uffs_CloseObject(f1);
	uffs_CloseObject(f2);
	if (fp1) fclose(fp1);
	if (fp2) fclose(fp2);
fail_2:
	if (f1) {
		uffs_PutObject(f1);
	}
	if (f2) {
		uffs_PutObject(f2);
	}
	return TRUE;
}

BOOL cmdCat(const char *tail)
{
	uffs_Object *obj;
	const char *name;
	char buf[100];
	int len;

	if (!tail) return FALSE;
	name = cli_getparam(tail, NULL);

	obj = uffs_GetObject();
	if (!obj) return FALSE;

	if (uffs_OpenObject(obj, name, UO_RDONLY, US_IREAD) == U_FAIL) {
		uffs_Perror(UFFS_ERR_NORMAL, "Can't open %s\n", name);
		goto fail;
	}

	while (uffs_EndOfFile(obj) == 0) {
		len = uffs_ReadObject(obj, buf, sizeof(buf) - 1);
		if (len == 0) break;
		if (len > 0) {
			buf[len] = 0;
			uffs_Perror(UFFS_ERR_NORMAL, "%s", buf);
		}
	}
	uffs_Perror(UFFS_ERR_NORMAL, "\n");
	uffs_CloseObject(obj);

fail:
	uffs_PutObject(obj);

	return TRUE;
}


static URET test_verify_file(const char *file_name)
{
	uffs_Object *f = NULL;
	int ret = U_FAIL;
	unsigned char buf[100];
	int i, pos, len;

	f = uffs_GetObject();
	if (f == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Fail to get object.\n");
		goto test_exit;
	}

	if (uffs_OpenObject(f, file_name, UO_RDONLY, US_IREAD) != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open file %s for read.\n", file_name);
		goto test_exit;
	}

	pos = 0;
	while (!uffs_EndOfFile(f)) {
		len = uffs_ReadObject(f, buf, sizeof(buf));
		for (i = 0; i < len; i++) {
			if (buf[i] != (pos++ & 0xFF)) {
				pos--;
				uffs_Perror(UFFS_ERR_NORMAL, PFX"Verify file %s failed at: %d, expect %x but got %x\n", file_name, pos, pos & 0xFF, buf[i]);
				goto test_failed;
			}
		}
	}

	if (pos != uffs_SeekObject(f, 0, USEEK_END)) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Verify file %s failed. invalid file length.\n", file_name);
		goto test_failed;
	}

	uffs_Perror(UFFS_ERR_NORMAL, PFX"Verify file %s succ.\n", file_name);
	ret = U_SUCC;

test_failed:
	uffs_CloseObject(f);

test_exit:
	uffs_PutObject(f);
	return ret;
}

static URET do_write_test_file(uffs_Object *f, int size)
{
	long pos;
	unsigned char buf[100];
	unsigned char data;
	int i, len;

	while (size > 0) {
		pos = uffs_SeekObject(f, 0, USEEK_CUR);
		len = (size > sizeof(buf) ? sizeof(buf) : size);
		data = pos & 0xFF;
		for (i = 0; i < len; i++, data++) {
			buf[i] = data;
		}
		if (uffs_WriteObject(f, buf, len) != len) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Write file failed, size %d at %d\n", len, pos);
			return U_FAIL;
		}
		size -= len;
	}

	return U_SUCC;
}

static URET test_append_file(const char *file_name, int size)
{
	int ret = U_FAIL;
	uffs_Object *f = NULL;

	f = uffs_GetObject();
	if (f == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Fail to get object.\n");
		goto test_exit;
	}

	if (uffs_OpenObject(f, file_name, UO_RDWR|UO_APPEND|UO_CREATE, US_IREAD|US_IWRITE) != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open file %s for append.\n", file_name);
		goto test_exit;
	}

	uffs_SeekObject(f, 0, USEEK_END);

	if (do_write_test_file(f, size) == U_FAIL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Write file %s failed.\n", file_name);
		goto test_failed;
	}
	ret = U_SUCC;

test_failed:
	uffs_CloseObject(f);

test_exit:
	if (f) uffs_PutObject(f);
	return ret;
}

static URET test_write_file(const char *file_name, int pos, int size)
{
	int ret = U_FAIL;
	uffs_Object *f = NULL;

	f = uffs_GetObject();
	if (f == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Fail to get object.\n");
		goto test_exit;
	}

	if (uffs_OpenObject(f, file_name, UO_RDWR|UO_CREATE, US_IREAD|US_IWRITE) != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open file %s for write.\n", file_name);
		goto test_exit;
	}

	if (uffs_SeekObject(f, pos, USEEK_SET) != pos) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't seek file %s at pos %d\n", file_name, pos);
		goto test_failed;
	}

	if (do_write_test_file(f, size) == U_FAIL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Write file %s failed.\n", file_name);
		goto test_failed;
	}
	ret = U_SUCC;

test_failed:
	uffs_CloseObject(f);

test_exit:
	if (f) uffs_PutObject(f);
	return ret;
}


static URET DoTest2(void)
{
	uffs_Object *f;
	URET ret = U_FAIL;
	char buf[100], buf_1[100];

	f = uffs_GetObject();
	if (f == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Fail to get object.\n");
		goto exit_test;
	}

	ret = uffs_OpenObject(f, "/abc/", UO_RDWR|UO_DIR, US_IREAD|US_IWRITE);
	if (ret != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open dir abc, err: %d\n", f->err);
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Try to create a new one...\n");
		ret = uffs_OpenObject(f, "/abc/", UO_RDWR|UO_CREATE|UO_DIR, US_IREAD|US_IWRITE);
		if (ret != U_SUCC) {
			uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't create new dir /abc/\n");
			goto exit_test;
		}
		else {
			uffs_CloseObject(f);
		}
	}
	else {
		uffs_CloseObject(f);
	}
	
	ret = uffs_OpenObject(f, "/abc/test.txt", UO_RDWR|UO_CREATE, US_IWRITE|US_IREAD);
	if (ret != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open /abc/test.txt\n");
		goto exit_test;
	}

	sprintf(buf, "123456789ABCDEF");
	ret = uffs_WriteObject(f, buf, strlen(buf));
	uffs_Perror(UFFS_ERR_NORMAL, PFX"write %d bytes to file, content: %s\n", ret, buf);

	ret = uffs_SeekObject(f, 3, USEEK_SET);
	uffs_Perror(UFFS_ERR_NORMAL, PFX"new file position: %d\n", ret);

	memset(buf_1, 0, sizeof(buf_1));
	ret = uffs_ReadObject(f, buf_1, 5);
	uffs_Perror(UFFS_ERR_NORMAL, PFX"read %d bytes, content: %s\n", ret, buf_1);

	if (memcmp(buf + 3, buf_1, 5) != 0) {
		ret = U_FAIL;
	}
	else {
		ret = U_SUCC;
	}

	uffs_CloseObject(f);

exit_test:
	if (f) {
		uffs_PutObject(f);
	}

	return ret;
}

BOOL cmdTest1(const char *tail)
{
	uffs_Object *f;
	URET ret;
	char buf[100];
	const char *name;

	if (!tail) {
		return FALSE;
	}

	name = cli_getparam(tail, NULL);

	f = uffs_GetObject();
	if (f == NULL) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Fail to get object.\n");
		return TRUE;	
	}

	ret = uffs_OpenObject(f, name, UO_RDWR|UO_CREATE|UO_TRUNC, US_IWRITE|US_IREAD);
	if (ret != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX"Can't open %s\n", name);
		goto fail;
	}

	sprintf(buf, "123456789ABCDEF");
	ret = uffs_WriteObject(f, buf, strlen(buf));
	uffs_Perror(UFFS_ERR_NORMAL, PFX"write %d bytes to file, content: %s\n", ret, buf);

	ret = uffs_SeekObject(f, 3, USEEK_SET);
	uffs_Perror(UFFS_ERR_NORMAL, PFX"new file position: %d\n", ret);

	memset(buf, 0, sizeof(buf));
	ret = uffs_ReadObject(f, buf, 5);
	uffs_Perror(UFFS_ERR_NORMAL, PFX"read %d bytes, content: %s\n", ret, buf);

	uffs_CloseObject(f);

fail:
	uffs_PutObject(f);

	return TRUE;
}

BOOL cmdTest2(const char *tail)
{
	uffs_Perror(UFFS_ERR_NORMAL, "Test return: %s !\n", DoTest2() == U_SUCC ? "succ" : "failed");

	return TRUE;
}

BOOL cmdTest3(const char *tail)
{
	const char *name;
	int i;
	int write_test_seq[] = { 20, 10, 500, 40, 1140, 900, 329, 4560, 352, 1100 };

	if (!tail) {
		return FALSE;
	}

	name = cli_getparam(tail, NULL);
	uffs_Perror(UFFS_ERR_NORMAL, "Test append file %s ...\n", name);
	for (i = 1; i < 500; i += 29) {
		if (test_append_file(name, i) != U_SUCC) {
			uffs_Perror(UFFS_ERR_NORMAL, "Append file %s test failed at %d !\n", name, i);
			return TRUE;
		}
		//if (test_verify_file(name) != U_SUCC) {
		//	uffs_Perror(UFFS_ERR_NORMAL, "Verify file %s failed at test no: %d.\n", name, i);
		//}
	}

	uffs_Perror(UFFS_ERR_NORMAL, "Check file %s ... \n", name);
	if (test_verify_file(name) != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, "Verify file %s failed.\n", name);
		return TRUE;
	}

	uffs_Perror(UFFS_ERR_NORMAL, "Test write file ...\n");
	for (i = 0; i < sizeof(write_test_seq) / sizeof(int) - 1; i++) {
		if (test_write_file(name, write_test_seq[i], write_test_seq[i+1]) != U_SUCC) {
			uffs_Perror(UFFS_ERR_NORMAL, "Test write file failed !\n");
			return TRUE;
		}
	}

	uffs_Perror(UFFS_ERR_NORMAL, "Check file %s ... \n", name);
	if (test_verify_file(name) != U_SUCC) {
		uffs_Perror(UFFS_ERR_NORMAL, "Verify file %s failed.\n", name);
		return TRUE;
	}

	uffs_Perror(UFFS_ERR_NORMAL, "Test succ !\n");

	return TRUE;
}

BOOL cmdMount(const char *tail)
{
	uffs_mountTable *tab = uffs_GetMountTable();
	tail = tail;
	while (tab) {
		uffs_Perror(UFFS_ERR_NORMAL, PFX" %s : (%d) ~ (%d)\n", tab->mountPoint, tab->startBlock, tab->endBlock);
		tab = tab->next;
	}
	return TRUE;
}

static struct cli_commandset cmdset[] = 
{
    { cmdFormat,	"format",		"[<mount>]",		"Format device" },
    { cmdMkf,		"mkfile",		"<name>",			"create a new file" },
    { cmdMkdir,		"mkdir",		"<name>",			"create a new directory" },
    { cmdRm,		"rm",			"<name>",			"delete file/directory" },
    { cmdRen,		"mv|ren",		"<old> <new>",		"rename file/directory" },
    { cmdLs,		"ls",			"<dir>",			"list dirs and files" },
    { cmdSt,		"info|st",		"<mount>",			"show statistic infomation" },
    { cmdTest1,		"t1",			"<name>",			"test 1" },
    { cmdTest2,		"t2",			NULL,				"test 2" },
    { cmdTest3,		"t3",			"<name>",			"test 3" },
    { cmdCp,		"cp",			"<src> <des>",		"copy files. the local file name start with '::'" },
    { cmdCat,		"cat",			"<name>",			"show file content" },
    { cmdPwd,		"pwd",			NULL,				"show current dir" },
    { cmdCd,		"cd",			"<path>",			"change current dir" },
    { cmdMount,		"mount",		NULL,				"list mounted file systems" },

    { NULL, NULL, NULL, NULL }
};


struct cli_commandset * get_helper_cmds()
{
	return cmdset;
};