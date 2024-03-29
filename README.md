# SAE-J1939 协议

<p align="center">

  <img src="https://img.shields.io/badge/license-Apache%202-blue.svg">
</p>

## 简介

本项目是对SAE-J1939协议的C语言实现，具有以下特点

- 较高的可移植性. 本项目对接硬件衔接层的接口文件为[src/port/j1939_port.c](src/port/j1939_port.c)和[src/port/j1939_port.h](src/port/j1939_port.h)，用户可在[config/j1939_config.h](config/j1939_config.h)中选择硬件衔接层接口.

- 较高的封装程度. 本项目封装了大部分内部处理函数与数据结构，对外仅留出必要的用户接口.

<center>

| 结构体 | 名称 | 描述 |
| --- | --- | --- |
| `typedef struct J1939 * J1939_t` | J1939控制句柄 | 不完整类型指针，`struct J1939`被封装 |
| `typedef struct J1939_Message * J1939_Message_t` | J1939消息 | 用于存储CAN ID，消息字节数，消息载荷 |
| `typedef struct J1939_PDU J1939_PDU_t` | J1939协议数据单元 | 仅用于J1939消息结构体中的ID联合体 |

</center>

<center>

| 接口函数 | 描述 | 输入 | 输出 |
| --- | --- | --- | --- |
| `J1939_GetPGN` | 获取消息PGN | 消息ID | PGN |
| `J1939_SetPGN` | 设置消息PGN | 消息ID指针, PGN | J1939状态 |
| `J1939_MessageCreate` | 创建J1939消息(长短报文复用此消息创建方法) | 消息ID，消息长度，载荷指针 | J1939消息句柄 |
| `J1939_MessageDelete` | 释放J1939消息 | J1939消息指针 | J1939状态 |
| `J1939_MessageCopy` | 复制一个J1939消息 | 将要复制的J1939消息 | 复制后的J1939消息 |
| `J1939_HandleCreate` | 创建J1939控制句柄 | HAL接口名称，设备地址，发送队列最大长度 | J1939控制句柄 |
| `J1939_HandleDelete` | 释放J1939控制句柄 | J1939控制句柄指针 | J1939状态 |
| `J1939_GetPortName` | 获取CAN接口名称 | J1939控制句柄 | CAN接口名称 |
| `J1939_SetSelfAdderess` | 设置设备地址 | J1939控制句柄，设备地址 | J1939状态 |
| `J1939_GetSelfAdderess` | 获取设备地址 | J1939控制句柄 | 设备地址 |
| `J1939_GetProtocolStatus` | 如果使能多帧传输协议则返回当前协议状态，否则返回错误状态 | J1939控制句柄 | J1939状态 |
| `J1939_TaskHandler` | 主控函数，用来进行J1939任务处理. 推荐5ms调用一次 | 无 | J1939状态 |
| `J1939_SendMessage` | 对J1939消息创建、发送、释放的封装 | J1939控制句柄，消息ID，消息长度，载荷指针 | J1939状态 |

</center>

- 较高的复用程度. 消息创建方法`J1939_MessageCreate()`与消息发送方法`J1939_SendMessage()`利用动态数组实现了长短报文消息创建&发送机制的复用；消息发送队列`J1939_t::TxFIFO`、虚拟总线`J1939_VirtualBus_t`、虚拟节点`J1939_VirtualNode_t`与J1939控制句柄注册器`J1939_Register_t`复用了`J1939_Queue_t`结构对应的系列方法，实现了J1939消息入队&出队、虚拟节点上线&下线、J1939控制句柄注册&注销等方法.

- 虚拟总线. 协议可通过[config/j1939_config.h](config/j1939_config.h)配置硬件衔接层为虚拟总线. 虚拟总线使得脱离硬件环境进行纯软件调试成为可能. 在虚拟总线的辅助下，用户可以方便的进行内存泄漏检测、收发回环测试、多帧消息传输协议测试，规避了调试不便的交叉编译应用场景.

- Debug log宏. 通过在config文件中设置`J1939_LOG_ENABLE`宏开启log打印功能. 调整log接口宏可指定log打印方法，将log输出到控制台、串口或者文件.

- 兼容C++.

- 轮询收发.

尽管本项目拥有现代化的代码风格，但这些做法并不符合MIRSA-C规范. 因此，开发者不为本项目的稳定性、可靠性作任何保障，任何将本项目作为商业用途而造成的损失均与开发者无关.

## 使用

推荐将本项目作为子模块，这样可以不改变本项目的原有结构.

1. 将本项目的根目录加入到加入到工程的include列表.

2. 复制`config`文件夹到本项目外，并将复制后的`config`文件夹加入到工程的include列表.

3. 将工程中所有CAN控制句柄填入[config/j1939_port.inc](config/j1939_port.inc).

4. 编写应用层代码. [config/j1939_application.c](config/j1939_application.c)提供了应用层函数接口的模板，定义如下. 此外，模板提供的所有函数接口都带有weak修饰符，这意味着允许用户在其他位置自行定义函数接口.

<center>

| 功能函数 | 描述 | 输入 | 输出 |
| --- | --- | --- | --- |
| `SoftwareFilter` | 软件滤波器 | J1939控制句柄，J1939消息 | J1939状态 |
| `DecodePayload` | 解码接收载荷 | J1939控制句柄，J1939消息 | J1939状态 |
| `SendingCallBack` | 发送成功回调 | J1939控制句柄，J1939消息 | J1939状态 |
| `MissingCallBack` | 发送失败回调 | J1939控制句柄，J1939消息 | J1939状态 |
| `ReadingCallBack` | 接收成功回调 | J1939控制句柄，J1939消息 | J1939状态 |
| `TimeoutCallBack` | 多帧传输协议超时回调 | J1939控制句柄，J1939消息 | J1939状态 |

</center>

接收消息时，功能函数的优先级由高至低依次是`ReadingCallBack` `SoftwareFilter` `DecodePayload`，即优先判定是否接收到消息，然后进行滤波，最后进行解码. 如果使能了多帧传输协议，且接收到的消息属于TP CM，则这一帧消息将在通过软件滤波器后直接入栈，不再经过解码功能函数.

5. 在工程中`#include "j1939.h"`以调用本项目提供的接口.

## 移植

本项目尚处于前期开发阶段，欢迎开发者对本项目进行移植.

目前支持的接口如下

<center>

| 接口宏 | 描述 |
| --- | --- |
| `J1939_PORT_SUSPEND` | 挂起，用于挂起协议进行调试 |
| `J1939_PORT_VIRTUAL` | 虚拟总线，用于纯软件仿真调试 |
| `J1939_PORT_STM32` | STM32全系列HAL库接口 |

</center>

## 已知问题

<center>

| 等级 | 类型 | 描述 | 状态 |
| --- | --- | --- | --- |
| low | 内存泄漏 | 在释放所有J1939控制句柄后继续调用`J1939_TaskHandler()`将导致注册表`Register()`再次申请内存. 如果后续不再使用协议，则注册表申请的内存将不会被释放. | 已修复，build1003去除了`Register()`单例模式，替换为静态全局变量，现在`J1939_TaskHandler()`会检查注册器状态，如果注册器未生成则直接返回 |

</center>

## TODO

- ~~多帧传输协议.~~
- 内存池?
- 虚拟多总线.
- 论证中断接收的可行性.

## Demo

build1007之前提供了如下demo

<center>

| Demo | 描述 |
| --- | --- |
| [demo/j1939_demo_stability.c](demo/j1939_demo_stability.c) | 重复进行“创建、使用、销毁”过程，以测试协议稳定性 |
| [demo/j1939_demo_cpp.cpp](demo/j1939_demo_cpp.cpp) | C++环境 |
| [demo/j1939_demo_protocol.c](demo/j1939_demo_protocol.c) | 多帧传输协议 |

</center>

从build1007开始，demo提供的是基于google test与mtrace的联合测试用例，具体情况见`demo`文件夹下的代码

## 参考文献

[1]丁海涛, 杨建森. SAE J1939协议[DB/OL]. [Link](http://blog.gitdns.org/2017/12/05/j1939-pctool/ourdev_509914(SAE-J1939).pdf), 2008年10月30日, 2022年02月02日

[2]Simma Software, Inc. Understanding SAE J1939[DB/OL]. [Link](https://www.simmasoftware.com/j1939-presentation.pdf), 2016年01月14日, 2022年02月12日

[3]宋超超. AUTOSAR架构的CAN通信配置之CANTP和J1939TP模块[DB/OL]. [Link](https://neyzoter.cn/2020/01/15/AUTOSAR-Can-J1939TPandCANTP-Conf/), 2020年01月15日, 2022年02月21日
