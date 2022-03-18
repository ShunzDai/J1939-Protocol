# ChangeLog

## 2022-03-18 build1009

- 重构链表，现在`J1939_Message_t` `J1939_t` `J1939_VirtualNode_t`自身就是节点，减少了内存申请/拷贝次数
- 更新readme

## 2022-03-06 build1008

- 优化J1939消息数据结构及创建/释放的处理流程
- 更新readme

## 2022-03-04 build1007.1

- 更新测试用例
- 去除cmake内无用的语句

## 2022-03-04 build1007

- 重构cmake结构
- 重构demo，新增google test子模块，现在使用google test与mtrace联合测试
- 优化结构体成员排列顺序
- 其他不影响运行的小修改

## 2022-02-28 build1006

- 移动`src/port/j1939_memory.c/h`到`src/memory/j1939_memory.c/h`，移除LittleVGL的`memset` `memcpy`及其相关的宏定义，替换为rt-thread的实现
- 新增TP CM abort的相关接口，但尚未暴露到用户层
- 去除`virtual.c`多余的分号
- 修改demo发送的载荷
- 更新readme

## 2022-02-23 build1005

本次更新内容如下

- 优化CAN句柄注册表复用方式，并修改其命名，以区分J1939句柄注册器
- 移动虚拟CAN到`port`
- 增加多帧传输协议部分处理函数对PGN、序号等信息的判定
- 减少部分函数对空指针的判定次数
- 增加代码可读性

## 2022-02-22 build1004

本次更新对代码进行了优化. 修改内容如下

- 优化CMDT发送机制，现在对剩余包数的计数在`J1939_TP_DT_TransmitManager`中进行，主控函数`J1939_TaskHandler`的控制顺序从"多帧传输协议->轮询接收->发送"改为"轮询接收->多帧传输协议->发送". 这样做之后响应CTS的反应速度在理论上有了1个轮询周期的提升，实测build1003版本在5毫秒调用一次主控、响应CTS包数为4的情况下收发1875字节用时2260毫秒，同样的条件下build1004用时1610毫秒，响应速度有了较大提升.
- 将缩进替换为空格
- 更新readme

## 2022-02-17 build1003

本次更新对代码进行了优化. 修改内容如下

- 用宏替代队列的`J1939_QueueHead` `J1939_QueueTail`两个方法
- 去除`J1939_FuncPtr_t`类型定义，build1002以后`J1939_t`不再使用此类型；新增`J1939_Send`方法；去除虚拟总线中冗余的extern
- 优化多帧传输协议：为`ReceiveManager`系列函数增加状态检查步骤；去除`J1939_TP_DT_CMDT_TransmitManager`中冗余的三目运算；增加`J1939_ProtocolTransmitManager`函数对多帧数据大小的限制，删除没有用到的TP状态枚举值
- 移除J1939控制句柄与虚拟总线的单例模式，替换为静态全局变量. 现在`J1939_TaskHandler()`会检查注册器状态，如果注册器未生成则直接返回
- 移植LittleVGL的`memset` `memcpy`函数替换标准库函数，新增`J939_strcmp`替换标准库函数
- 优化文件引用关系
- 更新readme

## 2022-02-13 build1002

本次更新是dev版本，且不向上兼容. 修改内容如下

- 修复TP_CM_BAM接收处理产生错误PDU权重的问题. 现在BAM与CMDT接收数据的权重为0
- 新增`J1939_t`的设备地址设置/获取方法以及HAL接口名获取方法
- 重构J1939_t的应用层. 现在使用j1939_application.c对接应用层，不再使用函数指针
- 更新readme，新增changelog
- 优化多帧传输协议demo的消息ID切换方式

## 2022-02-13 build1001

本次更新是release版本. 修改内容如下

- 重构BAM传输协议
- 新增CMDT传输协议
- 修改部分结构体、枚举类型以及它们的成员的命名，修改J1939_t结构体成员指针指向的函数接口的格式，修改demo文件名，修改J1939_SetPGN()函数的返回值
- 更新readme，新增测试图片