

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Simple implementation of IoTHub LWM2M Device object
//

//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.

#include "iotdm_internal.h"
#include "iotdm_dispatchers.h"
#include "device_object.h"


/**********************************************************************************
 * Device default property values
 *
 **********************************************************************************/

void set_default_device_property_values(object_device *obj)
{
    set_device_manufacturer(obj->instanceId, "Device_Manufacturer");
    set_device_modelnumber(obj->instanceId, "Device_ModelNumber");
    set_device_serialnumber(obj->instanceId, "Device_SerialNumber");
    set_device_firmwareversion(obj->instanceId, "Device_FirmwareVersion");
    set_device_batterylevel(obj->instanceId, -1);
    set_device_memoryfree(obj->instanceId, -1);
    set_device_currenttime(obj->instanceId, -1);
    set_device_utcoffset(obj->instanceId, "Device_UtcOffset");
    set_device_timezone(obj->instanceId, "Device_Timezone");
    set_device_devicetype(obj->instanceId, "Device_DeviceType");
    set_device_hardwareversion(obj->instanceId, "Device_HardwareVersion");
    set_device_batterystatus(obj->instanceId, -1);
    set_device_memorytotal(obj->instanceId, -1);
}

/**********************************************************************************
 * CALLBACK HANDLERS
 *
 **********************************************************************************/
IOTHUB_CLIENT_RESULT on_read_device_manufacturer(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_manufacturer_read_callback != NULL)
    {
        result = obj->device_manufacturer_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_manufacturer);
        // LogInfo("returning [%s] for Device_Manufacturer\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_modelnumber(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_modelnumber_read_callback != NULL)
    {
        result = obj->device_modelnumber_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_modelnumber);
        // LogInfo("returning [%s] for Device_ModelNumber\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_serialnumber(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_serialnumber_read_callback != NULL)
    {
        result = obj->device_serialnumber_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_serialnumber);
        // LogInfo("returning [%s] for Device_SerialNumber\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_firmwareversion(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_firmwareversion_read_callback != NULL)
    {
        result = obj->device_firmwareversion_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_firmwareversion);
        // LogInfo("returning [%s] for Device_FirmwareVersion\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_exec_device_reboot(object_device *obj)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_NOT_IMPLEMENTED;
    // LogInfo("inside execute handler for Device_Reboot\r\n");
    if (obj->device_reboot_execute_callback != NULL)
    {
        result = obj->device_reboot_execute_callback(obj);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_exec_device_factoryreset(object_device *obj)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_NOT_IMPLEMENTED;
    // LogInfo("inside execute handler for Device_FactoryReset\r\n");
    if (obj->device_factoryreset_execute_callback != NULL)
    {
        result = obj->device_factoryreset_execute_callback(obj);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_batterylevel(object_device *obj, int *value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_batterylevel_read_callback != NULL)
    {
        result = obj->device_batterylevel_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = obj->propval_device_batterylevel;
        // LogInfo("returning %d for Device_BatteryLevel\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_memoryfree(object_device *obj, int *value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_memoryfree_read_callback != NULL)
    {
        result = obj->device_memoryfree_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = obj->propval_device_memoryfree;
        // LogInfo("returning %d for Device_MemoryFree\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_currenttime(object_device *obj, int *value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_currenttime_read_callback != NULL)
    {
        result = obj->device_currenttime_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = obj->propval_device_currenttime;
        // LogInfo("returning %d for Device_CurrentTime\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_write_device_currenttime(object_device *obj, int value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    // LogInfo("Device_CurrentTime being set to %d\r\n", value);
    obj->propval_device_currenttime = value;

    if (obj->device_currenttime_write_callback != NULL)
    {
        result = obj->device_currenttime_write_callback(obj);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_utcoffset(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_utcoffset_read_callback != NULL)
    {
        result = obj->device_utcoffset_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_utcoffset);
        // LogInfo("returning [%s] for Device_UtcOffset\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_write_device_utcoffset(object_device *obj, char* value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    // LogInfo("Device_UtcOffset being set to [%s]\r\n", value);
    lwm2m_free(obj->propval_device_utcoffset);
    obj->propval_device_utcoffset = lwm2m_strdup(value);

    if (obj->device_utcoffset_write_callback != NULL)
    {
        result = obj->device_utcoffset_write_callback(obj);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_timezone(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_timezone_read_callback != NULL)
    {
        result = obj->device_timezone_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_timezone);
        // LogInfo("returning [%s] for Device_Timezone\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_write_device_timezone(object_device *obj, char* value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    // LogInfo("Device_Timezone being set to [%s]\r\n", value);
    lwm2m_free(obj->propval_device_timezone);
    obj->propval_device_timezone = lwm2m_strdup(value);

    if (obj->device_timezone_write_callback != NULL)
    {
        result = obj->device_timezone_write_callback(obj);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_devicetype(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_devicetype_read_callback != NULL)
    {
        result = obj->device_devicetype_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_devicetype);
        // LogInfo("returning [%s] for Device_DeviceType\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_hardwareversion(object_device *obj, char **value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_hardwareversion_read_callback != NULL)
    {
        result = obj->device_hardwareversion_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = lwm2m_strdup(obj->propval_device_hardwareversion);
        // LogInfo("returning [%s] for Device_HardwareVersion\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_batterystatus(object_device *obj, int *value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_batterystatus_read_callback != NULL)
    {
        result = obj->device_batterystatus_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = obj->propval_device_batterystatus;
        // LogInfo("returning %d for Device_BatteryStatus\r\n", *value);
    }
    return result;
}

IOTHUB_CLIENT_RESULT on_read_device_memorytotal(object_device *obj, int *value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_OK;
    if (obj->device_memorytotal_read_callback != NULL)
    {
        result = obj->device_memorytotal_read_callback(obj);
    }
    if (result == IOTHUB_CLIENT_OK)
    {
        *value = obj->propval_device_memorytotal;
        // LogInfo("returning %d for Device_MemoryTotal\r\n", *value);
    }
    return result;
}


