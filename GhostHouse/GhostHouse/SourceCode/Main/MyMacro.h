#pragma once

//===========================================================
//	Ï¸Û.
//===========================================================

//‰ð•ú.
#define SAFE_RELEASE(p)	if(p!=nullptr){(p)->Release();(p)=nullptr;}
//”jŠü.
#define SAFE_DELETE(p) if(p!=nullptr){delete (p);(p)=nullptr;}

#define SAFE_DELETE_ARY(p) if(p!=nullptr){delete[] (p);(p)=nullptr;}

#define ALIGN16	_declspec(align(16))