#include "efi.h"
void read_line(EFI_SYSTEM_TABLE *st, CHAR16 *buf, UINTN max) {
    UINTN len = 0;
    EFI_INPUT_KEY key;
    CHAR16 echo[2] = {0, 0};

    for (;;) {
        if (st->ConIn->ReadKeyStroke(st->ConIn, &key) != EFI_SUCCESS)
            continue;
        if (key.ScanCode == 0x16)
            st->RuntimeServices->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);

            if (key.UnicodeChar == u'\r') {
                st->ConOut->OutputString(st->ConOut, u"\r\n");
                break;
            }
            if (key.UnicodeChar == u'\b') {
                if (len > 0) {
                    len--;
                    st->ConOut->OutputString(st->ConOut, u"\b \b");
                }
                continue;
            }
            if (key.UnicodeChar >= u' ' && len < max - 1) {
                buf[len++] = key.UnicodeChar;
                echo[0] = key.UnicodeChar;
                st->ConOut->OutputString(st->ConOut, echo);
            }
    }
    buf[len] = 0;
}
int str_eq(CHAR16 *a, CHAR16 *b) {
    while (*a && *a == *b) {
        a++;
        b++;
    }
    return *a == *b;
}
//efi entry point
EFI_STATUS EFIAPI /*return status*/ efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) { //basically int(main)
    (void)ImageHandle; //prevent compiler from getting mad
    EFI_SIMPLE_TEXT_OUTPUT_PROTOCOL *out = SystemTable->ConOut;
    UINTN best = 0, bestSize = 0, cols, rows;
    for (UINTN i = 0; i < (UINTN)out->Mode->MaxMode; i++) {
        if (out->QueryMode(out, i, &cols, &rows) == EFI_SUCCESS && cols * rows > bestSize) {
            bestSize = cols * rows;
            best = i;
        }
    }
    out->SetMode(out, best);
    SystemTable->ConOut->SetAttribute(SystemTable->ConOut,
                                     EFI_TEXT_ATTR(EFI_GREEN,EFI_BLACK));
    SystemTable->ConOut->ClearScreen(SystemTable->ConOut); //clear screen
    SystemTable->ConOut->OutputString(SystemTable->ConOut, u"basic command line, type a command and if it has an argument, press enter then enter the argument then press enter again.\r\n\r\n");
    SystemTable->ConOut->OutputString(SystemTable->ConOut,
                                     u"press F12 to DIE\r\n");
    SystemTable->ConOut->EnableCursor(SystemTable->ConOut, 1); // show cursor
    EFI_INPUT_KEY key;
    CHAR16 inp[128];
    CHAR16  f[128];
    CHAR16 h[128];

for (;;) {
    read_line(SystemTable, f, 128);
    if (str_eq(f, u"echo")) {
        SystemTable->ConOut->SetAttribute(SystemTable->ConOut,
                                          EFI_TEXT_ATTR(EFI_RED,EFI_BLACK));
        read_line(SystemTable, h, 128);
        SystemTable->ConOut->SetAttribute(SystemTable->ConOut,
                                          EFI_TEXT_ATTR(EFI_GREEN,EFI_BLACK));
        out->OutputString(out, h);
        out->OutputString(out, u"\r\n");
    }

}
    return EFI_SUCCESS;
}
