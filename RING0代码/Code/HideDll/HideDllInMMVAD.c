  VOID ShowModules(DWORD Pid)
  {
      ULONG VAD;
      PEPROCESS TargetProcess;
      PsLookupProcessByProcessId( (HANDLE)Pid,TargetProcess);
      if(!TargetProcess)
       {
          DbgPrint("[EnumModules] Error on Get EProcess By Pid.");
          return;
       }
      VAD = *(ULONG *)((ULONG)TargetProcess + VadOffset);
      DbgPrint("[EnumModules] EPROCESS : 0x%X , VAD : 0x%X",TargetProcess,VAD);
	  
      PreOrderTraverse(VAD);
      DbgPrint("[EnumModules] Modules count : %d",nCount);
  }
  
  VOID PreOrderTraverse(ULONG mmVad)
  {
      if ( MmIsAddressValid( (ULONG *)mmVad ) )
       {
          ShowPath(mmVad);
          PreOrderTraverse( *(ULONG *)(mmVad + LeftChild) );
          PreOrderTraverse( *(ULONG *)(mmVad + RightChild) );
       }
  }
  
  VOID ShowPath(ULONG mmVad)
  {
      PUNICODE_STRING pPath;
      ULONG ca;
      ULONG fp;
      ca = *(ULONG *)(mmVad + ControlArea);
      if( !MmIsAddressValid( (ULONG *)ca ) )
       {
          DbgPrint("[EnumModules] ControlArea is not available : 0x%X",ca);
          return;
       }
      fp = *(ULONG *)(ca + FilePointer);
      if( !MmIsAddressValid( (ULONG *)fp ) )
       {
          DbgPrint("[EnumModules] FileObject is not available : 0x%X",fp);
          return;
       }
      pPath = (PUNICODE_STRING)(fp + FileName);
      DbgPrint("[EnumModules] The file name is %S",pPath->Buffer);
      nCount++;
  }