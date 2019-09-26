//HEADER_GOES_HERE
#ifndef __DX_H__
#define __DX_H__

extern BYTE *gpBuffer;
extern char gbBackBuf;
extern char gbEmulate;
extern HMODULE ghDiabMod;

void dx_init(HWND hWnd);
void dx_create_back_buffer();
void dx_create_primary_surface();
void lock_buf(BYTE idx);
void lock_buf_priv();
void unlock_buf(BYTE idx);
void unlock_buf_priv();
void dx_cleanup();
void dx_reinit();
void j_dx_reinit();

HRESULT CreatePalette();
HRESULT BltFast(DWORD dwX, DWORD dwY, LPRECT lpSrcRect);
HRESULT RenderPresent();
void PaletteGetEntries(DWORD dwNumEntries, LPPALETTEENTRY lpEntries);
void PaletteSetEntries(DWORD dwCount, LPPALETTEENTRY lpEntries);

/* data */

#endif /* __DX_H__ */
