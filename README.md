# wincode
过去写的一些Windows安全研究相关代码


ring0安全：

```
APC                                 HookNtCreateSectionProtectProcess.c Ring0ChangePEB
CALLGATE                            HookZwQueryDirectoryFile            SSDTHook
ChangeModulePath.cpp                IDTCALL                             SYSENTERHook
CloseMyHandle                       IDTHook                             modifyiopmbase
EAT Hook                            IRPHook                             porttalk
EPROCESS                            Inline Hook                         worker_queue.c
FindProcess                         InlineHookObReferenced              内核注入dll
GetNtoskrnlexe                      KernelAndUserHook                   更改TSS IO位图大小
HideDll                             KillThread                          驱动更改iopl
HideReg.c                           PspCidTable
```

ring3技巧：

```
ArpChect               PAGE_GURAD保护数据     U盘木马                三级跳                 线程守护
DNSniffer              PE木马                 WinPcapSniffer         单实例运行             自动运行
DealSC                 QV                     client                 反向连接               键盘监控
DownLoadFile           Reg                    findOpt                服务启动
ImprovePrivilege.cpp   SHELLCODE              sethc_door             端口复用
InjectIE               UnloadDll              takeowner.cpp          管理服务
```
