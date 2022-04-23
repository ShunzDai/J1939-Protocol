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
#include "src/memory/j1939_memory.h"
#include "src/port/j1939_port.h"

#if J1939_TRANSPORT_PROTOCOL_ENABLE

#define J1939_SIZE_PROTOCOL_PAYLOAD         (J1939_SIZE_CAN_BUFFER - 1)

#define _GetTotalPackets(Length)            (((Length) - 1) / J1939_SIZE_PROTOCOL_PAYLOAD + 1)
#define _GetLastSection(Length)             (((Length) % J1939_SIZE_PROTOCOL_PAYLOAD) ? ((Length) % J1939_SIZE_PROTOCOL_PAYLOAD) : (J1939_SIZE_PROTOCOL_PAYLOAD))
#define _GetByteOffset(PacketsCount)        (((PacketsCount) - 1) * J1939_SIZE_PROTOCOL_PAYLOAD)

/* Transport Protocol internal status */
typedef enum J1939_TP_Status{
  /* Ready to transmit/receive Transport Protocol messages */
  J1939_TP_READY,
  J1939_TP_COMPLETE_TX,
  J1939_TP_COMPLETE_RX,
  J1939_TP_CM_ABORT_TX,
  J1939_TP_CM_BAM_TX,
  J1939_TP_CM_RTS_TX,
  J1939_TP_CM_CTS_TX,
  J1939_TP_CM_CTS_RX,
  J1939_TP_CM_ACK_TX,
  J1939_TP_CM_ACK_RX,
  J1939_TP_DT_BAM_TX,
  J1939_TP_DT_BAM_RX,
  J1939_TP_DT_CMDT_TX,
  J1939_TP_DT_CMDT_RX,
} J1939_TP_Status_t;

/* Transport Protocol timeout parameters */
typedef enum J1939_Timeout{
  /* Reference SAE J1939-21 5.10.2.4 */
  J1939_TIMEOUT_TR                          = 200,
  J1939_TIMEOUT_TH                          = 500,
  J1939_TIMEOUT_T1                          = 750,
  J1939_TIMEOUT_T2                          = 1250,
  J1939_TIMEOUT_T3                          = 1250,
  J1939_TIMEOUT_T4                          = 1050,
} J1939_Timeout_t;

typedef enum J1939_Control{
  J1939_CONTROL_RTS                         = 0x10U,
  J1939_CONTROL_CTS                         = 0x11U,
  J1939_CONTROL_ACK                         = 0x13U,
  J1939_CONTROL_BAM                         = 0x20U,
  J1939_CONTROL_ABORT                       = 0xFFU,
} J1939_Control_t;

/* Transport protocol - Broadcast announce message structure */
typedef struct J1939_BAM{
  /* Fixed at 32 */
  uint64_t Control                          : 8;
  /* Message size in bytes */
  uint64_t MessageSize                      : 16;
  /* Number of packets */
  uint64_t TotalPackets                     : 8;
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
  uint64_t TotalPackets                     : 8;
  /* Fixed at 0xFF */
  uint64_t Reserved                         : 8;
  /* PGN */
  uint64_t PGN                              : 24;
} J1939_RTS_t;

/* Transport protocol - Clear to send struct */
typedef struct J1939_CTS{
  /* Fixed at 17 */
  uint64_t Control                          : 8;
  /* Max number of packets that can be sent at once. (Not larger than byte 5 of RTS) */
  uint64_t ResponsePackets                  : 8;
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
  uint64_t TotalPackets                     : 8;
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
} J1939_ABORT_t;

struct J1939_Protocol{
  uint8_t TotalPackets;
  uint8_t PacketsCount;
  uint8_t ResponsePackets;
  uint8_t Status;
  J1939_AbortReason_t AbortReason;
  J1939_Message_t Buffer;
  uint64_t Tick;
};

static J1939_Status_t J1939_ProtocolPush(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr, J1939_Status_t Type){
  *MsgPtr = Protocol->Buffer;
  J1939_memset(Protocol, 0, sizeof(struct J1939_Protocol));
  return Type;
}

static J1939_Status_t J1939_TP_DT_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  if (Protocol->Status != J1939_TP_DT_BAM_TX && Protocol->Status != J1939_TP_DT_CMDT_TX)
    return J1939_ERROR;

  uint8_t Section = J1939_SIZE_PROTOCOL_PAYLOAD;

  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);
  (*MsgPtr)->PDU.SourceAddress = Protocol->Buffer->PDU.SourceAddress;
  (*MsgPtr)->PDU.PDUSpecific = Protocol->Status == J1939_TP_DT_BAM_TX ? J1939_ADDRESS_GLOBAL : Protocol->Buffer->PDU.PDUSpecific;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_DT);

  if (++Protocol->PacketsCount == Protocol->TotalPackets){
    Section = _GetLastSection(Protocol->Buffer->Length);
    J1939_memset((*MsgPtr)->Payload + Section + 1, 0xFF, J1939_SIZE_PROTOCOL_PAYLOAD - Section);
    switch (Protocol->Status){
      case J1939_TP_DT_BAM_TX:
        Protocol->Status = J1939_TP_COMPLETE_TX;
        break;
      case J1939_TP_DT_CMDT_TX:
        Protocol->Status = J1939_TP_CM_ACK_RX;
        break;
      default:
        J1939_Assert(0);
        break;
    }
  }
  else if (Protocol->Status == J1939_TP_DT_CMDT_TX){
    if (!--Protocol->ResponsePackets)
      Protocol->Status = J1939_TP_CM_CTS_RX;
  }

  (*MsgPtr)->Payload[0] = Protocol->PacketsCount;

  J1939_memcpy((*MsgPtr)->Payload + 1, Protocol->Buffer->Payload + _GetByteOffset(Protocol->PacketsCount), Section);

  Protocol->Tick = J1939_PortGetTick();

  return J1939_TRANSMIT;
}

static J1939_Status_t J1939_TP_DT_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  if (Protocol->Status != J1939_TP_DT_BAM_RX && Protocol->Status != J1939_TP_DT_CMDT_RX)
    return J1939_ERROR;
  else if (Protocol->PacketsCount + 1 != Msg->Payload[0]){
    /* TODO: TP_CM_CTS_TX */
    return J1939_ERROR;
  }

  uint8_t Section = J1939_SIZE_PROTOCOL_PAYLOAD;

  if (++Protocol->PacketsCount == Protocol->TotalPackets){
    Section = _GetLastSection(Protocol->Buffer->Length);
    switch (Protocol->Status){
      case J1939_TP_DT_BAM_RX:
        Protocol->Status = J1939_TP_COMPLETE_RX;
        break;
      case J1939_TP_DT_CMDT_RX:
        Protocol->Status = J1939_TP_CM_ACK_TX;
        break;
      default:
        break;
    }
  }
  else if (Protocol->Status == J1939_TP_DT_CMDT_RX){
    if (!--Protocol->ResponsePackets)
      Protocol->Status = J1939_TP_CM_CTS_TX;
  }

  J1939_memcpy(Protocol->Buffer->Payload + _GetByteOffset(Protocol->PacketsCount), Msg->Payload + 1, Section);

  Protocol->Tick = J1939_PortGetTick();

  return J1939_OK;
}

static J1939_Status_t J1939_TP_DT_BAM_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  if (J1939_PortGetTick() - Protocol->Tick >= J1939_TP_BAM_TX_INTERVAL)
    return J1939_TP_DT_TransmitManager(Protocol, MsgPtr);

  return J1939_OK;
}

static J1939_Status_t J1939_TP_DT_CMDT_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  if (Protocol->ResponsePackets)
    return J1939_TP_DT_TransmitManager(Protocol, MsgPtr);

  return J1939_ERROR;
}

static J1939_Status_t J1939_TP_CM_RTS_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);

  (*MsgPtr)->PDU.SourceAddress = Protocol->Buffer->PDU.SourceAddress;
  (*MsgPtr)->PDU.PDUSpecific = Protocol->Buffer->PDU.PDUSpecific;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_CM);

  ((J1939_RTS_t *)(*MsgPtr)->Payload)->Control = J1939_CONTROL_RTS;
  ((J1939_RTS_t *)(*MsgPtr)->Payload)->MessageSize = Protocol->Buffer->Length;
  ((J1939_RTS_t *)(*MsgPtr)->Payload)->TotalPackets = Protocol->TotalPackets;
  ((J1939_RTS_t *)(*MsgPtr)->Payload)->PGN = J1939_GetPGN(Protocol->Buffer->ID);
  ((J1939_RTS_t *)(*MsgPtr)->Payload)->Reserved = 0xFF;

  Protocol->Status = J1939_TP_CM_CTS_RX;

  Protocol->Tick = J1939_PortGetTick();

  return J1939_TRANSMIT;
}

static J1939_Status_t J1939_TP_CM_RTS_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  if (Protocol->Status != J1939_TP_READY)
    return J1939_ERROR;

  Protocol->Buffer = J1939_MessageCreate(0, ((J1939_RTS_t *)Msg->Payload)->MessageSize, NULL);
  Protocol->Buffer->PDU.SourceAddress = Msg->PDU.SourceAddress;
  Protocol->Buffer->PDU.PDUSpecific = Msg->PDU.PDUSpecific;
  J1939_SetPGN(&Protocol->Buffer->ID, ((J1939_RTS_t *)Msg->Payload)->PGN);

  Protocol->TotalPackets = ((J1939_RTS_t *)Msg->Payload)->TotalPackets;

  Protocol->Status = J1939_TP_CM_CTS_TX;

  Protocol->Tick = J1939_PortGetTick();

  return J1939_OK;
}

static J1939_Status_t J1939_TP_CM_CTS_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);

  (*MsgPtr)->PDU.SourceAddress = Protocol->Buffer->PDU.PDUSpecific;
  (*MsgPtr)->PDU.PDUSpecific = Protocol->Buffer->PDU.SourceAddress;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_CM);

  ((J1939_CTS_t *)(*MsgPtr)->Payload)->Control = J1939_CONTROL_CTS;
  ((J1939_CTS_t *)(*MsgPtr)->Payload)->NextSequence = Protocol->PacketsCount + 1;
  ((J1939_CTS_t *)(*MsgPtr)->Payload)->PGN = J1939_GetPGN(Protocol->Buffer->ID);
  ((J1939_CTS_t *)(*MsgPtr)->Payload)->Reserved = 0xFFFF;
  ((J1939_CTS_t *)(*MsgPtr)->Payload)->ResponsePackets = (Protocol->TotalPackets - Protocol->PacketsCount <= J1939_TP_CM_CTS_RESPONSE) ? Protocol->TotalPackets - Protocol->PacketsCount : J1939_TP_CM_CTS_RESPONSE;

  Protocol->ResponsePackets = ((J1939_CTS_t *)(*MsgPtr)->Payload)->ResponsePackets;

  Protocol->Status = J1939_TP_DT_CMDT_RX;

  Protocol->Tick = J1939_PortGetTick();

  return J1939_TRANSMIT;
}

static J1939_Status_t J1939_TP_CM_CTS_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  if (Protocol->Status != J1939_TP_CM_CTS_RX)
    return J1939_ERROR;
  else if (J1939_GetPGN(Protocol->Buffer->ID) != ((J1939_CTS_t *)Msg->Payload)->PGN)
    return J1939_ERROR;
  else if (Protocol->PacketsCount + 1 != ((J1939_CTS_t *)Msg->Payload)->NextSequence)
    return J1939_ERROR;

  Protocol->ResponsePackets = ((J1939_CTS_t *)Msg->Payload)->ResponsePackets;

  Protocol->Status = J1939_TP_DT_CMDT_TX;

  Protocol->Tick = J1939_PortGetTick();

  return J1939_OK;
}

static J1939_Status_t J1939_TP_CM_ACK_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);

  (*MsgPtr)->PDU.SourceAddress = Protocol->Buffer->PDU.PDUSpecific;
  (*MsgPtr)->PDU.PDUSpecific = Protocol->Buffer->PDU.SourceAddress;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_CM);

  ((J1939_ACK_t *)(*MsgPtr)->Payload)->Control = J1939_CONTROL_ACK;
  ((J1939_ACK_t *)(*MsgPtr)->Payload)->MessageSize = Protocol->Buffer->Length;
  ((J1939_ACK_t *)(*MsgPtr)->Payload)->PGN = J1939_GetPGN(Protocol->Buffer->ID);
  ((J1939_ACK_t *)(*MsgPtr)->Payload)->Reserved = 0xFF;
  ((J1939_ACK_t *)(*MsgPtr)->Payload)->TotalPackets = Protocol->TotalPackets;

  Protocol->Status = J1939_TP_COMPLETE_RX;

  Protocol->Tick = J1939_PortGetTick();

  return J1939_TRANSMIT;
}

static J1939_Status_t J1939_TP_CM_ACK_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  if (Protocol->Status != J1939_TP_CM_ACK_RX)
    return J1939_ERROR;
  else if (J1939_GetPGN(Protocol->Buffer->ID) != ((J1939_ACK_t *)Msg->Payload)->PGN)
    return J1939_ERROR;
  else if (Protocol->Buffer->Length != ((J1939_ACK_t *)Msg->Payload)->MessageSize)
    return J1939_ERROR;
  else if (Protocol->TotalPackets != ((J1939_ACK_t *)Msg->Payload)->TotalPackets)
    return J1939_ERROR;

  Protocol->Status = J1939_TP_COMPLETE_TX;

  return J1939_OK;
}

static J1939_Status_t J1939_TP_CM_BAM_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);

  (*MsgPtr)->PDU.SourceAddress = Protocol->Buffer->PDU.SourceAddress;
  (*MsgPtr)->PDU.PDUSpecific = J1939_ADDRESS_GLOBAL;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_CM);

  ((J1939_BAM_t *)(*MsgPtr)->Payload)->Control = J1939_CONTROL_BAM;
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->MessageSize = Protocol->Buffer->Length;
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->TotalPackets = Protocol->TotalPackets;
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->PGN = J1939_GetPGN(Protocol->Buffer->ID);
  ((J1939_BAM_t *)(*MsgPtr)->Payload)->Reserved = 0xFF;

  Protocol->Status = J1939_TP_DT_BAM_TX;
  Protocol->Tick = J1939_PortGetTick();

  return J1939_TRANSMIT;
}

static J1939_Status_t J1939_TP_CM_BAM_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  if (Protocol->Status != J1939_TP_READY)
    return J1939_ERROR;

  Protocol->Buffer = J1939_MessageCreate(0, ((J1939_BAM_t *)Msg->Payload)->MessageSize, NULL);
  Protocol->Buffer->PDU.SourceAddress = Msg->PDU.SourceAddress;
  Protocol->Buffer->PDU.PDUSpecific = Msg->PDU.PDUSpecific;
  J1939_SetPGN(&Protocol->Buffer->ID, ((J1939_BAM_t *)Msg->Payload)->PGN);

  Protocol->TotalPackets = ((J1939_BAM_t *)Msg->Payload)->TotalPackets;

  Protocol->Status = J1939_TP_DT_BAM_RX;
  Protocol->Tick = J1939_PortGetTick();

  return J1939_OK;
}

static J1939_Status_t J1939_TP_CM_ABORT_TransmitManager(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  *MsgPtr = J1939_MessageCreate(0, J1939_SIZE_CAN_BUFFER, NULL);

  (*MsgPtr)->PDU.SourceAddress = Protocol->Buffer->PDU.PDUSpecific;
  (*MsgPtr)->PDU.PDUSpecific = Protocol->Buffer->PDU.SourceAddress;
  (*MsgPtr)->PDU.Priority = J1939_TP_DEFAULT_PRIORITY;
  J1939_SetPGN(&(*MsgPtr)->ID, J1939_PGN_TP_CM);

  ((J1939_ABORT_t *)(*MsgPtr)->Payload)->Control = J1939_CONTROL_ABORT;
  ((J1939_ABORT_t *)(*MsgPtr)->Payload)->Reason = (uint8_t)Protocol->AbortReason;
  ((J1939_ABORT_t *)(*MsgPtr)->Payload)->Reserved = 0xFFFFFF;
  ((J1939_ABORT_t *)(*MsgPtr)->Payload)->PGN = J1939_GetPGN(Protocol->Buffer->ID);

  J1939_memset(Protocol, 0, sizeof(struct J1939_Protocol));

  return J1939_OK;
}

static J1939_Status_t J1939_TP_CM_ABORT_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  if (J1939_GetPGN(Protocol->Buffer->ID) != ((J1939_ABORT_t *)Msg->Payload)->PGN)
    return J1939_ERROR;

  J1939_MessageDelete(&Protocol->Buffer);

  J1939_memset(Protocol, 0, sizeof(struct J1939_Protocol));

  return J1939_OK;
}

/**
  * @brief  J1939 transport protocol connection management message receive manager
  * @param  Protocol J1939 transport protocol handle
  * @param  Msg Received message
  * @retval J1939 status
  */
static J1939_Status_t J1939_TP_CM_ReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  switch ((J1939_Control_t)Msg->Payload[0]){
    case J1939_CONTROL_RTS:
      return J1939_TP_CM_RTS_ReceiveManager(Protocol, Msg);
    case J1939_CONTROL_CTS:
      return J1939_TP_CM_CTS_ReceiveManager(Protocol, Msg);
    case J1939_CONTROL_ACK:
      return J1939_TP_CM_ACK_ReceiveManager(Protocol, Msg);
    case J1939_CONTROL_BAM:
      return J1939_TP_CM_BAM_ReceiveManager(Protocol, Msg);
    case J1939_CONTROL_ABORT:
      return J1939_TP_CM_ABORT_ReceiveManager(Protocol, Msg);
    default:
      return J1939_ERROR;
  }
}

/**
  * @brief  J1939 transport protocol create
  * @param  void
  * @retval J1939 transport protocol handle
  */
J1939_Protocol_t J1939_ProtocolCreate(void){
  J1939_Protocol_t Protocol = (J1939_Protocol_t)J1939_malloc(sizeof(struct J1939_Protocol));
  J1939_memset(Protocol, 0, sizeof(struct J1939_Protocol));
  return Protocol;
}

/**
  * @brief  J1939 transport protocol delete
  * @param  Protocol J1939 transport protocol handle printer
  * @retval J1939 status
  */
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

/**
  * @brief  J1939 transport protocol receive manager
  * @param  Protocol J1939 transport protocol handle
  * @param  Msg The message to be sent
  * @retval J1939 status
  */
J1939_Status_t J1939_ProtocolTransmitManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  if (Protocol->Status != J1939_TP_READY)
    return J1939_BUSY;
  else if (Msg->Length > J1939_TP_BUFFER_SIZE)
    return J1939_ERROR;

  Protocol->Buffer = Msg;
  Protocol->TotalPackets = _GetTotalPackets(Msg->Length);

  if (Msg->PDU.PDUFormat >= J1939_ADDRESS_DIVIDE)
    Protocol->Status = J1939_TP_CM_BAM_TX;
  else
    Protocol->Status = J1939_TP_CM_RTS_TX;

  return J1939_OK;
}

/**
  * @brief  J1939 transport protocol receive manager
  * @param  Protocol J1939 transport protocol handle
  * @param  Msg Received message
  * @retval J1939 status
  */
J1939_Status_t J1939_ProtocolReceiveManager(J1939_Protocol_t Protocol, J1939_Message_t Msg){
  switch (J1939_GetPGN(Msg->ID)){
    case J1939_PGN_TP_CM:
      J1939_TP_CM_ReceiveManager(Protocol, Msg);
      return J1939_OK;/* Filter */
    case J1939_PGN_TP_DT:
      J1939_TP_DT_ReceiveManager(Protocol, Msg);
      return J1939_OK;/* Filter */
    default:
      return J1939_ERROR;
  }
}

J1939_Status_t J1939_ProtocolTaskHandler(J1939_Protocol_t Protocol, J1939_Message_t *MsgPtr){
  switch (Protocol->Status){
    case J1939_TP_READY:
      return J1939_OK;
    case J1939_TP_COMPLETE_TX:
      J1939_MessageDelete(&Protocol->Buffer);
      J1939_memset(Protocol, 0, sizeof(struct J1939_Protocol));
      return J1939_OK;
    case J1939_TP_COMPLETE_RX:
      return J1939_ProtocolPush(Protocol, MsgPtr, J1939_RECEIVED);
    case J1939_TP_CM_ABORT_TX:
      return J1939_TP_CM_ABORT_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_CM_BAM_TX:/* TP BAM start */
      return J1939_TP_CM_BAM_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_CM_RTS_TX:/* TP CMDT start */
      return J1939_TP_CM_RTS_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_CM_CTS_TX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_TR) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_TP_CM_CTS_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_CM_CTS_RX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_T3) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_OK;
    case J1939_TP_CM_ACK_TX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_TR) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_TP_CM_ACK_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_CM_ACK_RX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_T3) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_OK;
    case J1939_TP_DT_BAM_TX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_TR) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_TP_DT_BAM_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_DT_BAM_RX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_T1) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_OK;
    case J1939_TP_DT_CMDT_TX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_T3) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_TP_DT_CMDT_TransmitManager(Protocol, MsgPtr);
    case J1939_TP_DT_CMDT_RX:
      return (J1939_PortGetTick() - Protocol->Tick >= J1939_TIMEOUT_T3) ? J1939_ProtocolPush(Protocol, MsgPtr, J1939_TIMEOUT) : J1939_OK;
    default:
      return J1939_ERROR;
  }
}

J1939_Status_t J1939_ProtocolStatus(J1939_Protocol_t Protocol){
  return (Protocol->Status == J1939_TP_READY) ? J1939_OK : J1939_BUSY;
}

J1939_Status_t J1939_ProtocolAbort(J1939_Protocol_t Protocol, J1939_AbortReason_t AbortReason){
  switch (Protocol->Status){
    case J1939_TP_READY:
      return J1939_ERROR;
    case J1939_TP_COMPLETE_TX:
      return J1939_ERROR;
    case J1939_TP_COMPLETE_RX:
      return J1939_ERROR;
    case J1939_TP_CM_ABORT_TX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    case J1939_TP_CM_BAM_TX:
      return J1939_ERROR;
    case J1939_TP_CM_RTS_TX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    case J1939_TP_CM_CTS_TX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    case J1939_TP_CM_CTS_RX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    case J1939_TP_CM_ACK_TX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    case J1939_TP_CM_ACK_RX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    case J1939_TP_DT_BAM_TX:
      return J1939_ERROR;
    case J1939_TP_DT_BAM_RX:
      return J1939_ERROR;
    case J1939_TP_DT_CMDT_TX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    case J1939_TP_DT_CMDT_RX:
      Protocol->AbortReason = AbortReason;
      return J1939_OK;
    default:
      return J1939_ERROR;
  }
}

#endif /* J1939_TRANSPORT_PROTOCOL_ENABLE */
