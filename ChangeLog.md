# ChangeLog

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