/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *    Julien Vermillard, Sierra Wireless
 *    Bosch Software Innovations GmbH - Please refer to git log
 *    Pascal Rieux - Please refer to git log
 *    
 *******************************************************************************/

/*
 *  Resources:
 *
 *          Name         | ID | Operations | Instances | Mandatory |  Type   |  Range  | Units |
 *  Short ID             |  0 |     R      |  Single   |    Yes    | Integer | 1-65535 |       |
 *  Lifetime             |  1 |    R/W     |  Single   |    Yes    | Integer |         |   s   |
 *  Default Min Period   |  2 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Default Max Period   |  3 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Disable              |  4 |     E      |  Single   |    No     |         |         |       |
 *  Disable Timeout      |  5 |    R/W     |  Single   |    No     | Integer |         |   s   |
 *  Notification Storing |  6 |    R/W     |  Single   |    Yes    | Boolean |         |       |
 *  Binding              |  7 |    R/W     |  Single   |    Yes    | String  |         |       |
 *  Registration Update  |  8 |     E      |  Single   |    Yes    |         |         |       |
 *
 */

#include "liblwm2m.h"
#include "wakaama_client_internal.h"
#include "wakaama_object_utils.h"

static lwm2m_object_with_meta_t server_object = {0};

static bool server_object_write_verify_cb(lwm2m_list_t* instance, uint16_t changed_res_id);

OBJECT_META(server_instance_t, server_object_meta, server_object_write_verify_cb,
        {O_RES_R|O_RES_UINT16                 ,  offsetof(server_instance_t,shortServerId)},
        {O_RES_R|O_RES_W|O_RES_UINT32,  offsetof(server_instance_t,lifetime)},
        {O_RES_R|O_RES_W|O_RES_UINT32,  offsetof(server_instance_t,defaultMinPeriod)},
        {O_RES_R|O_RES_W|O_RES_UINT32,  offsetof(server_instance_t,defaultMaxPeriod)},
        {O_RES_E                                     ,  0},
        {O_RES_R|O_RES_W|O_RES_UINT32,  offsetof(server_instance_t,disableTimeout)},
        {O_RES_R|O_RES_W|O_RES_BOOL  ,  offsetof(server_instance_t,storing)},
        {O_RES_R|O_RES_W|O_RES_STRING_PREALLOC,  offsetof(server_instance_t,binding)},
        {O_RES_E                                     ,  0}
);

static bool server_object_write_verify_cb(lwm2m_list_t* instance, uint16_t changed_res_id) {
    server_instance_t* i = (server_instance_t*)instance;

    if(changed_res_id==7) {
        int len = strlen(i->binding);
        if (!(len > 0 && len <= 3)
                && (strncmp(i->binding, "U",   len) == 0
                 || strncmp(i->binding, "UQ",  len) == 0
                 || strncmp(i->binding, "S",   len) == 0
                 || strncmp(i->binding, "SQ",  len) == 0
                 || strncmp(i->binding, "US",  len) == 0
                 || strncmp(i->binding, "UQS", len) == 0))
            return false;
    }
    return true;
}

static uint8_t object_execute_cb(uint16_t instanceId,
                        uint16_t resourceId,
                        uint8_t * buffer,
                        int length,
                        lwm2m_object_t * objectP)
{
    server_instance_t * targetP;

    targetP = (server_instance_t *)lwm2m_list_find(objectP->instanceList, instanceId);
    if (NULL == targetP)
        return COAP_404_NOT_FOUND;

    switch (resourceId)
    {
    case LWM2M_SERVER_DISABLE_ID:
    case LWM2M_SERVER_UPDATE_ID:
        // executed in core, if COAP_204_CHANGED is returned
        return COAP_204_CHANGED;
    default:
        return COAP_405_METHOD_NOT_ALLOWED;
    }
}

lwm2m_object_t * init_server_object()
{
    lwm2m_object_create_preallocated(&server_object, 1, false, &server_object_meta);
    return (lwm2m_object_t*)&server_object;
}
