

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

//
// Simple implementation of IoTHub LWM2M Firmware Update object
//

//     This code was generated by a tool.
//
//     Changes to this file may cause incorrect behavior and will be lost if
//     the code is regenerated.

#include "iotdm_internal.h"
#include "iotdm_dispatchers.h"
#include "firmwareupdate_object.h"

// Property Indices for for firmwareupdate object
#define INDEX_FIRMWAREUPDATE_PACKAGE 0
#define INDEX_FIRMWAREUPDATE_PACKAGEURI 1
#define INDEX_FIRMWAREUPDATE_UPDATE 2
#define INDEX_FIRMWAREUPDATE_STATE 3
#define INDEX_FIRMWAREUPDATE_UPDATERESULT 4
#define INDEX_FIRMWAREUPDATE_PKGNAME 5
#define INDEX_FIRMWAREUPDATE_PKGVERSION 6

void set_default_firmwareupdate_property_values(object_firmwareupdate *obj);
IOTHUB_CLIENT_RESULT on_write_firmwareupdate_package(object_firmwareupdate *obj, void* value, size_t length);
IOTHUB_CLIENT_RESULT on_write_firmwareupdate_packageuri(object_firmwareupdate *obj, char* value);
IOTHUB_CLIENT_RESULT on_exec_firmwareupdate_update(object_firmwareupdate *obj);
IOTHUB_CLIENT_RESULT on_read_firmwareupdate_state(object_firmwareupdate *obj, int *value);
IOTHUB_CLIENT_RESULT on_read_firmwareupdate_updateresult(object_firmwareupdate *obj, int *value);
IOTHUB_CLIENT_RESULT on_read_firmwareupdate_pkgname(object_firmwareupdate *obj, char **value);
IOTHUB_CLIENT_RESULT on_read_firmwareupdate_pkgversion(object_firmwareupdate *obj, char **value);


/**********************************************************************************
 * Firmware Update setters
 *
 **********************************************************************************/
IOTHUB_CLIENT_RESULT set_firmwareupdate_state(uint16_t instanceId, int value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_ERROR;
    object_firmwareupdate *obj = get_object_instance(OID_FIRMWAREUPDATE, instanceId);
    if (obj != NULL)
    {
        if (obj->propval_firmwareupdate_state != value)
        {
            // LogInfo("FirmwareUpdate_State being set to %d\r\n", value);
            obj->propval_firmwareupdate_state = value;

            obj->resourceUpdated[INDEX_FIRMWAREUPDATE_STATE] = (char)true;
            wake_main_dm_thread(obj->channelHandle);
        }

        result = IOTHUB_CLIENT_OK;

    }
    return result;
}

IOTHUB_CLIENT_RESULT set_firmwareupdate_updateresult(uint16_t instanceId, int value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_ERROR;
    object_firmwareupdate *obj = get_object_instance(OID_FIRMWAREUPDATE, instanceId);
    if (obj != NULL)
    {
        if (obj->propval_firmwareupdate_updateresult != value)
        {
            // LogInfo("FirmwareUpdate_UpdateResult being set to %d\r\n", value);
            obj->propval_firmwareupdate_updateresult = value;

            obj->resourceUpdated[INDEX_FIRMWAREUPDATE_UPDATERESULT] = (char)true;
            wake_main_dm_thread(obj->channelHandle);
        }

        result = IOTHUB_CLIENT_OK;

    }
    return result;
}

IOTHUB_CLIENT_RESULT set_firmwareupdate_pkgname(uint16_t instanceId, char* value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_ERROR;
    object_firmwareupdate *obj = get_object_instance(OID_FIRMWAREUPDATE, instanceId);
    if (obj != NULL)
    {
        if ((obj->propval_firmwareupdate_pkgname == NULL) || strcmp(obj->propval_firmwareupdate_pkgname, value))
        {
            // LogInfo("FirmwareUpdate_PkgName being set to [%s]\r\n", value);
            lwm2m_free(obj->propval_firmwareupdate_pkgname);
            obj->propval_firmwareupdate_pkgname = lwm2m_strdup(value);

            obj->resourceUpdated[INDEX_FIRMWAREUPDATE_PKGNAME] = (char)true;
            wake_main_dm_thread(obj->channelHandle);
        }

        result = IOTHUB_CLIENT_OK;

    }
    return result;
}

IOTHUB_CLIENT_RESULT set_firmwareupdate_pkgversion(uint16_t instanceId, char* value)
{
    IOTHUB_CLIENT_RESULT result = IOTHUB_CLIENT_ERROR;
    object_firmwareupdate *obj = get_object_instance(OID_FIRMWAREUPDATE, instanceId);
    if (obj != NULL)
    {
        if ((obj->propval_firmwareupdate_pkgversion == NULL) || strcmp(obj->propval_firmwareupdate_pkgversion, value))
        {
            // LogInfo("FirmwareUpdate_PkgVersion being set to [%s]\r\n", value);
            lwm2m_free(obj->propval_firmwareupdate_pkgversion);
            obj->propval_firmwareupdate_pkgversion = lwm2m_strdup(value);

            obj->resourceUpdated[INDEX_FIRMWAREUPDATE_PKGVERSION] = (char)true;
            wake_main_dm_thread(obj->channelHandle);
        }

        result = IOTHUB_CLIENT_OK;

    }
    return result;
}


/**********************************************************************************
 * Firmware Update creation and destruction
 *
 **********************************************************************************/
static void destroy_firmwareupdate_object(object_firmwareupdate *obj)
{
    if (obj != NULL)
    {
        lwm2m_free(obj->propval_firmwareupdate_packageuri);
        lwm2m_free(obj->propval_firmwareupdate_pkgname);
        lwm2m_free(obj->propval_firmwareupdate_pkgversion);
        lwm2m_free(obj);
    }
}

static uint16_t g_next_firmwareupdate_instance_id = 0;
static uint16_t get_next_firmwareupdate_instance_id()
{
    return g_next_firmwareupdate_instance_id++;
}

IOTHUB_CLIENT_RESULT create_firmwareupdate_object(IOTHUB_CLIENT_HANDLE h, uint16_t *instanceId)
{
    IOTHUB_CLIENT_RESULT res = IOTHUB_CLIENT_OK;

    res = REGISTER_DISPATCHERS(OID_FIRMWAREUPDATE, object_firmwareupdate);
    if (res != IOTHUB_CLIENT_OK)
    {
        LogError("Failed registering dispatchers");
        res = IOTHUB_CLIENT_ERROR;
    }
    else
    {
        object_firmwareupdate *obj = (object_firmwareupdate *)lwm2m_malloc(sizeof(object_firmwareupdate));
        if (obj == NULL)
        {
            LogError("malloc failure");
            res = IOTHUB_CLIENT_ERROR;
        }
        else
        {
            memset(obj,0,sizeof(*obj));
            obj->instanceId = get_next_firmwareupdate_instance_id();
            obj->channelHandle = h;
                
            res = add_dm_object(&object_firmwareupdate_instance_list, obj);
            if (res != IOTHUB_CLIENT_OK)
            {
                LogError("adding Firmware Update object to object list\r\n");

                destroy_firmwareupdate_object(obj);
                obj = NULL;
            }
            else
            {
                set_default_firmwareupdate_property_values(obj);
            }
        }

        if (instanceId != NULL)
        {
            *instanceId = (obj == NULL) ? 0 : obj->instanceId;
        }
    }

    return res;
}

object_firmwareupdate *get_firmwareupdate_object(uint16_t instanceId)
{
    object_firmwareupdate *obj = NULL;

    (void)get_dm_object(object_firmwareupdate_instance_list, instanceId, (void*)&obj);

    return obj;
}

#define DO_SIGNAL_RESOURCE_CHANGE(index, property) \
    if (obj->resourceUpdated[index]) \
    { \
        on_resource_value_changed(OID_FIRMWAREUPDATE, obj->instanceId, property); \
        obj->resourceUpdated[index] = (char)false; \
    }

   
void signal_object_firmwareupdate_resource_changed(void *p)
{
    object_firmwareupdate *obj = (object_firmwareupdate*)p;
    DO_SIGNAL_RESOURCE_CHANGE(INDEX_FIRMWAREUPDATE_PACKAGE, PID_FIRMWAREUPDATE_PACKAGE)
    DO_SIGNAL_RESOURCE_CHANGE(INDEX_FIRMWAREUPDATE_PACKAGEURI, PID_FIRMWAREUPDATE_PACKAGEURI)
    DO_SIGNAL_RESOURCE_CHANGE(INDEX_FIRMWAREUPDATE_UPDATE, PID_FIRMWAREUPDATE_UPDATE)
    DO_SIGNAL_RESOURCE_CHANGE(INDEX_FIRMWAREUPDATE_STATE, PID_FIRMWAREUPDATE_STATE)
    DO_SIGNAL_RESOURCE_CHANGE(INDEX_FIRMWAREUPDATE_UPDATERESULT, PID_FIRMWAREUPDATE_UPDATERESULT)
    DO_SIGNAL_RESOURCE_CHANGE(INDEX_FIRMWAREUPDATE_PKGNAME, PID_FIRMWAREUPDATE_PKGNAME)
    DO_SIGNAL_RESOURCE_CHANGE(INDEX_FIRMWAREUPDATE_PKGVERSION, PID_FIRMWAREUPDATE_PKGVERSION)
}

BEGIN_READ_DISPATCHER(object_firmwareupdate)
    INTEGER_READ(PID_FIRMWAREUPDATE_STATE, on_read_firmwareupdate_state)
    INTEGER_READ(PID_FIRMWAREUPDATE_UPDATERESULT, on_read_firmwareupdate_updateresult)
    STRING_READ(PID_FIRMWAREUPDATE_PKGNAME, on_read_firmwareupdate_pkgname)
    STRING_READ(PID_FIRMWAREUPDATE_PKGVERSION, on_read_firmwareupdate_pkgversion)
END_READ_DISPATCHER()

BEGIN_EXEC_DISPATCHER(object_firmwareupdate)
    EXEC(PID_FIRMWAREUPDATE_UPDATE, on_exec_firmwareupdate_update)
END_EXEC_DISPATCHER()

BEGIN_WRITE_DISPATCHER(object_firmwareupdate)
    OPAQUE_WRITE(PID_FIRMWAREUPDATE_PACKAGE, on_write_firmwareupdate_package)
    STRING_WRITE(PID_FIRMWAREUPDATE_PACKAGEURI, on_write_firmwareupdate_packageuri)
END_WRITE_DISPATCHER();

BEGIN_OPERATION_LIST(object_firmwareupdate)
    OPERATION(PID_FIRMWAREUPDATE_PACKAGE, OP_W, LWM2M_TYPE_OPAQUE)
    OPERATION(PID_FIRMWAREUPDATE_PACKAGEURI, OP_W, LWM2M_TYPE_STRING)
    OPERATION(PID_FIRMWAREUPDATE_UPDATE, OP_E, LWM2M_TYPE_UNDEFINED)
    OPERATION(PID_FIRMWAREUPDATE_STATE, OP_R, LWM2M_TYPE_INTEGER)
    OPERATION(PID_FIRMWAREUPDATE_UPDATERESULT, OP_R, LWM2M_TYPE_INTEGER)
    OPERATION(PID_FIRMWAREUPDATE_PKGNAME, OP_R, LWM2M_TYPE_STRING)
    OPERATION(PID_FIRMWAREUPDATE_PKGVERSION, OP_R, LWM2M_TYPE_STRING)
END_OPERATION_LIST()


