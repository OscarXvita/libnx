#include <switch.h>
#include <string.h>

Result nvioctlNvhostAsGpu_BindChannel(u32 fd, u32 channel_fd) {
    struct {
        __in u32 fd;
    } data;

    memset(&data, 0, sizeof(data));
    data.fd = channel_fd;

    return nvIoctl(fd, _IOW(0x41, 0x01, data), &data);
}

Result nvioctlNvhostAsGpu_AllocSpace(u32 fd, u32 pages, u32 page_size, u32 flags, u64 align, u64 *offset) {
    Result rc = 0;

    struct {
        __in u32 pages;
        __in u32 page_size;
        __in u32 flags;
        u32      pad;
        union {
            __out u64 offset;
            __in  u64 align;
        };
    } data;

    memset(&data, 0, sizeof(data));
    data.pages = pages;
    data.page_size = page_size;
    data.flags = flags;
    data.align = align;

    rc = nvIoctl(fd, _IOWR(0x41, 0x02, data), &data);
    if (R_FAILED(rc)) return rc;

    *offset = data.offset;

    return rc;
}

Result nvioctlNvhostAsGpu_MapBufferEx(
    u32 fd, u32 flags, u32 kind, u32 nvmap_handle, u32 page_size,
    u64 buffer_offset, u64 mapping_size, u64 input_offset, u64 *offset)
{
    Result rc = 0;

    struct {
        __in u32 flags;        // bit0: fixed_offset, bit2: cacheable
        __in u32 kind;         // -1 is default
        __in u32 nvmap_handle;
        __inout u32 page_size; // 0 means don't care
        __in u64 buffer_offset;
        __in u64 mapping_size;
        __inout u64 offset;
    } data;

    memset(&data, 0, sizeof(data));
    data.flags = flags;
    data.kind = kind;
    data.nvmap_handle = nvmap_handle;
    data.page_size = page_size;
    data.buffer_offset = buffer_offset;
    data.mapping_size = mapping_size;
    data.offset = input_offset;

    rc = nvIoctl(fd, _IOWR(0x41, 0x06, data), &data);

    if (R_SUCCEEDED(rc) && offset) {
        *offset = data.offset;
    }

    return rc;
}

Result nvioctlNvhostAsGpu_GetVARegions(u32 fd, nvioctl_va_region regions[2]) {
    Result rc=0;

    struct {
        u64 not_used;        // contained output user ptr on linux, ignored
        __inout u32 bufsize; // forced to 2*sizeof(struct va_region)
        u32 pad;
        __out nvioctl_va_region regions[2];
    } data;

    memset(&data, 0, sizeof(data));
    data.bufsize = sizeof(data.regions);

    rc = nvIoctl(fd, _IOWR(0x41, 0x08, data), &data);

    if (R_SUCCEEDED(rc)) {
        memcpy(regions, data.regions, sizeof(data.regions));
    }

    return rc;
}

Result nvioctlNvhostAsGpu_InitializeEx(u32 fd, u32 big_page_size, u32 flags) {
    struct {
        __in u32 big_page_size;   // depends on GPU's available_big_page_sizes; 0=default
        __in s32 as_fd;           // ignored; passes 0
        __in u32 flags;           // ignored; passes 0
        __in u32 reserved;        // ignored; passes 0
        __in u64 unk0;
        __in u64 unk1;
        __in u64 unk2;
    } data;

    memset(&data, 0, sizeof(data));
    data.big_page_size = big_page_size;
    data.flags = flags;

    return nvIoctl(fd, _IOW(0x41, 0x09, data), &data);
}
