#include "qemu/osdep.h"
#include "hw/pci/msi.h"
#include "hw/pci/pci.h"

#define TYPE_MY_RNG "my_rng"
#define MY_RNG(obj) OBJECT_CHECK(my_rng, (obj), TYPE_MY_RNG)

typedef struct {
    PCIDevice parent_obj;
    uint32_t seed_register;
    MemoryRegion mmio;
} my_rng;

static uint64_t mmio_read(void *opaque, hwaddr addr, unsigned size) {
    // my_rng *rng = opaque;
    // uint64_t ret = 0;
    // switch (addr) {
    //     case 0:
    //         ret = rng->seed_register;
    //         break;
    //     default:
    //         ret = 0;
    //         break;
    // }
    // return ret;

    /* TODO implement that function later */
    return 0x0;
}

static void mmio_write(void *opaque, hwaddr addr, uint64_t val, unsigned size) {
    /* TODO implement that function later*/
    return;
}

static const MemoryRegionOps my_rng_ops = {
    .read = mmio_read,
    .write = mmio_write,
    // .endianness = DEVICE_NATIVE_ENDIAN,
};

static void my_rng_realize(PCIDevice *pdev, Error **errp) {
    my_rng *s = MY_RNG(pdev);
    memory_region_init_io(&s->mmio, OBJECT(s), &my_rng_ops, s,
                          "my-rng", 4096);
    pci_register_bar(&s->parent_obj, 0, PCI_BASE_ADDRESS_SPACE_MEMORY, &s->mmio);
}

static void my_rng_class_init(ObjectClass *class, void *data) {
    DeviceClass *dc = DEVICE_CLASS(class);
    PCEDeviceClass *k = PCI_DEVICE_CLASS(class);

    k->realize = my_rng_realize;
    k->vendor_id = PCI_VENDOR_ID_QEMU;
    k->device_id = 0xcafe;
    k->revision = 0x10;
    k->class_id = PCI_CLASS_OTHERS;

    set_bit(DEVICE_CATEGORY_MISC, dc->categories);
}

static void my_rng_register_types(void) {
    static InterfaceInfo interfaces[] = {
        { INTERFACE_CONVENTIONAL_PCI_DEVICE },
        { },
    };

    static const TypeInfo my_rng_info = {
        .name = TYPE_MY_RNG,
        .parent = TYPE_PCI_DEVICE,
        .instance_size = sizeof(my_rng),
        .class_init = my_rng_class_init,
        .interfaces = interfaces,
    };

    type_register_static(&my_rng_info);
}

type_init(my_rng_register_types);
