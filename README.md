# 概述

该项目将通过ESP32的BLE模块将从各个外围设备读取的数据放入通知特性发送到上位机。为满足每个模块的采样率，将通过Arduino的Ticker模块周期调用特性数据读取接口函数。同时会再另设置一个控制特性，来为上位机提供控制接口，以控制下位机的数据发送的开始与停止。			 

故本项目将由两个类来实现：外围设备和esp32的BLE模块。

# 功能分析

## 外围设备
### 外围数据采集对象
- 职责描述：采集外部信号，并提供一个数据接口供ESP32进行数据读取
- 对象描述：
- 对象成员
	- 成员属性：
		- 模块名称
		- 输出数据
		- 其他
	- 成员方法
		- 初始化
		- 读取数据--->涉及到一个模块读取多种数据可以设置一个读取数据的基类，并根据不同数据进行派生。接口将返回为String对象

### 外围设备服务对象
- 职责描述：将外围设备采集的各种数据进行封装，并适当地分配给各个特性。准备通过notify或者read方法向上位机传送数据。同时留有一个借口供另外一个控制服务进行控制数据收发状态。
- 对象成员
	- 成员属性
    	- 服务UUID
    	- 特性UUID
    	- 特性属性
    	- notify状态
    	- notify开关
  	- 成员方法


## BLE

- 职责描述：通过外围设备对象提供的接口读取数据，将其进行解析、打包并发送出去。并且可以接受上位机发来的命令来控制本机的连接状态以及发送状态。
- 对象成员
	- 成员属性
		- 服务的UUID
	- 成员方法
		- 初始化
		- 
	
		

