/**
  * Copyright 2022 ShunzDai
  *
  * Licensed under the Apache License, Version 2.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  *     http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  */
#include "j1939_protocol.h"
#include "j1939_config.h"
#include "src/message/j1939_message.h"
#include "src/port/j1939_memory.h"
#include "src/port/j1939_port.h"

#if J1939_ENABLE_TRANSPORT_PROTOCOL

/* Reference SAE J1939-21 5.10.1.1 */
/* min size = 9, max size = 1785 */
#define J1939_TP_BUFFER_SIZE                1785
#define J1939_TP_DEFAULT_PRIORITY           0x07
#define J1939_TP_CMDT_ONCE                  4
#define J1939_TP_BAM_INTERVAL               50

#define __GetTotalPackets(Length)           (((Length) % 7 == 0) ? ((Length)/7) : (((Length) - (Length) % 7)/7 + 1))
#define __GetLastSection(Length)            (((Length) % 7 == 0) ? (7) : ((Length) % 7))
#define __GetByteOffset(PacketsCount)       (7*((PacketsCount) - 1))

/* Transport Protocol internal status */
typedef enum J1939_TPStatus{
  /* Ready to transmit/receive Transport Protocol messages */
  J1939_TP_READY,
  J1939_TP_CM_BAM_TX,
  J1939_TP_DT_BAM_TX,
  J1939_TP_DT_BAM_RX,
} J1939_TPStatus_t;

/* Transport Protocol timeout parameters */
typedef enum J1939_Timeout{
  /* Reference SAE J1939-21 5.10.2.4 */
  J1939_Tr                                  = 200,
  J1939_Th                                  = 500,
  J1939_T1                                  = 750,
  J1939_T2                                  = 1250,
  J1939_T3                                  = 1250,
  J1939_T4                                  = 1050,
} J1939_Timeout_t;

typedef enum J1939_Control{
  CONTROL_RTS                               = 0x10U,
  CONTROL_CTS                               = 0x11U,
  CONTROL_ENDACK                            = 0x13U,
  CONTROL_BAM                               = 0x20U,
  CONTROL_ABORT                             = 0xFFU,
} J1939_Control_t;

/* Transport protocol - Broadcast announce message structure */
typedef struct J1939_BAM{
  /* Fixed at 32 */
  uint64_t Control                          : 8;
  /* Message size in bytes */
  uint64_t MessageSize                      : 16;
  /* Number of packets */
  uint64_t NumberofPackets                  : 8;
  /* Fixed at 0xFF */
  uint64_t Reserved                         : 8;
  /* PGN */
  uint64_t PGN                              : 24;
} J1939_BAM_t;

/* Transport protocol - Request to send struct */
typedef struct J1939_RTS{
  /* Fixed at 16 */
  uint64_t Control                          : 8;
  /* Message size in bytes */
  uint64_t MessageSize                      : 16;
  /* Number of packets */
  uint64_t NumberofPackets                  : 8;
  /* Total number of packet sent in response to CTS. */
  uint64_t TotalResponse                    : 8;
  /* PGN */
  uint64_t PGN                              : 24;
} J1939_RTS_t;

/* Transport protocol - Clear to send struct */
typedef struct J1939_CTS{
  /* Fixed at 17 */
  uint64_t Control                          : 8;
  /* Max number of packets that can be sent at once. (Not larger than byte 5 of RTS) */
  uint64_t MaxPackets                       : 8;
  /* Next sequence number to start with */
  uint64_t NextSequence                     : 8;
  /* Fixed at 0xFFFF */
  uint64_t Reserved                         : 16;
  /* PGN */
  uint64_t PGN                              : 24;
} J1939_CTS_t;

/* Transport protocol - EndofMsgACK_t struct */
typedef struct J1939_ACK{
  /* Fixed at 19 */
  uint64_t Control                          : 8;
  /* Total message size in bytes */
  uint64_t MessageSize                      : 16;
  /* Total number of packets */
  uint64_t NumberofPackets                  : 8;
  /* Fixed at 0xFF */
  uint64_t Reserved                         : 8;
  /* PGN */
  uint64_t PGN                              : 24;
} J1939_ACK_t;

/* Transport protocol - Connection abort struct */
typedef struct J1939_Abort{
  /* Fixed at 255 */
  uint64_t Control                          : 8;
  /* Connection abort reason */
  uint64_t Reason                           : 8;
  /* Fixed at 0xFFFFFF */
  uint64_t Reserved                         : 24;
  /* PGN */
  uint64_t PGN                              : 24;
} J1939_Abort_t;

struct J1939_Protocol{
  J1939_TPStatus_t Status;
  J1939_Message_t Buffer;
  uint8_t TotalPackets;
  uint8_t PacketsCount;
  uint64_t Tick;
};

static J1939_Status_t J1939_ProtocolReset(J1939_Protocol_t Protocol){
  Protocol->Status = J1939_TP_READY;
  J1939_MessageDelete(&Protocol->Buffer);
  Protocol->TotalPackets = 0;
  Protocol->PacketsCount = 0;
  Protocol->Tick = 0;
  return J1939_OK;
}

static J1939_Status_t J1939_TP_DT_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  uint8_t Section = J1939_SIZE_CAN_BUFFER - 1;

  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);
  (*MsgPtr)->PDU.PDUSpecific = J1939_ADDRESS_GLOBAL;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_DT);

  if (Protocol->TotalPackets == ++Protocol->PacketsCount){
    Section = __GetLastSection(Protocol->Buffer->Length);
    (*MsgPtr)->Payload[0] = Protocol->PacketsCount;
    J1939_memcpy((*MsgPtr)->Payload + 1, Protocol->Buffer->Payload + __GetByteOffset(Protocol->PacketsCount), Section);
    J1939_memset((*MsgPtr)->Payload + Section + 1, 0xFF, J1939_SIZE_CAN_BUFFER - 1 - Section);
    J1939_ProtocolReset(Protocol);
  }
  else{
    (*MsgPtr)->Payload[0] = Protocol->PacketsCount;
    J1939_memcpy((*MsgPtr)->Payload + 1, Protocol->Buffer->Payload + __GetByteOffset(Protocol->PacketsCount), Section);
    Protocol->Tick = J1939_PortGetTick();
  }

  return J1939_TRANSMIT;
}

static J1939_Status_t J1939_TP_DT_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  uint8_t Section = J1939_SIZE_CAN_BUFFER - 1;
  if (Msg->Payload[0] == ++Protocol->PacketsCount){
    if (Protocol->PacketsCount == Protocol->TotalPackets)
        Section = __GetLastSection(Protocol->Buffer->Length);
    J1939_memcpy(Protocol->Buffer->Payload + __GetByteOffset(Protocol->PacketsCount), Msg->Payload + 1, Section);
    Protocol->Tick = J1939_PortGetTick();
    return J1939_OK;
  }
  else{
    Protocol->PacketsCount--;
    return J1939_ERROR;
  }
}

static J1939_Status_t J1939_TP_DT_Decode(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  *MsgPtr = Protocol->Buffer;
  Protocol->Status = J1939_TP_READY;
  Protocol->TotalPackets = 0;
  Protocol->PacketsCount = 0;
  Protocol->Tick = 0;
  return J1939_RECEIVED;
}

static J1939_Status_t J1939_TP_CM_BAM_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);

  (*MsgPtr)->PDU.SourceAddress = Protocol->Buffer->PDU.SourceAddress;
  (*MsgPtr)->PDU.PDUSpecific = J1939_ADDRESS_GLOBAL;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_CM);

  ((J1939_BAM_t *)(*MsgPtr)->Payload)->Control = CONTROL_BAM;
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->MessageSize = Protocol->Buffer->Length;
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->NumberofPackets = Protocol->TotalPackets;
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->PGN = J1939_GetPGN(Protocol->Buffer->ID);
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->Reserved = 0xFF;

  Protocol->Status = J1939_TP_DT_BAM_TX;
  Protocol->Tick = J1939_PortGetTick();

  return J1939_TRANSMIT;
}

static J1939_Status_t J1939_TP_CM_BAM_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  Protocol->Buffer = J1939_MessageCreate(Msg->ID, ((J1939_BAM_t *)Msg->Payload)->MessageSize, NULL);
  J1939_SetPGN(&Protocol->Buffer->ID, ((J1939_BAM_t *)Msg->Payload)->PGN);

  Protocol->TotalPackets = ((J1939_BAM_t *)Msg->Payload)->NumberofPackets;

  Protocol->Status = J1939_TP_DT_BAM_RX;
  Protocol->Tick = J1939_PortGetTick();

  return J1939_OK;
}

static J1939_Status_t J1939_TP_CM_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  switch ((J1939_Control_t)Msg->Payload[0]){
    case CONTROL_BAM:
      return J1939_TP_CM_BAM_ReceiveManager(Protocol, Msg);
    default:
      return J1939_ERROR;
  }
}

J1939_Protocol_t J1939_ProtocolCreate(void){
  J1939_Protocol_t Protocol = (J1939_Protocol_t)J1939_malloc(sizeof(struct J1939_Protocol));
  J1939_memset(Protocol, 0, sizeof(struct J1939_Protocol));
  return Protocol;
}

J1939_Status_t J1939_ProtocolDelete(J1939_Protocol_t *Protocol){
  if (Protocol == NULL){
    J1939_LOG_ERROR("[Protocol]A null pointer appears");
    return J1939_ERROR;
  }
  else if (*Protocol == NULL){
    return J1939_OK;
  }
  else{
    if ((*Protocol)->Buffer != NULL){
      J1939_MessageDelete(&(*Protocol)->Buffer);
    }
    J1939_free(*Protocol);
    *Protocol = NULL;
    return J1939_OK;
  }
}

J1939_Status_t J1939_ProtocolTransmitManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  Protocol->Buffer = J1939_MessageCopy(Msg);
  Protocol->TotalPackets = __GetTotalPackets(Msg->Length);

  if (Msg->PDU.PDUFormat >= J1939_ADDRESS_DIVIDE)
    Protocol->Status = J1939_TP_CM_BAM_TX;
  else
    ;

  return J1939_OK;
}

J1939_Status_t J1939_ProtocolReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  switch (J1939_GetPGN(Msg->ID)){
    case J1939_PGN_TP_CM:
      return J1939_TP_CM_ReceiveManager(Protocol, Msg);
    case J1939_PGN_TP_DT:
      return J1939_TP_DT_ReceiveManager(Protocol, Msg);
    default:
      return J1939_ERROR;
  }
}

J1939_Status_t J1939_ProtocolTaskHandler(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  switch (Protocol->Status){
    case J1939_TP_READY:
      return J1939_OK;
    case J1939_TP_CM_BAM_TX:
      return J1939_TP_CM_BAM_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_DT_BAM_TX:
      if (J1939_PortGetTick() - Protocol->Tick >= J1939_TP_BAM_INTERVAL)
        return J1939_TP_DT_TransmitManager(Protocol, MsgPtr);
      else
        return J1939_OK;
    case J1939_TP_DT_BAM_RX:
      if (Protocol->PacketsCount == Protocol->TotalPackets && Protocol->PacketsCount != 0)
        return J1939_TP_DT_Decode(Protocol, MsgPtr);
      else
        return J1939_OK;
    default:
      return J1939_ERROR;
  }
}

#endif /* J1939_ENABLE_TRANSPORT_PROTOCOL */
