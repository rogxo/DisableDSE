
# DisableDSE

### DSE Call Stack
```cpp
nt!SeValidateImageHeader
nt!MiValidateSectionCreate+0x436
nt!MiValidateSectionSigningPolicy+0xa6
nt!MiCreateNewSection+0x5ad
nt!MiCreateImageOrDataSection+0x2d0
nt!MiCreateSection+0xf4
nt!MiCreateSystemSection+0xa0
nt!MiCreateSectionForDriver+0x125
nt!MiObtainSectionForDriver+0xa6
nt!MmLoadSystemImageEx+0xd7
nt!MmLoadSystemImage+0x26
nt!IopLoadDriver+0x224
nt!IopLoadUnloadDriver+0x4e
nt!ExpWorkerThread+0x105
nt!PspSystemThreadStartup+0x55
nt!KiStartSystemThread+0x2a
```

### SeValidateImageHeader and SeValidateImageData

```cpp
nt!SeValidateImageHeader:
fffff805`3b882f20 488bc4          mov     rax,rsp
fffff805`3b882f23 48895808        mov     qword ptr [rax+8],rbx
fffff805`3b882f27 48897010        mov     qword ptr [rax+10h],rsi
fffff805`3b882f2b 57              push    rdi
fffff805`3b882f2c 4881eca0000000  sub     rsp,0A0h
fffff805`3b882f33 33f6            xor     esi,esi
fffff805`3b882f35 488bda          mov     rbx,rdx
fffff805`3b882f38 4839354155dbff  cmp     qword ptr [nt!SeCiCallbacks+0x20 (ffff805`3b638480)],rsi
fffff805`3b882f3f 488bf9          mov     rdi,rcx
fffff805`3b882f42 488970f0        mov     qword ptr [rax-10h],rsi
fffff805`3b882f46 448bde          mov     r11d,esi
fffff805`3b882f49 0f8451301500    je      nt!SeValidateImageHeader+0x153080 (fffff805`3b9d5fa0)
fffff805`3b882f4f 8b9424f0000000  mov     edx,dword ptr [rsp+0F0h]
fffff805`3b882f56 f6c201          test    dl,1
fffff805`3b882f59 0f85c1000000    jne     nt!SeValidateImageHeader+0x100 (fffff805`3b883020)
...

nt!SeValidateImageData:
fffff805`3b8833a0 4883ec48        sub     rsp,48h
fffff805`3b8833a4 488b05dd50dbff  mov     rax,qword ptr [nt!SeCiCallbacks+0x28 (fffff805`3b638488)]
fffff805`3b8833ab 4c8bd1          mov     r10,rcx
fffff805`3b8833ae 4885c0          test    rax,rax
fffff805`3b8833b1 741f            je      nt!SeValidateImageData+0x32 (fffff805`3b8833d2)
fffff805`3b8833b3 488b4c2478      mov     rcx,qword ptr [rsp+78h]
fffff805`3b8833b8 48894c2428      mov     qword ptr [rsp+28h],rcx
fffff805`3b8833bd 8b4c2470        mov     ecx,dword ptr [rsp+70h]
fffff805`3b8833c1 894c2420        mov     dword ptr [rsp+20h],ecx
fffff805`3b8833c5 498bca          mov     rcx,r10
fffff805`3b8833c8 e86369b4ff      call    nt!guard_dispatch_icall (fffff805`3b3c9d30)
fffff805`3b8833cd 4883c448        add     rsp,48h
fffff805`3b8833d1 c3              ret
```

nt!SeCiCallbacks+0x20 -> Address of CiValidateImageHeader
nt!SeCiCallbacks+0x28 -> Address of CiValidateImageData 
