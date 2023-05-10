#include <unistd.h>
#include <stdint.h>

struct virtio_pci_cap {
    uint8_t cap_vndr;       /* Generic PCI field: PCI_CAP_ID_VNDR */
    uint8_t cap_next;       /* Generic PCI field: next ptr. */
    uint8_t cap_len;        /* Generic PCI field: capability length */
    uint8_t cfg_type;       /* Identifies the structure. */
    uint8_t bar;            /* Where to find it. */
    uint8_t padding[3];     /* Pad to full dword. */
    uint32_t offset;        /* Offset within bar. */
    uint32_t length;        /* Length of the structure, in bytes. */
};

/* Common configuration */
#define VIRTIO_PCI_CAP_COMMON_CFG   1
/* Notifications */
#define VIRTIO_PCI_CAP_NOTIFY_CFG   2
/* ISR Status */
#define VIRTIO_PCI_CAP_ISR_CFG      3
/* Device specific configuration */
#define VIRTIO_PCI_CAP_DEVICE_CFG   4
/* PCI configuration access */
#define VIRTIO_PCI_CAP_PCI_CFG      5

#define VIRTIO_CONFIG_STATUS_RESET      0x00
#define VIRTIO_CONFIG_STATUS_ACK        0x01
#define VIRTIO_CONFIG_STATUS_DRIVER     0x02
#define VIRTIO_CONFIG_STATUS_DRIVER_OK      0x04
#define VIRTIO_CONFIG_STATUS_FEATURES_OK    0x08
#define VIRTIO_CONFIG_STATUS_DEV_NEED_RESET 0x40
#define VIRTIO_CONFIG_STATUS_FAILED     0x80
