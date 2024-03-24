#include "qemu/osdep.h"
#include "hw/pci/msi.h"
#include "hw/pci/pci.h"
#include <ctype.h>
#include <stdint.h>

#define TYPE_LKP_ENC "lkp_enc"
#define LKP_ENC(obj) OBJECT_CHECK(lkp_enc, (obj), TYPE_LKP_ENC)

typedef struct {
    PCIDevice parent_obj;
    char buffer[128];
    MemoryRegion mmio;
} lkp_enc;


static uint64_t mmio_read(void *opaque, hwaddr addr, unsigned size) {
    lkp_enc *dev;
    dev = (lkp_enc *)opaque;

    // Ensure that the address is within the bounds of the buffer
    if (addr < sizeof(dev->buffer)) {
        // Calculate the offset within the buffer based on the address
        char *buffer_addr = dev->buffer + addr;

        // Return the byte at the calculated offset
        return *buffer_addr;
    } else {
        // Handle out-of-bounds access, for now, returning 0
        return 0;
    }
}

static void mmio_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
    char val_char;
    lkp_enc *dev;
    dev = (lkp_enc *)opaque;

    // Ensure that the address is within the bounds of the buffer
    if (addr < sizeof(dev->buffer)) {
        // Calculate the offset within the buffer based on the address
        char *buffer_addr = dev->buffer + addr;

        val_char = (char) val;
        // Check if the character is lowercase
        if (islower(val_char)) {
            // If lowercase, convert to uppercase
            *buffer_addr = toupper(val_char);
        } else if (isupper(val_char)) {
            // If uppercase, convert to lowercase
            *buffer_addr = tolower(val_char);
        } else {
            // If not a letter, write the value to the buffer
            *buffer_addr = val_char;
        }
    }
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
