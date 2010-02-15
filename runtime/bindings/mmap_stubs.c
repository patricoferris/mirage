/*
 * Copyright (C) 2006-2009 Citrix Systems Inc.
 * Copyright (C) 2010 Anil Madhavapeddy <anil@recoil.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation; version 2.1 only. with the special
 * exception on linking described in file LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 */

#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include "mmap_stubs.h"

#include <xen/xen.h>
#include <mini-os/hypervisor.h>

#include <mlvalues.h>
#include <memory.h>
#include <alloc.h>
#include <custom.h>
#include <fail.h>

#define GET_C_STRUCT(a) ((struct mmap_interface *) a)

CAMLprim value
stub_xenstore_init(value unit)
{
	CAMLparam1(unit);
	CAMLlocal1(result);
        
	result = caml_alloc(sizeof(struct mmap_interface), Abstract_tag);

        GET_C_STRUCT(result)->len = 4096;
        GET_C_STRUCT(result)->addr = (char *)mfn_to_virt(start_info.store_mfn);

	CAMLreturn(result);
}

CAMLprim value
stub_mmap_read(value interface, value start, value len)
{
	CAMLparam3(interface, start, len);
	CAMLlocal1(data);
	struct mmap_interface *intf;
	int c_start;
	int c_len;

	c_start = Int_val(start);
	c_len = Int_val(len);
	intf = GET_C_STRUCT(interface);

	if (c_start > intf->len)
		caml_invalid_argument("start invalid");
	if (c_start + c_len > intf->len)
		caml_invalid_argument("len invalid");

	data = caml_alloc_string(c_len);
	memcpy((char *) data, intf->addr + c_start, c_len);

	CAMLreturn(data);
}

CAMLprim value
stub_mmap_write(value interface, value data,
                               value start, value len)
{
	CAMLparam4(interface, data, start, len);
	struct mmap_interface *intf;
	int c_start;
	int c_len;

	c_start = Int_val(start);
	c_len = Int_val(len);
	intf = GET_C_STRUCT(interface);

	if (c_start > intf->len)
		caml_invalid_argument("start invalid");
	if (c_start + c_len > intf->len)
		caml_invalid_argument("len invalid");

	memcpy(intf->addr + c_start, (char *) data, c_len);

	CAMLreturn(Val_unit);
}
