/*
 * VFIO API definition
 *
 * Copyright (C) 2012 Red Hat, Inc.  All rights reserved.
 *     Author: Alex Williamson <alex.williamson@redhat.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#ifndef _UAPIVFIO_H
#define _UAPIVFIO_H

#include <linux/types.h>
#include <linux/ioctl.h>
#include <unistd.h>
#include <stdint.h>

#define VFIO_API_VERSION	0


/* Kernel & User level defines for VFIO IOCTLs. */

/* Extensions */

#define VFIO_TYPE1_IOMMU		1
#define VFIO_NOIOMMU_IOMMU		8

/*
 * The IOCTL interface is designed for extensibility by embedding the
 * structure length (argsz) and flags into structures passed between
 * kernel and userspace.  We therefore use the _IO() macro for these
 * defines to avoid implicitly embedding a size into the ioctl request.
 * As structure fields are added, argsz will increase to match and flag
 * bits will be defined to indicate additional fields with valid data.
 * It's *always* the caller's responsibility to indicate the size of
 * the structure passed by setting argsz appropriately.
 */

#define VFIO_TYPE	(';')
#define VFIO_BASE	100

/* -------- IOCTLs for VFIO file descriptor (/dev/vfio/vfio) -------- */

/**
 * VFIO_GET_API_VERSION - _IO(VFIO_TYPE, VFIO_BASE + 0)
 *
 * Report the version of the VFIO API.  This allows us to bump the entire
 * API version should we later need to add or change features in incompatible
 * ways.
 * Return: VFIO_API_VERSION
 * Availability: Always
 */
#define VFIO_GET_API_VERSION		_IO(VFIO_TYPE, VFIO_BASE + 0)

/**
 * VFIO_CHECK_EXTENSION - _IOW(VFIO_TYPE, VFIO_BASE + 1, __u32)
 *
 * Check whether an extension is supported.
 * Return: 0 if not supported, 1 (or some other positive integer) if supported.
 * Availability: Always
 */
#define VFIO_CHECK_EXTENSION		_IO(VFIO_TYPE, VFIO_BASE + 1)

/**
 * VFIO_SET_IOMMU - _IOW(VFIO_TYPE, VFIO_BASE + 2, __s32)
 *
 * Set the iommu to the given type.  The type must be supported by an
 * iommu driver as verified by calling CHECK_EXTENSION using the same
 * type.  A group must be set to this file descriptor before this
 * ioctl is available.  The IOMMU interfaces enabled by this call are
 * specific to the value set.
 * Return: 0 on success, -errno on failure
 * Availability: When VFIO group attached
 */
#define VFIO_SET_IOMMU			_IO(VFIO_TYPE, VFIO_BASE + 2)

/* -------- IOCTLs for GROUP file descriptors (/dev/vfio/$GROUP) -------- */

/**
 * VFIO_GROUP_GET_STATUS - _IOR(VFIO_TYPE, VFIO_BASE + 3,
 *						struct vfio_group_status)
 *
 * Retrieve information about the group.  Fills in provided
 * struct vfio_group_info.  Caller sets argsz.
 * Return: 0 on succes, -errno on failure.
 * Availability: Always
 */
struct vfio_group_status {
	__u32	argsz;
	__u32	flags;
#define VFIO_GROUP_FLAGS_VIABLE		(1 << 0)
#define VFIO_GROUP_FLAGS_CONTAINER_SET	(1 << 1)
};
#define VFIO_GROUP_GET_STATUS		_IO(VFIO_TYPE, VFIO_BASE + 3)

/**
 * VFIO_GROUP_SET_CONTAINER - _IOW(VFIO_TYPE, VFIO_BASE + 4, __s32)
 *
 * Set the container for the VFIO group to the open VFIO file
 * descriptor provided.  Groups may only belong to a single
 * container.  Containers may, at their discretion, support multiple
 * groups.  Only when a container is set are all of the interfaces
 * of the VFIO file descriptor and the VFIO group file descriptor
 * available to the user.
 * Return: 0 on success, -errno on failure.
 * Availability: Always
 */
#define VFIO_GROUP_SET_CONTAINER	_IO(VFIO_TYPE, VFIO_BASE + 4)

/**
 * VFIO_GROUP_UNSET_CONTAINER - _IO(VFIO_TYPE, VFIO_BASE + 5)
 *
 * Remove the group from the attached container.  This is the
 * opposite of the SET_CONTAINER call and returns the group to
 * an initial state.  All device file descriptors must be released
 * prior to calling this interface.  When removing the last group
 * from a container, the IOMMU will be disabled and all state lost,
 * effectively also returning the VFIO file descriptor to an initial
 * state.
 * Return: 0 on success, -errno on failure.
 * Availability: When attached to container
 */
#define VFIO_GROUP_UNSET_CONTAINER	_IO(VFIO_TYPE, VFIO_BASE + 5)

/**
 * VFIO_GROUP_GET_DEVICE_FD - _IOW(VFIO_TYPE, VFIO_BASE + 6, char)
 *
 * Return a new file descriptor for the device object described by
 * the provided string.  The string should match a device listed in
 * the devices subdirectory of the IOMMU group sysfs entry.  The
 * group containing the device must already be added to this context.
 * Return: new file descriptor on success, -errno on failure.
 * Availability: When attached to container
 */
#define VFIO_GROUP_GET_DEVICE_FD	_IO(VFIO_TYPE, VFIO_BASE + 6)

/* --------------- IOCTLs for DEVICE file descriptors --------------- */

/**
 * VFIO_DEVICE_GET_INFO - _IOR(VFIO_TYPE, VFIO_BASE + 7,
 *						struct vfio_device_info)
 *
 * Retrieve information about the device.  Fills in provided
 * struct vfio_device_info.  Caller sets argsz.
 * Return: 0 on success, -errno on failure.
 */
struct vfio_device_info {
	__u32	argsz;
	__u32	flags;
#define VFIO_DEVICE_FLAGS_RESET	(1 << 0)	/* Device supports reset */
#define VFIO_DEVICE_FLAGS_PCI	(1 << 1)	/* vfio-pci device */
	__u32	num_regions;	/* Max region index + 1 */
	__u32	num_irqs;	/* Max IRQ index + 1 */
};
#define VFIO_DEVICE_GET_INFO		_IO(VFIO_TYPE, VFIO_BASE + 7)

/**
 * VFIO_DEVICE_GET_REGION_INFO - _IOWR(VFIO_TYPE, VFIO_BASE + 8,
 *				       struct vfio_region_info)
 *
 * Retrieve information about a device region.  Caller provides
 * struct vfio_region_info with index value set.  Caller sets argsz.
 * Implementation of region mapping is bus driver specific.  This is
 * intended to describe MMIO, I/O port, as well as bus specific
 * regions (ex. PCI config space).  Zero sized regions may be used
 * to describe unimplemented regions (ex. unimplemented PCI BARs).
 * Return: 0 on success, -errno on failure.
 */
struct vfio_region_info {
	__u32	argsz;
	__u32	flags;
#define VFIO_REGION_INFO_FLAG_READ	(1 << 0) /* Region supports read */
#define VFIO_REGION_INFO_FLAG_WRITE	(1 << 1) /* Region supports write */
#define VFIO_REGION_INFO_FLAG_MMAP	(1 << 2) /* Region supports mmap */
	__u32	index;		/* Region index */
	__u32	resv;		/* Reserved for alignment */
	__u64	size;		/* Region size (bytes) */
	__u64	offset;		/* Region offset from start of device fd */
};
#define VFIO_DEVICE_GET_REGION_INFO	_IO(VFIO_TYPE, VFIO_BASE + 8)

/**
 * VFIO_DEVICE_GET_IRQ_INFO - _IOWR(VFIO_TYPE, VFIO_BASE + 9,
 *				    struct vfio_irq_info)
 *
 * Retrieve information about a device IRQ.  Caller provides
 * struct vfio_irq_info with index value set.  Caller sets argsz.
 * Implementation of IRQ mapping is bus driver specific.  Indexes
 * using multiple IRQs are primarily intended to support MSI-like
 * interrupt blocks.  Zero count irq blocks may be used to describe
 * unimplemented interrupt types.
 *
 * The EVENTFD flag indicates the interrupt index supports eventfd based
 * signaling.
 *
 * The MASKABLE flags indicates the index supports MASK and UNMASK
 * actions described below.
 *
 * AUTOMASKED indicates that after signaling, the interrupt line is
 * automatically masked by VFIO and the user needs to unmask the line
 * to receive new interrupts.  This is primarily intended to distinguish
 * level triggered interrupts.
 *
 * The NORESIZE flag indicates that the interrupt lines within the index
 * are setup as a set and new subindexes cannot be enabled without first
 * disabling the entire index.  This is used for interrupts like PCI MSI
 * and MSI-X where the driver may only use a subset of the available
 * indexes, but VFIO needs to enable a specific number of vectors
 * upfront.  In the case of MSI-X, where the user can enable MSI-X and
 * then add and unmask vectors, it's up to userspace to make the decision
 * whether to allocate the maximum supported number of vectors or tear
 * down setup and incrementally increase the vectors as each is enabled.
 */
struct vfio_irq_info {
	__u32	argsz;
	__u32	flags;
#define VFIO_IRQ_INFO_EVENTFD		(1 << 0)
#define VFIO_IRQ_INFO_MASKABLE		(1 << 1)
#define VFIO_IRQ_INFO_AUTOMASKED	(1 << 2)
#define VFIO_IRQ_INFO_NORESIZE		(1 << 3)
	__u32	index;		/* IRQ index */
	__u32	count;		/* Number of IRQs within this index */
};
#define VFIO_DEVICE_GET_IRQ_INFO	_IO(VFIO_TYPE, VFIO_BASE + 9)

/**
 * VFIO_DEVICE_SET_IRQS - _IOW(VFIO_TYPE, VFIO_BASE + 10, struct vfio_irq_set)
 *
 * Set signaling, masking, and unmasking of interrupts.  Caller provides
 * struct vfio_irq_set with all fields set.  'start' and 'count' indicate
 * the range of subindexes being specified.
 *
 * The DATA flags specify the type of data provided.  If DATA_NONE, the
 * operation performs the specified action immediately on the specified
 * interrupt(s).  For example, to unmask AUTOMASKED interrupt [0,0]:
 * flags = (DATA_NONE|ACTION_UNMASK), index = 0, start = 0, count = 1.
 *
 * DATA_BOOL allows sparse support for the same on arrays of interrupts.
 * For example, to mask interrupts [0,1] and [0,3] (but not [0,2]):
 * flags = (DATA_BOOL|ACTION_MASK), index = 0, start = 1, count = 3,
 * data = {1,0,1}
 *
 * DATA_EVENTFD binds the specified ACTION to the provided __s32 eventfd.
 * A value of -1 can be used to either de-assign interrupts if already
 * assigned or skip un-assigned interrupts.  For example, to set an eventfd
 * to be trigger for interrupts [0,0] and [0,2]:
 * flags = (DATA_EVENTFD|ACTION_TRIGGER), index = 0, start = 0, count = 3,
 * data = {fd1, -1, fd2}
 * If index [0,1] is previously set, two count = 1 ioctls calls would be
 * required to set [0,0] and [0,2] without changing [0,1].
 *
 * Once a signaling mechanism is set, DATA_BOOL or DATA_NONE can be used
 * with ACTION_TRIGGER to perform kernel level interrupt loopback testing
 * from userspace (ie. simulate hardware triggering).
 *
 * Setting of an event triggering mechanism to userspace for ACTION_TRIGGER
 * enables the interrupt index for the device.  Individual subindex interrupts
 * can be disabled using the -1 value for DATA_EVENTFD or the index can be
 * disabled as a whole with: flags = (DATA_NONE|ACTION_TRIGGER), count = 0.
 *
 * Note that ACTION_[UN]MASK specify user->kernel signaling (irqfds) while
 * ACTION_TRIGGER specifies kernel->user signaling.
 */
struct vfio_irq_set {
	__u32	argsz;
	__u32	flags;
#define VFIO_IRQ_SET_DATA_NONE		(1 << 0) /* Data not present */
#define VFIO_IRQ_SET_DATA_BOOL		(1 << 1) /* Data is bool (u8) */
#define VFIO_IRQ_SET_DATA_EVENTFD	(1 << 2) /* Data is eventfd (s32) */
#define VFIO_IRQ_SET_ACTION_MASK	(1 << 3) /* Mask interrupt */
#define VFIO_IRQ_SET_ACTION_UNMASK	(1 << 4) /* Unmask interrupt */
#define VFIO_IRQ_SET_ACTION_TRIGGER	(1 << 5) /* Trigger interrupt */
	__u32	index;
	__u32	start;
	__u32	count;
	__u8	data[];
};
#define VFIO_DEVICE_SET_IRQS		_IO(VFIO_TYPE, VFIO_BASE + 10)

#define VFIO_IRQ_SET_DATA_TYPE_MASK	(VFIO_IRQ_SET_DATA_NONE | \
					 VFIO_IRQ_SET_DATA_BOOL | \
					 VFIO_IRQ_SET_DATA_EVENTFD)
#define VFIO_IRQ_SET_ACTION_TYPE_MASK	(VFIO_IRQ_SET_ACTION_MASK | \
					 VFIO_IRQ_SET_ACTION_UNMASK | \
					 VFIO_IRQ_SET_ACTION_TRIGGER)
/**
 * VFIO_DEVICE_RESET - _IO(VFIO_TYPE, VFIO_BASE + 11)
 *
 * Reset a device.
 */
#define VFIO_DEVICE_RESET		_IO(VFIO_TYPE, VFIO_BASE + 11)

/*
 * The VFIO-PCI bus driver makes use of the following fixed region and
 * IRQ index mapping.  Unimplemented regions return a size of zero.
 * Unimplemented IRQ types return a count of zero.
 */

enum {
	VFIO_PCI_BAR0_REGION_INDEX,
	VFIO_PCI_BAR1_REGION_INDEX,
	VFIO_PCI_BAR2_REGION_INDEX,
	VFIO_PCI_BAR3_REGION_INDEX,
	VFIO_PCI_BAR4_REGION_INDEX,
	VFIO_PCI_BAR5_REGION_INDEX,
	VFIO_PCI_ROM_REGION_INDEX,
	VFIO_PCI_CONFIG_REGION_INDEX,
	/*
	 * Expose VGA regions defined for PCI base class 03, subclass 00.
	 * This includes I/O port ranges 0x3b0 to 0x3bb and 0x3c0 to 0x3df
	 * as well as the MMIO range 0xa0000 to 0xbffff.  Each implemented
	 * range is found at it's identity mapped offset from the region
	 * offset, for example 0x3b0 is region_info.offset + 0x3b0.  Areas
	 * between described ranges are unimplemented.
	 */
	VFIO_PCI_VGA_REGION_INDEX,
	VFIO_PCI_NUM_REGIONS
};

enum {
	VFIO_PCI_INTX_IRQ_INDEX,
	VFIO_PCI_MSI_IRQ_INDEX,
	VFIO_PCI_MSIX_IRQ_INDEX,
	VFIO_PCI_NUM_IRQS
};

/**
 * VFIO_DEVICE_GET_PCI_HOT_RESET_INFO - _IORW(VFIO_TYPE, VFIO_BASE + 12,
 *                                            struct vfio_pci_hot_reset_info)
 *
 * Return: 0 on success, -errno on failure:
 *      -enospc = insufficient buffer, -enodev = unsupported for device.
 */
struct vfio_pci_dependent_device {
        __u32   group_id;
        __u16   segment;
        __u8    bus;
        __u8    devfn; /* Use PCI_SLOT/PCI_FUNC */
};

struct vfio_pci_hot_reset_info {
        __u32   argsz;
        __u32   flags;
        __u32   count;
        struct vfio_pci_dependent_device        devices[];
};

#define VFIO_DEVICE_GET_PCI_HOT_RESET_INFO      _IO(VFIO_TYPE, VFIO_BASE + 12)

/**
 * VFIO_DEVICE_PCI_HOT_RESET - _IOW(VFIO_TYPE, VFIO_BASE + 13,
 *                                  struct vfio_pci_hot_reset)
 *
 * Return: 0 on success, -errno on failure.
 */
struct vfio_pci_hot_reset {
        __u32   argsz;
        __u32   flags;
        __u32   count;
        __s32   group_fds[];
};

#define VFIO_DEVICE_PCI_HOT_RESET       _IO(VFIO_TYPE, VFIO_BASE + 13)

/* -------- API for Type1 VFIO IOMMU -------- */

/**
 * VFIO_IOMMU_GET_INFO - _IOR(VFIO_TYPE, VFIO_BASE + 12, struct vfio_iommu_info)
 *
 * Retrieve information about the IOMMU object. Fills in provided
 * struct vfio_iommu_info. Caller sets argsz.
 *
 * XXX Should we do these by CHECK_EXTENSION too?
 */
struct vfio_iommu_type1_info {
	__u32	argsz;
	__u32	flags;
#define VFIO_IOMMU_INFO_PGSIZES (1 << 0)	/* supported page sizes info */
	__u64	iova_pgsizes;		/* Bitmap of supported page sizes */
};

#define VFIO_IOMMU_GET_INFO _IO(VFIO_TYPE, VFIO_BASE + 12)

/**
 * VFIO_IOMMU_MAP_DMA - _IOW(VFIO_TYPE, VFIO_BASE + 13, struct vfio_dma_map)
 *
 * Map process virtual addresses to IO virtual addresses using the
 * provided struct vfio_dma_map. Caller sets argsz. READ &/ WRITE required.
 */
struct vfio_iommu_type1_dma_map {
	__u32	argsz;
	__u32	flags;
#define VFIO_DMA_MAP_FLAG_READ (1 << 0)		/* readable from device */
#define VFIO_DMA_MAP_FLAG_WRITE (1 << 1)	/* writable from device */
	__u64	vaddr;				/* Process virtual address */
	__u64	iova;				/* IO virtual address */
	__u64	size;				/* Size of mapping (bytes) */
};

#define VFIO_IOMMU_MAP_DMA _IO(VFIO_TYPE, VFIO_BASE + 13)

/**
 * VFIO_IOMMU_UNMAP_DMA - _IOWR(VFIO_TYPE, VFIO_BASE + 14,
 *							struct vfio_dma_unmap)
 *
 * Unmap IO virtual addresses using the provided struct vfio_dma_unmap.
 * Caller sets argsz.  The actual unmapped size is returned in the size
 * field.  No guarantee is made to the user that arbitrary unmaps of iova
 * or size different from those used in the original mapping call will
 * succeed.
 */
struct vfio_iommu_type1_dma_unmap {
	__u32	argsz;
	__u32	flags;
	__u64	iova;				/* IO virtual address */
	__u64	size;				/* Size of mapping (bytes) */
};

#define VFIO_IOMMU_UNMAP_DMA _IO(VFIO_TYPE, VFIO_BASE + 14)

#endif /* _UAPIVFIO_H */

#include <errno.h>
#include <libgen.h>
#include <fcntl.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/vfs.h>
#include <stdbool.h>

#include <linux/ioctl.h>

void usage(char *name)
{
	printf("usage: %s <iommu group id> <ssss:bb:dd.f>\n", name);
}

#include "header.h"

void read_pci_config(int device)
{
#define VFIO_GET_REGION_ADDR(x) ((uint64_t) x << 40ULL)
#define PCI_CAPABILITY_LIST 0x34
#define PCI_CAP_ID_VNDR     0x09 

    uint8_t pos;
    pread(device, &pos, sizeof(pos),
            VFIO_GET_REGION_ADDR(VFIO_PCI_CONFIG_REGION_INDEX) +
            PCI_CAPABILITY_LIST);
    printf("Capability pointer (offset) is %d\n", pos);

    while (pos) {
        struct virtio_pci_cap cap;
        pread(device, &cap, 2,
                VFIO_GET_REGION_ADDR(VFIO_PCI_CONFIG_REGION_INDEX) +
                pos);
        if (cap.cap_vndr != PCI_CAP_ID_VNDR)
            goto next;

        pread(device, &cap, sizeof(cap),
                VFIO_GET_REGION_ADDR(VFIO_PCI_CONFIG_REGION_INDEX) +
                pos);
        switch (cap.cfg_type) {
            case VIRTIO_PCI_CAP_COMMON_CFG:
                printf("common cfg: offset: %d, bar: %d, len: %d\n",
                        cap.offset, cap.bar, cap.length);
                uint64_t bar_offset = cap.offset + (VFIO_GET_REGION_ADDR(cap.bar));

                bool print_common_cfg = true;
                if (print_common_cfg) {
                    int i;
                    printf("pread: ");
                    for (i = 0; i < 16; i += 4) {
                        uint32_t value;
                        pread(device, &value, 4, VFIO_GET_REGION_ADDR(4) + i);
                        printf("%d, ", value);
                    }
                    uint16_t value = 0;
                    pread(device, &value, 2, VFIO_GET_REGION_ADDR(4) + i);
                    printf("%d, ", value);
                    i += 2;

                    value = 0;
                    pread(device, &value, 2, VFIO_GET_REGION_ADDR(4) + i);
                    printf("%d, ", value);
                    i += 2;

                    value = 0;
                    pread(device, &value, 1, VFIO_GET_REGION_ADDR(4) + i);
                    printf("%d, ", value);
                    i += 1;

                    value = 0;
                    pread(device, &value, 1, VFIO_GET_REGION_ADDR(4) + i);
                    printf("%d\n", value);
                    printf("end\n\n\n");
                }

                uint8_t status_offset = 20;
                uint8_t status;
                pread(device, &status, 1, bar_offset + status_offset);
                printf("VirtIO status (INIT): %d\n", status);

                uint8_t value = VIRTIO_CONFIG_STATUS_RESET;
                pwrite(device, &value, 1, bar_offset + status_offset);

                pread(device, &status, 1, bar_offset + status_offset);
                printf("VirtIO status (RESET): %d\n", status);
                break;
        }
next:
        pos = cap.cap_next;
    }
}

void read_region_info(int device,
        struct vfio_region_info *region_info,
        struct vfio_device_info device_info)
{
    int i;
    for (i = 0; i < device_info.num_regions; i++) {
        if (i == 6) {
            printf("Skip ROM region\n");
            continue;
        }
        printf("Region %d: ", i);
        region_info[i].index = i;
        if (ioctl(device, VFIO_DEVICE_GET_REGION_INFO, &region_info[i])) {
            printf("Failed to get info\n");
            continue;
        }

        printf("size 0x%lx, offset 0x%lx, flags 0x%x\n",
                (unsigned long)region_info[i].size,
                (unsigned long)region_info[i].offset, region_info[i].flags);
    }
}

int main(int argc, char **argv)
{
    int i, ret, container, group, device, groupid;
    char path[PATH_MAX];
    int seg, bus, dev, func;

    struct vfio_group_status group_status = {
        .argsz = sizeof(group_status)
    };

    struct vfio_device_info device_info = {
        .argsz = sizeof(device_info)
    };

    struct vfio_region_info region_info[9];
    for (i = 0; i < 6; i ++) {
        region_info[i].argsz = sizeof(region_info);
    }

    if (argc < 3) {
        usage(argv[0]);
        return -1;
    }

    ret = sscanf(argv[1], "%d", &groupid);
    if (ret != 1) {
        usage(argv[0]);
        return -1;
    }

    ret = sscanf(argv[2], "%04x:%02x:%02x.%d", &seg, &bus, &dev, &func);
    if (ret != 4) {
        usage(argv[0]);
        return -1;
    }

    printf("Using PCI device %04x:%02x:%02x.%d in group %d\n",
            seg, bus, dev, func, groupid);

    //sleep(20);

    container = open("/dev/vfio/vfio", O_RDWR);
    if (container < 0) {
        printf("Failed to open /dev/vfio/vfio, %d (%s)\n",
                container, strerror(errno));
        return container;
    }

    snprintf(path, sizeof(path), "/dev/vfio/noiommu-%d", groupid);
    group = open(path, O_RDWR);
    if (group < 0) {
        printf("Failed to open %s, %d (%s)\n",
                path, group, strerror(errno));
        return group;
    }

    ret = ioctl(group, VFIO_GROUP_GET_STATUS, &group_status);
    if (ret) {
        printf("ioctl(VFIO_GROUP_GET_STATUS) failed\n");
        return ret;
    }

    if (!(group_status.flags & VFIO_GROUP_FLAGS_VIABLE)) {
        printf("Group not viable, are all devices attached to vfio?\n");
        return -1;
    }

    printf("pre-SET_CONTAINER:\n");
    printf("VFIO_CHECK_EXTENSION VFIO_TYPE1_IOMMU: %sPresent\n",
            ioctl(container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU) ?
            "" : "Not ");
    printf("VFIO_CHECK_EXTENSION VFIO_NOIOMMU_IOMMU: %sPresent\n",
            ioctl(container, VFIO_CHECK_EXTENSION, VFIO_NOIOMMU_IOMMU) ?
            "" : "Not ");

    ret = ioctl(group, VFIO_GROUP_SET_CONTAINER, &container);
    if (ret) {
        printf("Failed to set group container\n");
        return ret;
    }

    printf("post-SET_CONTAINER:\n");
    printf("VFIO_CHECK_EXTENSION VFIO_TYPE1_IOMMU: %sPresent\n",
            ioctl(container, VFIO_CHECK_EXTENSION, VFIO_TYPE1_IOMMU) ?
            "" : "Not ");
    printf("VFIO_CHECK_EXTENSION VFIO_NOIOMMU_IOMMU: %sPresent\n",
            ioctl(container, VFIO_CHECK_EXTENSION, VFIO_NOIOMMU_IOMMU) ?
            "" : "Not ");

    ret = ioctl(container, VFIO_SET_IOMMU, VFIO_TYPE1_IOMMU);
    if (!ret) {
        printf("ERROR, was able to use type1 IOMMU with no-iommu\n");
        return -1;
    }

    ret = ioctl(container, VFIO_SET_IOMMU, VFIO_NOIOMMU_IOMMU);
    printf("ioctl: VFIO_SET_IOMMU (NOIOMMU): %d\n", ret);
    if (ret) {
        printf("Failed to set IOMMU\n");
        return ret;
    }

    snprintf(path, sizeof(path), "%04x:%02x:%02x.%d", seg, bus, dev, func);

    device = ioctl(group, VFIO_GROUP_GET_DEVICE_FD, path);
    if (device < 0) {
        printf("Failed to get device %s (%d)\n", path, device);
        return -1;
    }

    if (ioctl(device, VFIO_DEVICE_GET_INFO, &device_info)) {
        printf("Failed to get device info\n");
        return -1;
    }

    printf("Device (%d) supports %d regions, %d irqs\n",
            device, device_info.num_regions, device_info.num_irqs);

    read_region_info(device, region_info, device_info);
    read_pci_config(device);

    printf("Success\n");
    printf("Press any key to exit\n");
    fgetc(stdin);

    return 0;
}

