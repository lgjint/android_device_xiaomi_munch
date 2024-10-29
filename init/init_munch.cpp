#include <cstdlib>
#include <string.h>

#define _REALLY_INCLUDE_SYS__SYSTEM_PROPERTIES_H_
#include <sys/_system_properties.h>
#include <android-base/properties.h>

#include "property_service.h"
#include "vendor_init.h"
using android::base::GetProperty;
using std::string;

typedef struct {
    const char* hwc_value;
    const char* sku_value;
    const char* brand;
    const char* device;
    const char* marketname;
    const char* mod_device;
    const char* model;
    const char* build_fingerprint;
    const char* version_incremental;
    const char* user;
    bool nfc;
} variant_info_t;

static const variant_info_t munch_in_info = {
    .hwc_value = "INDIA",
    .sku_value = "",
    .brand = "POCO",
    .device = "munch",
    .marketname = "POCO F4",
    .mod_device = "munch_in_global",
    .model = "22021211RI",
    .build_fingerprint = "POCO/munch_in/munch:13/RKQ1.211001.001/V816.0.5.0.ULMMIXM:user/release-keys",
    .version_incremental = "V816.0.5.0.ULMMIXM",
    .user = "builder",
    .nfc = true,
};

static const variant_info_t munch_global_info = {
    .hwc_value = "GLOBAL",
    .sku_value = "",
    .brand = "POCO",
    .device = "munch",
    .marketname = "POCO F4",
    .mod_device = "munch_global",
    .model = "22021211RG",
    .build_fingerprint = "POCO/munch_global/munch:13/RKQ1.211001.001/V816.0.5.0.ULMMIXM:user/release-keys",
    .version_incremental = "V816.0.5.0.ULMMIXM",
    .user = "builder",
    .nfc = true,
};

static const variant_info_t munch_info = {
    .hwc_value = "CN",
    .sku_value = "",
    .brand = "Redmi",
    .device = "munch",
    .marketname = "Redmi K40S",
    .mod_device = "munch_global",
    .model = "22021211RC",
    .build_fingerprint = "Redmi/munch/munch:13/RKQ1.211001.001/V816.0.5.0.ULMMIXM:user/release-keys",
    .version_incremental = "V816.0.5.0.ULMMIXM",
    .user = "builder",
    .nfc = true,
};

static const std::vector<variant_info_t> variants = {
    munch_in_info,
    munch_global_info,
    munch_info,
};

void property_override(char const prop[], char const value[]) {
    auto pi = (prop_info*) __system_property_find(prop);
    if (pi != nullptr)
        __system_property_update(pi, value, strlen(value));
    else
        __system_property_add(prop, strlen(prop), value, strlen(value));
}

void set_ro_build_prop(const string &source, const string &prop,
                       const string &value, bool product = true) {
    string prop_name;
    if (product)
        prop_name = "ro.product." + source + prop;
    else
        prop_name = "ro." + source + "build." + prop;

    property_override(prop_name.c_str(), value.c_str());
}

void set_device_props(const variant_info_t &info) {
    string source_partitions[] = { "", "bootimage.", "odm.", "product.",
                                   "system.", "system_ext.", "vendor.", "vendor_dlkm." };

    for (const string &source : source_partitions) {
        set_ro_build_prop(source, "brand", info.brand);
        set_ro_build_prop(source, "model", info.model);
        set_ro_build_prop(source, "name", info.device);
        set_ro_build_prop(source, "marketname", info.marketname);
        set_ro_build_prop(source, "user", info.user, false);
        set_ro_build_prop(source, "version.incremental", info.version_incremental, false);
        set_ro_build_prop(source, "fingerprint", info.build_fingerprint, false);
    }

    property_override("ro.vendor_dlkm.build.version.incremental", info.version_incremental);
    property_override("bluetooth.device.default_name", info.marketname);
}

void vendor_load_properties() {
    string region = GetProperty("ro.boot.hwc", "");
    for (const auto &info : variants) {
        if (region == info.hwc_value) {
            set_device_props(info);
            break;
        }
    }
    property_override("ro.boot.hardware.revision", GetProperty("ro.boot.hwversion", "").c_str());
}
