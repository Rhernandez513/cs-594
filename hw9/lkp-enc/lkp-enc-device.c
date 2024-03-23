#include "qemu/osdep.h"
#include "hw/pci/msi.h"
#include "hw/pci/pci.h"
#include <stdio.h>
#include <ctype.h>

#define TYPE_LKP_ENC "lkp_enc"
#define LKP_ENC(obj) OBJECT_CHECK(lkp_enc, (obj), TYPE_LKP_ENC)

typedef struct {
    PCIDevice parent_obj;
    char *buffer;
    MemoryRegion mmio;
} lkp_enc;

static uint64_t mmio_read(void *opaque, hwaddr addr, unsigned size) {
    lkp_enc *dev;
    dev = (lkp_enc *)opaque;

     // Casting the buffer pointer to uint64_t and returning it
    return (uint64_t)dev->buffer;
}

static void mmio_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
    char *ptr;
    lkp_enc *dev;

    // Cast the opaque pointer to lkp_enc
    dev = (lkp_enc *)opaque;

    // Set the buffer inside the device memory to the address of the value
    // dev->buffer = (char *) &val;

    // copy the contents of val to the buffer
    snprintf(dev->buffer, size + 1, "%s", (char *)&val);

    // create an iterator pointer to the buffer
    ptr = dev->buffer;

    // Loop through each character until the terminating character is reached
    while (*ptr != '\0') {
        // Check if the character is lowercase
        if (islower(*ptr)) {
            // If lowercase, convert to uppercase
            *ptr = toupper(*ptr);
        } else if (isupper(*ptr)) {
            // If uppercase, convert to lowercase
            *ptr = tolower(*ptr);
        }
        // Move to the next character in the buffer
        ptr++;
    }

    return;
}

static const MemoryRegionOps lkp_enc_ops = {
    .read = mmio_read,
    .write = mmio_write,
};

static void lkp_enc_realize(PCIDevice *pdev, Error **errp) {
    lkp_enc *s = LKP_ENC(pdev);
    memory_region_init_io(&s->mmio, OBJECT(s), &lkp_enc_ops, s,
                          "lkp-enc", 4096);
    pci_register_bar(&s->parent_obj, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mmio);
}

static void lkp_enc_class_init(ObjectClass *class, void *data) {
    DeviceClass *dc = DEVICE_CLASS(class);
    PCIDeviceClass *k = PCI_DEVICE_CLASS(class);

    k->realize = lkp_enc_realize;
    k->vendor_id = PCI_VENDOR_ID_QEMU;
    k->device_id = 0xbeef;
    k->revision = 0x10;
    k->class_id = PCI_CLASS_OTHERS;

    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static void lkp_enc_register_types(void) {
    static InterfaceInfo interfaces[] = {
        { INTERFACE_CONVENTIONAL_PCI_DEVICE },
        { },
    };

    static const TypeInfo lkp_enc_info = {
        .name = TYPE_LKP_ENC,
        .parent = TYPE_PCI_DEVICE,
        .instance_size = sizeof(lkp_enc),
        .class_init = lkp_enc_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&lkp_enc_info);
}

type_init(lkp_enc_register_types);
